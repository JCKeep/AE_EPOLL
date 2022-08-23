# ~~Redis设计与实现~~


## 基础数据结构


### sds (Simple Dynamic String)


#### 数据结构定义

```c
typedef struct sdshdr {
    // 记录sds保存的字符串长度
    int len;
    // 记录buf中未使用的字节数
    int free;
    // 采取C字符串以'\0'结尾的习惯，能够重用部分标准C字符串的函数
    char buf[];
}
```

#### sds动态空间分配策略

1. 空间预分配

   * 当对sds进行修改后，sds的长度小于1MB，此时分配内存会预留`free=len`长度的空间，所以会分配`free + len + 1`长度的字符串。
   * 当对sds进行修改后，sds的长度大于1MB，此时会分配`free=1MB`的预留空间。

2. 惰性删除

   * 当对sds进行删减操作时，多余的内存不会被回收，而是由free记录空闲空间。

#### 二进制安全

所以sds的api都会以处理二进制的方式来处理sds，所以即使sds字符串中间存储了'\0'也不会被误判，因为真正判断结尾的为`sdshdr->len`，因而sds也可以用于存储二进制文件。

<!-- #### sds api

|函数|功能|复杂度|
|-|-|-|
|sdsnew|创建一个包含给定C字符串的sds|O($N$)|
|sdsempty|创建一个不包含内容的空sds|O($1$)|
|sdsfree|释放给定sds空间|O($N$)|
|sdslen|返回sds已使用的字节数|O($1$)|
|sdsavail|返回sds未使用的字节数|O($1$)|
|sdsdup|创建一个sds的副本|O($N$)|
|sdsclear|情况sds字符串的内容|O($1$)|
|sdscat|将C字符串拼接到sds字符串末尾|O($N$)|
|sdscatsds|拼接两个sds字符串|O($N$)|
|sdscpy|将给定C字符串复制到sds，覆盖原有字符串|O($N$)|
|sdsgrowzero|用空字符扩展到sds所有位置|O($N$)|
|sdsrange|保留sds给定位置的字符，不在区间内的被覆盖或清除|O($N$)|
|sdstrim|从sds中溢出给定的所以出现过的字符|O($N^2$)|
|sdscmp|比较两个sds字符串|O($N$)| -->


### 链表

#### 数据结构定义

```c
typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    // 用void指针存储数据，以实现多类型
    void *value;
} listNode;

typedef struct list {
    listNode *head;
    listNode *tail;
    // 复制函数，用于创建一个list的副本
    void *(*dup)(void *ptr);
    // 内存释放函数
    void (*free)(void *ptr);
    // listNode->value的值比较函数
    int (*match)(void *ptr, void *key);
    unsigned long len;
} list;
```

list实现多类型的方式为用`void *`指针存储值，以及每个值对应的`dup, free, match`函数来实现存储多类型，因为C语言不支持泛型，所以Redis需要手动实现泛型。
当list存储的值为int整型时，会将`void *`转为`long`(指针与`long`的位数相同)来保存，所以在Redis中整型返回值都是以`long`来表示的。


### 字典

#### 数据结构定义

```c
struct dict {
    // 描述字典存储类型相关的结构
    dictType *type;

    // 包含两个hashtable, ht_table[0]为主表，ht_table[1]主要用于对ht_table[0]进行rehash扩容
    dictEntry **ht_table[2];
    // 描述哈希表的节点数量
    unsigned long ht_used[2];

    // rehash下标，当开始rehash是，rehashidx=0，依次对每个ht_table[0]的哈希桶进行rehash
    // 记录rehash的进度，未进行为-1
    long rehashidx; /* rehashing not in progress if rehashidx == -1 */

    /* Keep small vars at end for optimal (minimal) struct padding */
    int16_t pauserehash; /* If >0 rehashing is paused (<0 indicates coding error) */
    // 用于记录hashtable的大小，每次扩容逗我2的幂次方，这样进行哈希映射可以使用位运算，加快运算效率
    signed char ht_size_exp[2]; /* exponent of size. (size = 1<<exp) */
};

typedef struct dictEntry {
    // 键，用void *存储以支持多类型
    void *key;
    // 值，用联合体保存，提高空间利用率
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v;
    // 拉链法解决
    struct dictEntry *next;     /* Next entry in the same hash bucket. */
    void *metadata[];           /* An arbitrary number of bytes (starting at a
                                 * pointer-aligned address) of size as returned
                                 * by dictType's dictEntryMetadataBytes(). */
} dictEntry;

typedef struct dictType {
    uint64_t (*hashFunction)(const void *key);
    void *(*keyDup)(dict *d, const void *key);
    void *(*valDup)(dict *d, const void *obj);
    int (*keyCompare)(dict *d, const void *key1, const void *key2);
    void (*keyDestructor)(dict *d, void *key);
    void (*valDestructor)(dict *d, void *obj);
    int (*expandAllowed)(size_t moreMem, double usedRatio);
    /* Allow a dictEntry to carry extra caller-defined metadata.  The
     * extra memory is initialized to 0 when a dictEntry is allocated. */
    size_t (*dictEntryMetadataBytes)(dict *d);
} dictType;
```

#### rehash

为了让哈希表的负载因子(load factor)保持在一定范围内，避免哈希冲突过多影响效率，程序会根据条件对哈希表大小进行扩展或收缩。
扩展或收缩哈希表可以通过执行`rehash`来进行，相应步骤如下: 
1. 为字典`ht_table[1]`哈希表分配空间，这个哈希表的空间大小取决于要执行的操作以及`ht_table[0]`哈希表的大小
   * 执行扩展操作，那么`ht_table[1]`的大小为min $2^N$ >= ht_used[0] * 2
   * 执行收缩操作，那么`ht_table[1]`的大小为min $2^N$ >= ht_used[0]
2. 将保存在`ht_table[0]`的值重新计算哈希值复制到`ht_table[1]`上
3. 删除`ht_table[0]`，并将`ht_table[0]`设为`ht_table[1]`，并新建一个空的`ht_table[1]`，为下次`rehash`做准备

哈希表的扩展与收缩

1. 扩展
   * 当前没有执行`BGSAVE`和`BGREWRITEAOF`操作，并且哈希表负载因子大于等于1
   * 当前正在执行`BGSAVE`和`BGREWRITEAOF`操作，并且哈希表负载因子大于等于5
2. 收缩
   * 当哈希表负载因子小于0.1时

* 渐进式`rehash`
  
  *哈希表执行`rehash`操作不是一次性进行全部，而是均摊到每次增删改查中，以避免哈希表太大`rehash`的时间消耗，`rehash`期间增删改查会同时`ht_table[0]`, `ht_table[1]`上进行，但新增数据只会增加在`ht_table[1]`上，保证`ht_table[0]`不会新增节点。*


### 整数集合

#### 数据结构定义

```c
typedef struct intset {
    // 整数编码方式
    uint32_t encoding;
    // 包含的元素个数
    uint32_t length;
    // 保存元素的数组
    int8_t contents[];
} intset;
```

|encoding|contents type|
|-|-|
|INTSET_ENC_INT16|int16_t|
|INTSET_ENC_INT32|int32_t|
|INTSET_ENC_INT64|int64_t|

*整数类型动态升级*

contents声明为int8_t类型，但是intset并不保存int8_t类型的数据，仅仅只是将int8_t类型当作字节数组来使用。当我们向intset中加入超过当前数据类型的数据时，intset会自动将数据类型升级，并重新分配contents的空间，并按照新类型大小存放(均基于小端法存放)。intset不支持类型降级操作。


### 压缩列表

#### 数据结构定义

1. ZIPLIST

```c
/**
 * 压缩列表是定义在一个连续内存块的数据结构，下面为Ziplist的各个组成部分
 * <zlbytes> <zltail> <zllen> <entry1> <entry2> ... <entryN> <zlend>
 */
```

|属性|类型|长度(byte)|用途|
|-|-|-|-|
|zlbytes|uint32_t|4|记录整个压缩列表所占用的字节数: 在对压缩列表进行内存重分配或者计算zlend的位置时使用|
|zltail|uint32_t|4|记录压缩列表尾节点距离起始地址有多少字节: 通过偏移量能够快速确定尾节点位置|
|zllen|uint16_t|2|记录压缩列表包含的节点数量，当zllen < UINT16_MAX时，代表节点数量，大于时需要遍历才能确定|
|entryN|列表节点|不定|压缩列表包含的各个节点，长度由存储的内容决定|
|zlend|uint8_t|1|特殊值0xFF，用于标记列表结尾|


2. ZIPLIST ENTRY

```c
/**
 * ziplist entry节点定义
 * <prevlen> <encoding> <entry-data>
 */
```

* prevlen(记录前一节点的大小)
  * 如果前一节点大小小于254字节，那么占用内存为1字节
  * 如果前一节点大小大于等于254字节，那么占用5字节的内存，第一字节被设置为0xFE(254)，之后四字节记录大小

> 这么设计的原因主要为实现双向链表功能，记录前一个节点的大小能够轻易地计算出前一个节点的位置，zltail记录最后一个节点，进而能够
> 从后向前遍历; 当从前往后遍历时，只需判断第一字节是否为0xFE以及encoding即可计算出下一节点的位置，从而实现了简单的双向链表功能。


|编码|编码长度(byte)|content属性保存值|
|-|-|-|
|00bbbbbb|1|长度小于$2^6$的字节数组|
|01bbbbbb xxxxxxxx|2|长度小于$2^{14}$的字节数组|
|10______ bbbbbbbb cccccccc dddddddd xxxxxxxx|5|长度小于$2^{32}$的字符数组|
|11000000|1|int16_t|
|11010000|1|int32_t|
|11100000|1|int64_t|
|11110000|1|24位有符号整数|
|11111110|1|8位有符号整数|
|1111xxxx|1|0~12, 无content|

* 因为prevlen记录前一个节点大小可能需要1或5字节，所以当对ziplist修改的时候可能会发生连锁反应，造成大量小节点重新分配空间，降低时间复杂度位$O(N^2)$


### 对象

#### 数据结构定义

```c
typedef struct redisObject {
    // 对象类型
    unsigned type:4;
    // 对象编码和底层数据结构实现
    unsigned encoding:4;
    unsigned lru:LRU_BITS; /* LRU time (relative to global lru_clock) or
                            * LFU data (least significant 8 bits frequency
                            * and most significant 16 bits access time). */
    // 引用计数器
    int refcount;
    // 指向底层数据结构的指针
    void *ptr;
} robj;
```

类型

|类型常量|名称|
|-|-|
|REDIS_STRING|字符串对象|
|REDIS_LIST|列表对象|
|REDIS_HASH|哈希对象|
|REDIS_SET|集合对象|
|REDIS_ZSET|有序集合对象|

编码

|类型|编码|对象|
|-|-|-|
|REDIS_STRING|REDIS_ENCODING_INT|使用整数值实现的字符串对象|
|REDIS_STRING|REDIS_ENCODING_EMBSTR|使用embstr编码的sds实现的字符串对象|
|REDIS_STRING|REDIS_ENCODING_RAW|使用sds实现的字符串对象|
|REDIS_LIST|REDIS_ENCODING_ZIPLIST|使用压缩列表实现的列表对象|
|REDIS_LIST|REDIS_ENCODING_LINKEDLIST|使用双端链表实现的列表对象|
|REDIS_HASH|REDIS_ENCODING_ZIPLIST|使用ziplist编码实现的哈希对象|
|REDIS_HASH|REDIS_ENCODING_HT|使用字典实现的哈希对象|
|REDIS_SET|REDIS_ENCODING_INTSET|使用intset实现的集合对象|
|REDIS_SET|REDIS_ENCODING_HT|使用字典实现的集合对象|


## 单机数据库实现

### 数据库

```c
/* Redis database representation. There are multiple databases identified
 * by integers from 0 (the default database) up to the max configured
 * database. The database number is the 'id' field in the structure. */
typedef struct redisDb
{
    dict *dict;                             /* The keyspace for this DB */
    dict *expires;                          /* Timeout of keys with a timeout set */
    dict *blocking_keys;                    /* Keys with clients waiting for data (BLPOP)*/
    dict *ready_keys;                       /* Blocked keys that received a PUSH */
    dict *watched_keys;                     /* WATCHED keys for MULTI/EXEC CAS */
    int id;                                 /* Database ID */
    long long avg_ttl;                      /* Average TTL, just for stats */
    unsigned long expires_cursor;           /* Cursor of the active expire cycle. */
    list *defrag_later;                     /* List of key names to attempt to defrag one by one, gradually. */
    clusterSlotToKeyMapping *slots_to_keys; /* Array of slots to keys. Only used in cluster mode (db 0). */
} redisDb;
```


### RDB文件结构

```c
/**
 * <REDIS> <version> <databases> <EOF> <check_number>
 *                   /         \
 *                  /           \
 * <SELECTDB> <db_number> <key_value_pairs> ...
 *                        /               \
 *                       /                 \
 * 1. persist           <TYPE> <key> <value>
 * 2. expire  <EXPIRETIME_MS> <ms> <TYPE> <key> <value>
 */
```

TYPE类型决定value的编码，key为STRING对象

```c
/**
 * TYPE                             value
 * REDIS_RDB_TYPE_STRING            <len> <string>
 * REDIS_RDB_TYPE_LIST              <list_size> <item1> <item2> ... <itemN>
 * REDIS_RDB_TYPE_SET               <set_size> <elem1> <elem2> ... <elemN>
 * REDIS_RDB_TYPE_ZSET              <zset_size> <elem1> <elem2> ... <elemN>
 * REDIS_RDB_TYPE_HASH              <hash_size> <key1> <value1> ... <keyN> <valueN>
 * REDIS_RDB_TYPE_LIST_ZIPLIST      转换成STRING对象保存
 * REDIS_RDB_TYPE_SET_INTSET        转换成STRING对象保存
 * REDIS_RDB_TYPE_HASH_ZIPLIST      转换成STRING对象保存
 */
```

以上每个elem都一个为STRING对象
