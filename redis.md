# **Redis**

## 目录

[Redis基本操作](#redis基本操作)

- [Key](#key)
- [String](#string)
- [List](#list)
- [Hash](#hash)
- [Set](#set)
- [Zset](#zset)
- [HyperLoglog](#hyperloglog)
- [Publish/Subscribe](#publishsubscribe)
- [Redis GEO](#redis-geo地理位置)
- [Redis事务](#redis事务)
- [Lua Script](#redis-lua脚本)
- [布隆过滤器](#redis-布隆过滤器)

[Redis相关应用](#application)

- [缓存](#缓存)
- [排行榜](#排行榜)
- [计数器](#计数器)
- [Redis分布式锁](#redis分布式锁httpsjuejincnpost6961380552519712798)
  - [由`SETNX`和`LUA Script`实现的简单锁](#简单由setnx和lua脚本实现的锁)
  - [Redis可重入锁](#redis可重入锁)
- [消息队列 & 延迟队列](#延时队列--消息队列)
- [Redis三大缓存问题](#redis缓存穿透-缓存击穿-缓存雪崩)
- [Redis限流算法](#redis限流算法)

[SpringBoot Redis](#springboot集成redis)

[Redis持久化策略](#redis持久化策略)

[Redis Cluster](#redis主从哨兵clusterredis-clustermd)

- [主从架构](#redis主从架构)
- [~~哨兵架构~~](#redis哨兵)
- [Redis Cluster](#redis-cluster)

[Redis配置项汇总](#redis配置项汇总)


## Redis基本操作

### Key

| Command           | Description                                     |
| ----------------- | ----------------------------------------------- |
| DEL key           | 若键存在的情况下，该命令用于删除键                               |
| DUMP key          | 用于序列化给定 key ，并返回被序列化的值                          |
| EXISTS key        | 用于检查键是否存在，若存在则返回1，否则返回0                         |
| EXPIRE key        | 设置key的过期时间，以秒为单位                                |
| PEXPIRE key       | 设置key的过期时间，以毫秒为单位                               |
| KEYS pattern      | 此命令用于查找与指定 pattern 匹配的 key                      |
| MOVE key db       | 将当前数据库中的 key 移动至指定的数据库中（默认存储为 0 库，可选 1-15中的任意库） |
| PERSIST key       | 该命令用于删除 key 的过期时间，然后 key 将一直存在，不会过期             |
| TTL key           | 用于检查 key 还剩多长时间过期，以秒为单位                         |
| RANDOMKEY         | 从当前数据库中随机返回一个 key                               |
| RENAME key newkey | 修改 key 的名称                                      |
| TYPE key          | 该命令用于获取 value 的数据类型                             |

* *Redis*数据淘汰策略

> 可以设置内存最大使用量，当内存使用量超出时，会施行数据淘汰策略。

| 策略              | 描述                         |
| --------------- | -------------------------- |
| volatile-lru    | 从已设置过期时间的数据集中挑选最近最少使用的数据淘汰 |
| volatile-ttl    | 从已设置过期时间的数据集中挑选将要过期的数据淘汰   |
| volatile-random | 从已设置过期时间的数据集中任意选择数据淘汰      |
| allkeys-lru     | 从所有数据集中挑选最近最少使用的数据淘汰       |
| allkeys-random  | 从所有数据集中任意选择数据进行淘汰          |
| noeviction      | 禁止驱逐数据                     |

### String

| Command                   | Description                                       |
| ------------------------- | ------------------------------------------------- |
| SET key value             | 用于设定指定键的值                                         |
| GET key                   | 用于检索指定键的值                                         |
| GETRANGE key start end    | 返回 key 中字符串值的子字符                                  |
| GETSET key value          | 将给定 key 的值设置为 value，并返回 key 的旧值                   |
| GETBIT key offset         | 对 key 所存储的字符串值，获取其指定偏移量上的位                        |
| MGET key1 [key2...]       | 批量获取一个或多个 key 所存储的值，减少网络耗时开销                      |
| SETBIT key offset value   | 对 key 所储存的字符串值，设置或清除指定偏移量上的位                      |
| SETEX key seconds value   | 将值 value 存储到 key中 ，并将 key 的过期时间设为 seconds (以秒为单位) |
| SETNX key value           | 当 key 不存在时设置 key 的值                               |
| SETRANGE key offset value | 从偏移量 offset 开始，使用指定的 value 覆盖的 key 所存储的部分字符串值     |
| STRLEN key                | 返回 key 所储存的字符串值的长度                                |
| INCR key                  | 将 key 所存储的整数值加 1                                  |
| DECR key                  | 将 key 所存储的整数值减 1                                  |
| APPEND key value          | 该命令将 value 追加到 key 所存储值的末尾                        |

### List

| Command                              | Description                                                  |
| ------------------------------------ | ------------------------------------------------------------ |
| LPUSH key value1 [value2]            | 在列表头部插入一个或者多个值                                               |
| LRANGE key start end                 | 获取列表指定范围内的元素                                                 |
| RPUSH key value1 [value2]            | 在列表尾部添加一个或多个值                                                |
| LPUSHX key value                     | 当储存列表的 key 存在时，用于将值插入到列表头部                                   |
| LINDEX key index                     | 通过索引获取列表中的元素                                                 |
| LINSERT key before/after pivot value | 指定列表中一个元素在它之前或之后插入另外一个元素                                     |
| LPOP key                             | 从列表的头部弹出元素，默认为第一个元素                                          |
| LLEN key                             | 用于获取列表的长度                                                    |
| BLPOP key1 [key2] timeout            | 用于删除并返回列表中的第一个元素（头部操作），如果列表中没有元素，就会发生阻塞，直到列表等待超时或发现可弹出元素为止   |
| BRPOPLPUSH src dest timeout          | 从列表中取出最后一个元素，并插入到另一个列表的头部。如果列表中没有元素，就会发生阻塞，直到等待超时或发现可弹出元素时为止 |
| LTRIM key start stop                 | 保留列表中指定范围内的元素值                                               |

### Hash

| Command                     | Description                      |
| --------------------------- | -------------------------------- |
| HDEL key field1 [field2]    | 用于删除一个或多个哈希表字段                   |
| HEXISTS key field           | 用于确定哈希表字段是否存在                    |
| HGET key field              | 获取 key 关联的哈希字段的值                 |
| HGETALL key                 | 获取 key 关联的所有哈希字段值                |
| HINCRBY key field increment | 给 key 关联的哈希字段做整数增量运算             |
| HKEYS key                   | 获取 key 关联的所有字段和值                 |
| HLEN key                    | 获取 key 中的哈希表的字段数量                |
| HSET key field value        | 用于设置指定 key 的哈希表字段和值（field/value） |
| HVALS key                   | 用于获取哈希表中的所有值                     |

### Set

| Command                       | Description                 |
| ----------------------------- | --------------------------- |
| SADD key member1 [member2...] | 向集合中添加一个或者多个元素，并且自动去重       |
| SCARD key                     | 返回集合中元素的个数                  |
| SDIFF key1 [key2]             | 求两个或多个集合的差集                 |
| SDIFFSTORE dest key1 [key2]   | 求两个集合或多个集合的差集，并将结果保存到指定的集合中 |
| SINTER key1 [key2]            | 求两个或多个集合的交集                 |
| STINTERSTORE dest key1 [key2] | 求两个或多个集合的交集，并将结果保存到指定的集合中   |
| SISMEMBER key member          | 查看指定元素是否存在于集合中              |
| SMEMBERS key                  | 查看集合中所有元素                   |
| SMOVE src dest member         | 将集合中的元素移动到指定的集合中            |
| SPOP key [count]              | 弹出指定数量的元素                   |
| SUNION key1 [key2]            | 求两个或者多个集合的并集                |
| SREM key member1 [member2]    | 删除一个或者多个元素，若元素不存在则自动忽略      |

### Zset

| Command                                       | Description                           |
| --------------------------------------------- | ------------------------------------- |
| ZADD key score1 member1 [score2 member2]      | 用于将一个或多个成员添加到有序集合中，或者更新已存在成员的 score 值 |
| ZCARD key                                     | 获取有序集合中成员的数量                          |
| ZCOUNT key min max                            | 用于统计有序集合中指定 score 值范围内的元素个数           |
| ZINCRBY key increment member                  | 用于增加有序集合中成员的分值                        |
| ZINTERSCORE dest numkeys key [key...]         | 求两个或者多个有序集合的交集，并将所得结果存储在新的 key 中      |
| ZRANGE key start stop [WITHSCORES]            | 返回有序集合中指定索引区间内的成员数量                   |
| ZRANGEBYLEX key min max [LIMIT offset count]  | 返回有序集中指定字典区间内的成员数量                    |
| ZRANGEBYSCORE key min max [WITHSCORE] [LIMIT] | 返回有序集合中指定分数区间内的成员                     |
| ZRANK key member                              | 返回有序集合中指定成员的排名                        |
| ZREM key member [member...]                   | 移除有序集合中的一个或多个成员                       |
| ZREMRANGEBYSCORE key min max                  | 移除有序集合中指定分数区间内的所有成员                   |
| ZREMRANGEBYRANK key start stop                | 移除有序集合中指定排名区间内的所有成员                   |
| ZSCORE key member                             | 返回有序集中，指定成员的分数值                       |
| ZREVRANK key member                           | 返回有序集合中指定成员的排名(倒排名)                   |
| ZUNIONSTORE dest numkeys key [key...]         | 求两个或多个有序集合的并集，并将返回结果存储在新的 key 中       |

### HyperLoglog

| Command                                  | Description                   |
| ---------------------------------------- | ----------------------------- |
| PFADD key element [element...]           | 添加指定元素到 HyperLogLog key 中     |
| PFCOUNT key [key...]                     | 返回指定 HyperLogLog key 的基数估算值   |
| PFMERGE destkey sourcekey [sourcekey...] | 将多个 HyperLogLog key 合并为一个 key |

### Publish/Subscribe

| Command                                     | Description                                                                      |
| ------------------------------------------- | -------------------------------------------------------------------------------- |
| PSUBSCRIBE pattern [pattern...]             | 订阅一个或多个符合指定模式的频道                                                                 |
| PUBSUB subcommand [argument [argument...]] | 查看发布/订阅系统状态，可选参数1.channel 返回在线状态的频道。1.numpat 返回指定模式的订阅者数量。3.numsub 返回指定频道的订阅者数量。 |
| PUBSUB subcommand [argument [argument...]] | 将信息发送到指定的频道                                                                      |
| PUNSUBSCRIBE [pattern [pattern...]]        | 退订所有指定模式的频道                                                                      |
| SUBSCRIBE channel [channel...]             | 订阅一个或者多个频道的消息                                                                    |
| UNSUBSCRIBE [channel [channel...]]         | 退订指定的频道                                                                          |

### Redis GEO地理位置

* Redis GEO 有很多应用场景，举一个简单的例子，你一定点过外卖，或者用过打车软件，在这种 APP上会显示“店家距离你有多少米”或者“司机师傅距离你有多远”，类似这种功能就可以使用 Redis GEO 实现。数据库中存放着商家所处的经纬度，你的位置则由手机定位获取，这样 APP 就计算出了最终的距离。再比如微信中附近的人、摇一摇、实时定位等功能都依赖地理位置实现。

| Command           | Description                             |
| ----------------- | --------------------------------------- |
| GEOADD            | 将指定的地理空间位置（纬度、经度、名称）添加到指定的 key 中        |
| GEOPOS            | 从 key 里返回所有给定位置元素的位置（即经度和纬度）            |
| GEODIST           | 返回两个地理位置间的距离，如果两个位置之间的其中一个不存在， 那么命令返回空值 |
| GEORADIUS         | 根据给定地理位置坐标(经纬度)获取指定范围内的地理位置集合           |
| GEORADIUSBYMEMBER | 根据给定地理位置(具体的位置元素)获取指定范围内的地理位置集合         |
| GEOHASH           | 获取一个或者多个的地理位置的 GEOHASH 值                |
| ZREM              | 通过有序集合的 zrem 命令实现对地理位置信息的删除             |

### Redis事务

* 单独的隔离操作
* 不保证原子性

| Command            | Description                                      |
| ------------------ | ------------------------------------------------ |
| MULTI              | 开启一个事务                                           |
| EXEC               | 执行事务中的所有命令                                       |
| WATCH key [key...] | 在开启事务之前用来监视一个或多个key 。如果事务执行时这些 key 被改动过，那么事务将被打断 |
| DISCARD            | 取消事务                                             |
| UNWATCH            | 取消 WATCH 命令对 key 的监控                             |

### Redis Lua脚本

| Command                                         | Description                    |
| ----------------------------------------------- | ------------------------------ |
| EVAL script numkeys keys [key...] argv [arg...] | 执行lua脚本                        |
| SCRIPT LOAD script                              | 将脚本 script 添加到脚本缓存中，但不马上执行这个脚本 |
| SCRIPT FLUSH                                    | 从脚本缓存中移除所有脚本                   |
| SCRIPT KILL                                     | 杀死当前正在运行的 Lua 脚本               |

* SpringBoot运行lua脚本

```java
    /* 先定义RedisScript类，返回类型为Long，因为Redis不支持Integer(不一定需要声明为Bean) */
    @Bean
    public RedisScript<Long> redisLua() {
        DefaultRedisScript<Long> redisScript = new DefaultRedisScript<>();
        redisScript.setScriptSource(new ResourceScriptSource(new ClassPathResource("redis/Unlock.lua")));
        redisScript.setResultType(Long.class);
        return redisScript;
    }

    /* 运行脚本 */
    stringRedisTemplate.execute(redisScript, List.of(key), args...);
```

### Redis 布隆过滤器

相比于Set集合的去重功能而言，布隆过滤器在空间上能节省90%以上，但是它的不足之处是去重率大约在99%左右，也就是说有1%左右的误判率，这种误差是由布隆过滤器的自身结构决定的。俗话说“鱼与熊掌不可兼得”，如果想要节省空间，就需要牺牲1%的误判率，而且这种误判率，在处理海量数据时，几乎可以忽略。

![bloom](./source/redis/bllom.gif)

当使用布隆过滤器添加key时，会使用不同的hash函数对key存储的元素值进行哈希计算，从而会得到多个哈希值。根据哈希值计算出一个整数索引值，将该索引值与位数组长度做取余运算，最终得到一个位数组位置，并将该位置的值变为1。每个hash函数都会计算出一个不同的位置，然后把数组中与之对应的位置变为1。通过上述过程就完成了元素添加(add)操作。

特点: 
* 判断一个元素不存在其一定不存在
* 但判断存在会出现一定概率的误判，并且随着数据量变大出错的概率也随之升高

|Command|Description|
|-|-|
|BF.ADD|添加元素到布隆过滤器|
|BF.EXISTS|判断某个元素是否在于布隆过滤器中|
|BF.MADD|同时添加多个元素到布隆过滤器|
|BF.EXISTS|同时判断多个元素是否存在于布隆过滤器中|
|BF.REVERSE|以自定义的方式设置布隆过滤器参数值，共有3个参数分别是key,error_rate,initial_size|

#### SpringBoot使用Redis布隆过滤器

1. 使用Google Guava API, 但不支持分布式
2. 运用Redis自带布隆过滤器

* BFADD.lua

```lua
local key = KEYS[1]
local value = ARGV[1]

return redis.call("BF.ADD", key, value)
```

* BFEXISTS.lua

```lua
local key = KEYS[1]
local value = ARGV[1]

return redis.call("BF.EXISTS", key, value)
```

* RedisConfig.java

```java
    @Bean(name = "BF.ADD")
    public RedisScript<Boolean> redisBloomAdd() {
        DefaultRedisScript<Boolean> script = new DefaultRedisScript<>();
        script.setScriptSource(new ResourceScriptSource(new ClassPathResource("redis/BFADD.lua")));
        script.setResultType(Boolean.class);
        return script;
    }

    @Bean(name = "BF.EXISTS")
    public RedisScript<Boolean> redisBloomExists() {
        DefaultRedisScript<Boolean> script = new DefaultRedisScript<>();
        script.setScriptSource(new ResourceScriptSource(new ClassPathResource("redis/BFEXISTS.lua")));
        script.setResultType(Boolean.class);
        return script;
    }
```


## Application

### 缓存

* Redis作为缓存思路如下
  ![cache](source/redis/img_1.png)

### 排行榜

运用Redis的Sorted-Set能够容易的实现榜单的功能，例如音乐榜单，将score设置为音乐的播放量，每次用户播放音乐，利用`ZINCRBY`指令将score自增，即可实现，`Zset`中存音乐的歌曲名

### 计数器

计数器可以用`INCR`指令实现

> HyperLogLog 也有一些特定的使用场景，它最典型的应用场景就是统计网站用户月活量，或者网站页面的 UV(网站独立访客)数据等。
> UV 与 PV(页面浏览量) 不同，UV 需要去重，同一个用户一天之内的多次访问只能计数一次。这就要求用户的每一次访问都要带上自身的用户 ID，无论是登陆用户还是未登陆用户都需要一个唯一 ID 来标识。
> 当一个网站拥有巨大的用户访问量时，我们可以使用 Redis 的 HyperLogLog 来统计网站的 UV （网站独立访客）数据，它提供的去重计数方案，虽说不精确，但 0.81% 的误差足以满足 UV 统计的需求。

### [**Redis分布式锁**](https://juejin.cn/post/6961380552519712798)

![RedisLock](./source/redis/redislock.png)
![RedisLock](./source/redis/redislock1.png)

#### 简单由`SETNX`和`lua`脚本实现的锁

```java
@Component
@Component
public class RedisLock {
    private static final Logger logger = Logger.getLogger("com.example.rediscache.Util.RedisLock");
    @Autowired
    @Qualifier("redisScript")
    private RedisScript<Long> redisScript;
    @Autowired
    private StringRedisTemplate stringRedisTemplate;

    /**
     * 多了一些不必要的判断，降低了锁的性能
     * 用trylock，因为操作trylock和unlock都保证了Redis操作的原子性
     * 不会发生死锁，也不会多个进程同时获得锁，所以后面的判断是多余的
     */
    @Deprecated
    public boolean lock(String key, String value) {
        if (stringRedisTemplate.opsForValue().setIfAbsent(key, value))
            return true;
        String currentValue = stringRedisTemplate.opsForValue().get(key);
        if (currentValue != null && Long.parseLong(currentValue) < System.currentTimeMillis()) {
            logger.info("Holding lock out of time");
            String oldValue = stringRedisTemplate.opsForValue().getAndSet(key, value);
            if (oldValue == null || oldValue.equals(currentValue))
                return true;
        }
        return false;
    }

    public boolean trylock(String key, String value, long timeout) {
        return stringRedisTemplate.opsForValue().setIfAbsent(key, value, Duration.ofMillis(timeout));
    }

    public void unlock(String key, String value) {
        stringRedisTemplate.execute(redisScript, List.of(key), value);
    }
}
```

> `value`设为`currentTime + timeOut`

* **获得锁**
  
  1. 如果`SETNX key currentTime + timeOut`成功，说明当前没有进程持有该锁，当前线程获得锁并返回。
  2. 如果当前有进程持有锁，则获取值`GET key`
  3. 如果超时未释放锁，则说明该进程出现问题，需要是否锁，采用`GETSET`原子操作
  4. 若`oldValue`为空，则说明锁被释放，可以获得锁，若`oldValue`等于`currentValue`则说明过程中没有别的进程获得过锁，也可以获得锁
  5. 其他情况获得失败

* **释放锁**
  
  1. 释放锁应检验value是否一致，才能del，需要原子操作，要用到Redis Lua脚本执行。
     RedisScript Bean定义:
     
     ```java
     @Bean
     public RedisScript<Long> redisLua() {
        DefaultRedisScript<Long> redisScript = new DefaultRedisScript<>();
        redisScript.setScriptSource(new ResourceScriptSource(new ClassPathResource("redis/Unlock.lua")));
        redisScript.setResultType(Long.class);
        return redisScript;
     }
     ```
     
     Unlick.lua脚本:
     
     ```lua
     if redis.call("GET", KEYS[1]) == ARGV[1] then
        return redis.call("DEL", KEYS[1])
     else
        return 0
     end
     ```

#### Redis可重入锁

1. 需要存储锁名称lockName、获得该锁的线程id和对应线程的进入次数count
2. 加锁
   每次线程获取锁时，判断是否已存在该锁
   * 不存在
   * 设置hash的key为线程id，value初始化为1
   * 设置过期时间
   * 返回获取锁成功true
   * 存在
   * 继续判断是否存在当前线程id的hash key
   * 存在，线程key的value + 1，重入次数增加1，设置过期时间
   * 不存在，返回加锁失败
3. 解锁
   每次线程来解锁时，判断是否已存在该锁
   * 存在
   * 是否有该线程的id的hash key，有则减1，无则返回解锁失败
   * 减1后，判断剩余count是否为0，为0则说明不再需要这把锁，执行del命令删除

lock.lua

```lua
local key = KEYS[1]
local threadId = ARGV[1]
local timeOut = ARGV[2]

if redis.call("EXISTS", key) == 0 then
    redis.call("HSET", key, threadId, "1")
    redis.call("EXPIRE", key, timeOut)
    return 1
elseif redis.call("TTL", key) == -1 then
    redis.call("del", key)
    return 0
end

if redis.call("HEXISTS", key, threadId) == 1 then
    redis.call("HINCRBY", key, threadId, "1")
    redis.call("EXPIRE", key, timeOut)
    return 1
end

return 0
```

unlock.lua

```lua
local key = KEYS[1]
local threadId = ARGV[1]

if redis.call('HEXISTS', key, threadId) == 0 then
    return 0;
end

local count = redis.call('HINCRBY', key, threadId, -1)

if count == 0 then
    redis.call('DEL', key)
    return 0
end
```

RedisReentrantLock.java

```java
package com.example.seckilling.util;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.data.redis.core.StringRedisTemplate;
import org.springframework.data.redis.core.script.RedisScript;
import org.springframework.stereotype.Component;

import java.util.Collections;
import java.util.UUID;

@Component
public class RedisReentrantLock {
    @Autowired
    private StringRedisTemplate stringRedisTemplate;
    @Autowired
    @Qualifier("lockScript")
    private RedisScript<Long> lockScript;
    @Autowired
    @Qualifier("unlockScript")
    private RedisScript<Long> unlockScript;

    public String tryLock(String key, long timeout) {
        String field = UUID.randomUUID().toString() + Thread.currentThread().getId();
        Long res = stringRedisTemplate.execute(lockScript, Collections.singletonList(key), field, String.valueOf(timeout));
        if (res != null && res.intValue() == 1)
            return field;
        return null;
    }

    public boolean lock(String key, String field, long timeout) {
        Long res = stringRedisTemplate.execute(lockScript, Collections.singletonList(key), field, String.valueOf(timeout));
        return res != null && res.intValue() == 1;
    }

    public String lock(String key, long timeout) throws InterruptedException {
        while (true) {
            String field = tryLock(key, timeout);
            if (field != null)
                return field;
            else
                Thread.sleep(200);
        }
    }

    public void unlock(String key, String field) {
        stringRedisTemplate.execute(unlockScript, Collections.singletonList(key), field);
    }
}
```

* *RedisLock正确性测试测试*

```java
@SpringBootTest
public class RedisLockTest {
    @Autowired
    private RedisLock redisLock;
    @Autowired
    private StringRedisTemplate stringRedisTemplate;
    @Value("${redis.lock.test.timeout}")
    private long timeout;
    @Value("${redis.lock.test.nThread}")
    private int nThread;
    private static int j = 0;
    private static long currentTime = 0;
    /**
     * 在Springboot @Test中，使用多线程时，是无法深度执行解析的。原因在junit单元测试使用的是守护线程。
     * 所以循环判断是否执行结束
     */
    @Test
    public void test() {
        ExecutorService executorService = Executors.newFixedThreadPool(nThread);
        Runnable runnable = () -> {
            for (int i = 0; i < 5000; i++) {
                while (true) {
                    String value = String.valueOf(System.currentTimeMillis() + timeout);
                    if (redisLock.lock("redislocktest", value)) {
                        j++;
                        redisLock.unlock("redislocktest", value);
                        break;
                    }
                }
            }
            System.out.println("Finish, " + j + ", " + (System.currentTimeMillis() - currentTime));
            Thread.yield();
        };
        currentTime = System.currentTimeMillis();
        for (int i = 0; i < 8; i++)
            executorService.execute(runnable);
        executorService.shutdown();
        try {
            Thread.sleep(60000);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        };
    }
}
```

*测试结果共享变量j运行结果正确，此程序只用于Redis分布式锁的准确性测试，实际运用为避免SQL数据库的缓存击穿*

#### RedissionLock

![RedissionLock](./source/redis/redissionlock.png)

> Redission实现分布式锁的思路与我自己的实现是相同的，只不过在实际应用过程中需要针对实际业务进行优化。

### 最新列表

最新列表通过List的`LPUSH`和`LTRIM`实现

### **延时队列 & 消息队列**

* 消息队列用`lpush,lpop`实现，较简单，不讲述

可以使用`zset`这个命令，用设置好的时间戳作为score进行排序，使用`zadd score1 value1`命令就可以一直往内存中生产消息。再利用`zrangebysocre`查询符合条件的所有待处理的任务，通过循环执行队列任务即可。也可以通过`zrangebyscore key min max withscores limit 0 1`查询最早的一条任务，来进行消费

* 实现如下
  
  ```java
  @Component
  public class RedisDelayQueue extends Thread{
    private static Logger logger = Logger.getLogger("com.example.rediscache.Util.RedisDelayQueue");
    @Autowired
    private StringRedisTemplate stringRedisTemplate;
    @Value("${redis.queue.delay}")
    private long delay;
    @Value("${redis.queue.key}")
    private String key;
  
    public boolean push(String msg) {
        return stringRedisTemplate.opsForZSet().add(key, msg, (double) (delay + System.currentTimeMillis()));
    }
  
    public String get() {
        Set<String> msg = stringRedisTemplate.opsForZSet().rangeByScore(key, 0, (double) System.currentTimeMillis(), 0, 1);
        if (msg.size() == 0) {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            return null;
        }
        else {
            String valueToRem = (String) msg.toArray()[0];
            stringRedisTemplate.opsForZSet().remove(key, valueToRem);
            return valueToRem;
        }
    }
  
    @Override
    public void run() {
        while (true) {
            String msg = get();
            if (msg != null) {
                /* 相关业务逻辑 */
                logger.info(msg);
            }
        }
    }
  }
  ```
  
  > *Redis LUA*脚本优化版本未实现

* Redis延时队列优势
1. Redis zset支持高性能的 score 排序

2. Redis是在内存上进行操作的，速度非常快

3. Redis可以搭建集群，当消息很多时候，我们可以用集群来提高消息处理的速度，提高可用性

4. Redis具有持久化机制，当出现故障的时候，可以通过AOF和RDB方式来对数据进行恢复，保证了数据的可靠性
* Redis延时队列劣势
  
  > 没有重试机制 - 处理消息出现异常没有重试机制, 这些需要自己去实现, 包括重试次数的实现等
  > 没有 ACK 机制 - 例如在获取消息并已经删除了消息情况下, 正在处理消息的时候客户端崩溃了, 这条正在处理的这些消息就会丢失, MQ 是需要明确的返回一个值给 MQ 才会认为这个消息是被正确的消费了如果对消息可靠性要求较高, 推荐使用 MQ 来实现
  > 存在误差

### **Redis缓存穿透, 缓存击穿, 缓存雪崩**

#### 缓存穿透

缓存穿透是指当用户查询某个数据时，Redis 中不存在该数据，也就是缓存没有命中，此时查询请求就会转向持久层数据库 MySQL，结果发现 MySQL 中也不存在该数据，MySQL 只能返回一个空对象，代表此次查询失败。如果这种类请求非常多，或者用户利用这种请求进行恶意攻击，就会给 MySQL 数据库造成很大压力，甚至于崩溃，这种现象就叫缓存穿透。

* Redis布隆过滤器解决缓存雪崩

```java
String get(String key) {
    String value = redis.get(key);     
    if (value  == null) {
        if(!bloomfilter.mightContain(key)){
            return null; 
        }else{
            value = db.get(key); 
            redis.set(key, value); 
        }    
    }
    return value；
}
```

#### 缓存击穿

缓存击穿是指用户查询的数据缓存中不存在，但是后端数据库却存在，这种现象出现原因是一般是由缓存中 key 过期导致的。比如一个热点数据 key，它无时无刻都在接受大量的并发访问，如果某一时刻这个 key 突然失效了，就致使大量的并发请求进入后端数据库，导致其压力瞬间增大。这种现象被称为缓存击穿。

缓存击穿有两种解决方法：

1. 改变过期时间，设置热点数据永不过期。

2. 分布式锁。采用分布式锁的方法，重新设计缓存的使用方式，过程如下：
   上锁：当我们通过 key 去查询数据时，首先查询缓存，如果没有，就通过分布式锁进行加锁，第一个获取锁的进程进入后端数据库查询，并将查询结果缓到Redis 中。
   解锁：当其他进程发现锁被某个进程占用时，就进入等待状态，直至解锁后，其余进程再依次访问被缓存的 key。
* 分布式锁解决方案

```java
    @Value("${redis.lock.key}")
    private String lockKey;
    @Value("${redis.lock.timeout}")
    private long timeout;

    /**用分布式锁解决Redis缓存击穿问题
     * 第一个访问MySQL的进程获得锁，其他线程等待，等到第一个进程读取数据库
     * 并将数据写到缓存时，再从缓存读取
     */
    public User getUser(int id) throws Exception {
        String key = "user::" + id;
        while (true) {
            User user = (User) serializableRedisTemplate.opsForValue().get(key);
            if (user != null) {
                return user;
            } else {
                if (redisLock.lock(lockKey + id, String.valueOf(System.currentTimeMillis() + timeout))) {
                    User user1 = userMapper.getUser(id);
                    serializableRedisTemplate.opsForValue().set(key, user1, Duration.ofHours(1));
                    redisLock.unlock(lockKey + id);
                    return user1;
                }
            }
        }
    }
```

#### 缓存雪崩

缓存雪崩是指缓存中大批量的 key 同时过期，而此时数据访问量又非常大，从而导致后端数据库压力突然暴增，甚至会挂掉，这种现象被称为缓存雪崩。它和缓存击穿不同，缓存击穿是在并发量特别大时，某一个热点 key 突然过期，而缓存雪崩则是大量的 key 同时过期，因此它们根本不是一个量级。
解决方案

1. 缓存雪崩和缓存击穿有相似之处，所以也可以采用热点数据永不过期的方法，来减少大批量的 key 同时过期。
2. 再者就是为 key 设置随机过期时间，避免 key 集中过期。


### Redis限流算法

通过秒杀活动，如果我们运气爆棚，可能会用非常低的价格买到不错的商品（这种概率堪比买福利彩票中大奖）。
但有些高手，并不会像我们一样老老实实，通过秒杀页面点击秒杀按钮，抢购商品。他们可能在自己的服务器上，模拟正常用户登录系统，跳过秒杀页面，直接调用秒杀接口。
如果是我们手动操作，一般情况下，一秒钟只能点击一次秒杀按钮。但是如果是服务器，一秒钟可以请求成上千接口。 这种差距实在太明显了，如果不做任何限制，绝大部分商品可能是被机器抢到，而非正常的用户，有点不太公平。所以，我们有必要识别这些非法请求，做一些限制。

#### 通过Redis Zset数据结构实现滑动窗口算法进行限流

* 缺点: 面对高并发情况流速不均匀，常常出现99%的时间都阻塞的情况
* 优点: 支持分布式，实现简单

利用lua script实现，保证操作的原子性，flowlimit.lua
```lua
local key = KEYS[1]
local currentTime = tonumber(ARGV[1])
local sladeExpiredTime = tonumber(ARGV[2])
local max = tonumber(ARGV[3])
local expire = tonumber(ARGV[4])
local zkey = ARGV[5]

-- 利用Zset数据结构进行限流处理

-- 移除滑动区间外的值, sladeExpiredTime = currentTime - expire
redis.call("ZREMRANGEBYSCORE", key, 0, sladeExpiredTime)

-- 重新设置zset过期时间
redis.call("PEXPIRE", key, expire)

local total = redis.call("ZCARD", key) + 1


if total > max
then
    return 0
else
    --注意，score在前key在后, 第一次写debug了半个小时
    redis.call("ZADD", key, currentTime, zkey) 
    return 1
end
```

#### 利用令牌桶算法实现限流(Redis-cell)

* 优点: 支持分布式，实现简单，流速均匀，能有效应该瞬时高并发

```
CL.THROTTLE <key> <max_burst> <count per period> <period> [<quantity>]

CL.THROTTLE user123 15 30 60 1
               ▲     ▲  ▲  ▲ ▲
               |     |  |  | └───── apply 1 token (default if omitted)
               |     |  └──┴─────── 30 tokens / 60 seconds
               |     └───────────── 15 max_burst
               └─────────────────── key "user123"

127.0.0.1:6379> CL.THROTTLE user123 15 30 60
1) (integer) 0
2) (integer) 16
3) (integer) 15
4) (integer) -1
5) (integer) 2

The meaning of each array item is:

1. Whether the action was limited:
    0 indicates the action is allowed.
    1 indicates that the action was limited/blocked.
2. The total limit of the key (max_burst + 1). This is equivalent to the common X-RateLimit-Limit HTTP header.
3. The remaining limit of the key. Equivalent to X-RateLimit-Remaining.
4. The number of seconds until the user should retry, and always -1 if the action was allowed. Equivalent to Retry-After.
5. The number of seconds until the limit will reset to its maximum capacity. Equivalent to X-RateLimit-Reset.
```

CL-THROTTLE.lua
```lua
local key = KEYS[1]
local max_burst = tonumber(ARGV[1])
local countPerTime = tonumber(ARGV[2])
local period = tonumber(ARGV[3])
local quantity = tonumber(ARGV[4])

-- 利用Redis-cell进行限流处理
return redis.call("CL.THROTTLE", key, max_burst, countPerTime, period, quantity)
```

* RedisConfig.java

```java
    @Bean(name = "flowLimited")
    public RedisScript<Boolean> redisFlowLimited() {
        DefaultRedisScript<Boolean> script = new DefaultRedisScript<>();
        script.setScriptSource(new ResourceScriptSource(new ClassPathResource("redis/flowlimit.lua")));
        script.setResultType(Boolean.class);
        return script;
    }

    @Bean(name = "CL.THROTTLE")
    public RedisScript<List> redisCell() {
        DefaultRedisScript<List> script = new DefaultRedisScript<>();
        script.setScriptSource(new ResourceScriptSource(new ClassPathResource("redis/CL-THROTTLE.lua")));
        script.setResultType(List.class);
        return script;
    }
```

* 测试程序

```java
@SpringBootTest
public class RedisServiceTest {
    @Autowired
    private RedisService redisService;
    @Autowired
    private StringRedisTemplate stringRedisTemplate;

    @Test
    public void test() {
        stringRedisTemplate.delete("bloomtest");
        for (int i = 0; i < 10000; i++) {
            /*if (redisService.flowLimited("flowTest", 100, 2000)) {
                redisService.bfadd("bloomtest", String.valueOf(i));
            }*/
            List<Long> list = redisService.clChrottle("clthrottletest", 200, 100, 2, 1);
            if (list.get(0).intValue() == 0)
                redisService.bfadd("bloomtest", String.valueOf(i));
        }

        double count = 0;
        for (int i = 0; i < 10000; i++) {
            if (redisService.bfexists("bloomtest", String.valueOf(i)))
                count += 1;
        }

        System.out.println("Total = " + (int) count);
        System.out.println("hit rate = " + count / 10000.0);
    }
}
```



## SpringBoot集成Redis

### 配置文件`application.properties`

```properties
# 要连接的数据库，默认为0
spring.redis.database=0
# 主机名
spring.redis.host=localhost
# 端口号
spring.redis.port=6379
# 设置超时时间
spring.redis.timeout=3000
spring.redis.jedis.pool.max-active=8
spring.redis.jedis.pool.max-wait=-1
spring.redis.jedis.pool.max-idle=8
spring.redis.jedis.pool.min-idle=0
```

### Redis配置类

* 默认有一个`StringRedisTemplate`, 但项目中我们往往需要存储实体类，所以需要自定义一个`Template`

```java
@Configuration
public class RedisConfig {
    @Autowired
    private RedisConnectionFactory redisConnectionFactory;

    /**一下为需要用注解实现缓存时需要
     * 一般不用，因为应用面窄，自定义速度快，自由度高
     */
    @Bean
    public RedisCacheManager redisCacheManager() {
        RedisCacheConfiguration config = RedisCacheConfiguration
                .defaultCacheConfig()
                .entryTtl(Duration.ofMinutes(30))
                .serializeKeysWith(RedisSerializationContext.SerializationPair.fromSerializer(new StringRedisSerializer()))
                .serializeValuesWith(RedisSerializationContext.SerializationPair.fromSerializer(new GenericJackson2JsonRedisSerializer()));
        RedisCacheManager build = RedisCacheManager
                .builder(redisConnectionFactory)
                .cacheDefaults(config)
                .build();
        return build;
    }

    @Bean
    public RedisTemplate<String, Serializable> serializableRedisTemplate() {
        RedisTemplate<String, Serializable> serializableRedisTemplate = new RedisTemplate<>();
        serializableRedisTemplate.setConnectionFactory(redisConnectionFactory);
        serializableRedisTemplate.setKeySerializer(new StringRedisSerializer());
        serializableRedisTemplate.setHashKeySerializer(new StringRedisSerializer());
        serializableRedisTemplate.setValueSerializer(new GenericJackson2JsonRedisSerializer());
        return serializableRedisTemplate;
    }
}
```

## Redis持久化策略

### RDB持久化

* RDB就是快照模式，它是Redis默认的数据持久化方式，它会将数据库的快照保存在*dump.rdb*这个二进制文件中。
* Redis 使用操作系统的多进程*Copy On Write*机制来实现快照持久化操作。
* RDB 实际上是 Redis 内部的一个定时器事件，它每隔一段固定时间就去检查当前数据发生改变的次数和改变的时间频率，看它们是否满足配置文件中规定的持久化触发条件。当满足条件时，Redis 就会通过操作系统调用 fork() 来创建一个子进程，该子进程与父进程享有相同的地址空间。
* Redis 通过子进程遍历整个内存空间来获取存储的数据，从而完成数据持久化操作。注意，此时的主进程则仍然可以对外提供服务，父子进程之间通过操作系统的 COW 机制实现了数据段分离，从而保证了父子进程之间互不影响。
* 也可以手动备份，使用命令`SAVE`, 或`BGSAVE`,`LASTSAVE`查看备份是否完成，重启服务数据迁移完成，使用`config get dir`查看备份数据位置

### AOF持久化

启用AOF配置

```shell
vim /etc/redis/redis.conf
appendonly yes //改为yes
sudo /etc/init.d/redis-server restart
```

* 写入机制: Redis 在收到客户端修改命令后，先进行相应的校验，如果没问题，就立即将该命令存追加到 .aof文件中，也就是先存到磁盘中，然后服务器再执行命令。这样就算遇到了突发的宕机情况情况，也只需将存储到  .aof 文件中的命令，进行一次“命令重演”就可以恢复到宕机前的状态。在上述执行过程中，有一个很重要的环节就是命令的写入，这是一个 IO 操作。Redis 为了提升写入效率，它不会将内容直接写入到磁盘中，而是将其放到一个内存缓存区（buffer）中，等到缓存区被填满时才真正将缓存区中的内容写入到磁盘里。
* 重写机制:为了让aof文件的大小控制在合理的范围内，Redis提供了AOF重写机制，手动执行`BGREWRITEAOF`命令，开始重写aof文件, 缩减文件大小。

## [Redis主从,哨兵,Cluster](./redis-cluster.md)

### Redis主从架构

---

单机的Redis，能够承载的QPS大概就在上万到几万不等。对于缓存来说，一般都是用来支撑读高并发的。因此架构做成主从(master-slave)架构，一主多从，主负责写，并且将数据复制到其它的slave节点，从节点负责读。所有的读请求全部走从节点。这样也可以很轻松实现水平扩容，支撑读高并发。

![master-slaver](./source/redis/redis-master-slave.png)

Redis replication -> 主从架构 -> 读写分离 -> 水平扩容支撑读高并发

#### Redis replication 的核心机制

---

* Redis采用**异步方式**复制数据到slave节点，不过从Redis2.8开始，slave node会周期性地确认自己每次复制的数据量
* 一个master node可以配置多个slave node
* slave node也可以链接其他的slave node
* slave进行复制时，不会阻塞master node的正常工作
* slave进行复制时，也不会阻塞自身的正常运行，他会继续用旧数据进行服务。不过复制完成后需要删除旧数据加载新数据，这个时候就要阻塞请求
* slave node主要用来进行横向扩容，做读写分离，扩容slave node可以提高读高并发的吞吐量。

注意，如果采用了主从架构，那么建议必须开启master node的持久化，不建议用slave node作为master node的数据热备，因为那样的话，如果你关闭master的持久化，可能在master宕机重启的时候数据是空的，然后可能一经过复制， slave node的数据也丢了。

另外，master的各种备份方案，也需要做。万一本地的所有文件丢失了，从备份中挑选一份rdb去恢复master，这样才能确保启动的时候，是有数据的，即使采用了后续讲解的高可用机制，slave node可以自动接管master node，但也可以sentinel还没检测到master failure，master node就自动重启了，还是可能导致上面所有的 slave node数据被清空。

#### Redis主从复制的核心原理

当启动一个slave node的时候，它会发送一个 `PSYNC` 命令给master node。

如果这是slave node初次连接到master node，那么会触发一次`full resynchronization`全量复制。此时master会启动一个后台线程，开始生成一份`RDB`快照文件，同时还会将从客户端client新收到的所有写命令缓存在内存中。 `RDB` 文件生成完毕后， master 会将这个 `RDB` 发送给 slave，slave 会先写入本地磁盘，然后再从本地磁盘加载到内存中，接着 master 会将内存中缓存的写命令发送到slave，slave也会同步这些数据。slave node如果跟master node有网络故障，断开了连接，会自动重连，连接之后master node仅会复制给slave部分缺少的数据。

![redis-master-sync](source/redis/redis-master-slave-replication.png)

#### 主从复制断点续传

master node会在内存中维护一个backlog，master和slave都会保存一个replica offset还有一个master run id，offset就是保存在backlog中的。如果master 和slave网络连接断掉了，slave会让master从上次replica offset开始继续复制，如果没有找到对应的offset，那么就会执行一次`resynchronization`

#### 过期key处理

slave不会过期key，只会等待master过期key。如果master过期了一个key，或者通过LRU淘汰了一个key，那么会模拟一条del命令发送给slave。

#### 全量复制

* master执行bgsave，在本地生成一份rdb快照文件
* master node将rdb快照文件发送给slave node，如果rdb复制时间超过60秒(repl-timeout)，那么slave node就会认为复制失败，可以适当调大这个参数(对于千兆网卡的机器，一般每秒传输100MB，6G文件，很可能超过60s)
* master node在生成rdb时，会将所有新的写命令缓存在内存中，在slave node保存了rdb之后，再将新的写命令复制给 slave node。
* 如果在复制期间，内存缓冲区持续消耗超过64MB，或者一次性超过256MB，那么停止复制，复制失败。
* slave node接收到rdb之后，清空自己的旧数据，然后重新加载rdb到自己的内存中，同时基于旧的数据版本对外提供服务。
* 如果slave node开启AOF，那么会立即执行BGREWRITEAOF，重写AOF。

#### 增量复制

* 如果全量复制过程中，master-slave网络连接断掉，那么slave重新连接master时，会触发增量复制
* master直接从自己的backlog中获取部分丢失的数据，发送给slave node，默认backlog就是1MB
* master就是根据slave发送的psync中的offset来从backlog中获取数据的



### Redis哨兵


### Redis Cluster

#### Redis Cluster简介

Redis Cluster即Redis集群，是Redis官方在3.0版本推出的一套分布式存储方案。完全去中心化，由多个节点组成，所有节点彼此互联。Redis客户端可以直接连接任何一节点获取集群中的键值对，不需要中间代理，如果该节点不存在用户所指定的键值，其内部会自动把客户端重定向到键值所在的节点。

Redis集群是一个网状结构，每个节点都通过TCP连接跟其他每个节点连接。在一个有N个节点的集群中，每个节点都有N-1个流出的TCP连接，和N-1个流入的连接，这些TCP连接会永久保持。

Redis Cluster 同其他分布式存储系统一样，主要具备以下两个功能：

**数据分区**Redis集群会将用户数据分散保存至各个节点中，突破单机Redis内存最大存储容量。集群引入了哈希槽slot的概念，其搭建完成后会生16384个哈希槽slot，同时会根据节点的数量大致均等的将16384个哈希槽映射到不同的节点上。当用户存储key-value时，集群会先对key进行CRC16校验然后对16384取模来决定key-value放置哪个槽，从而实现自动分割数据到不同的节点上。

**数据冗余**Redis集群支持主从复制和故障恢复。集群使用了主从复制模型，每个主节点master应至少有一个从节点slave。假设某个主节点故障，其所有子节点会广播一个数据包给集群里的其他主节点来请求选票，一旦某个从节点收到了大多数主节点的回应，那么它就赢得了选举，被推选为主节点，负责处理之前旧的主节点负责的哈希槽。

#### Redis集群搭建

1. 编写配置文件`redis.conf`，搭建6个节点的集群，3个`master`，3个`slave`，端口分别为`6377,6378,6379,7000,7001,7002`，并创建6个文件夹名字分别为`6377,6378,6379,7000,7001,7002`，每个目录下包含一个redis.conf文件

*最小集群配置*

```conf
port 6379
cluster-enabled yes
cluster-config-file nodes.conf
cluster-node-timeout 5000
appendonly yes
```

2. 进入每个目录下运行`redis-server ./redis.conf`，启动6给redis服务器**(注意: 一定要在目录下启动服务器)**
3. 创建集群

*创建6个节点的集群，3个master，3个slave，--cluster-replicas 1 代表每个master分配1给slave*

```shell
redis-cli --cluster create 127.0.0.1:6377 127.0.0.1:6378 127.0.0.1:6379 127.0.0.1:7000 127.0.0.1:7001 127.0.0.1:7002 --cluster-replicas 1
```

4. 查看集群`redis-cli -p 6379 cluster nodes`


#### Redis Cluster相关命令

```shell
redis-cli --cluster help
Cluster Manager Commands:
  create         host1:port1 ... hostN:portN   #创建集群
                 --cluster-replicas <arg>      #从节点个数
  check          host:port                     #检查集群
                 --cluster-search-multiple-owners #检查是否有槽同时被分配给了多个节点
  info           host:port                     #查看集群状态
  fix            host:port                     #修复集群
                 --cluster-search-multiple-owners #修复槽的重复分配问题
  reshard        host:port                     #指定集群的任意一节点进行迁移slot，重新分slots
                 --cluster-from <arg>          #需要从哪些源节点上迁移slot，可从多个源节点完成迁移，以逗号隔开，传递的是节点的node id，还可以直接传递--from all，这样源节点就是集群的所有节点，不传递该参数的话，则会在迁移过程中提示用户输入
                 --cluster-to <arg>            #slot需要迁移的目的节点的node id，目的节点只能填写一个，不传递该参数的话，则会在迁移过程中提示用户输入
                 --cluster-slots <arg>         #需要迁移的slot数量，不传递该参数的话，则会在迁移过程中提示用户输入。
                 --cluster-yes                 #指定迁移时的确认输入
                 --cluster-timeout <arg>       #设置migrate命令的超时时间
                 --cluster-pipeline <arg>      #定义cluster getkeysinslot命令一次取出的key数量，不传的话使用默认值为10
                 --cluster-replace             #是否直接replace到目标节点
  rebalance      host:port                                      #指定集群的任意一节点进行平衡集群节点slot数量 
                 --cluster-weight <node1=w1...nodeN=wN>         #指定集群节点的权重
                 --cluster-use-empty-masters                    #设置可以让没有分配slot的主节点参与，默认不允许
                 --cluster-timeout <arg>                        #设置migrate命令的超时时间
                 --cluster-simulate                             #模拟rebalance操作，不会真正执行迁移操作
                 --cluster-pipeline <arg>                       #定义cluster getkeysinslot命令一次取出的key数量，默认值为10
                 --cluster-threshold <arg>                      #迁移的slot阈值超过threshold，执行rebalance操作
                 --cluster-replace                              #是否直接replace到目标节点
  add-node       new_host:new_port existing_host:existing_port  #添加节点，把新节点加入到指定的集群，默认添加主节点
                 --cluster-slave                                #新节点作为从节点，默认随机一个主节点
                 --cluster-master-id <arg>                      #给新节点指定主节点
  del-node       host:port node_id                              #删除给定的一个节点，成功后关闭该节点服务
  call           host:port command arg arg .. arg               #在集群的所有节点执行相关命令
  set-timeout    host:port milliseconds                         #设置cluster-node-timeout
  import         host:port                                      #将外部redis数据导入集群
                 --cluster-from <arg>                           #将指定实例的数据导入到集群
                 --cluster-copy                                 #migrate时指定copy
                 --cluster-replace                              #migrate时指定replace
  help           

For check, fix, reshard, del-node, set-timeout you can specify the host and port of any working node in the cluster.
```

**删除节点并删除数据需要先执行`flushall`清空数据, 在执行`redis-cli --cluster reshard`将哈希槽分配给其他节点, 再执行`redis-cli --cluster del-node`删除节点，在进入该节点目录下删除`nodes.conf`和`appendonly.conf`文件，这样下次启动就不会重新连接现有集群了**


### SpringBoot Redis集群配置

* pom.xml依赖

```xml
<dependency>
	<groupId>org.springframework.boot</groupId>
	<artifactId>spring-boot-starter-data-redis</artifactId>
</dependency>
```

* application.properties

*注意，nodes写全部节点，master & slave都写*

```properties
spring.redis.timeout=3000ms
spring.redis.lettuce.pool.max-active=16
spring.redis.lettuce.pool.max-wait=-1
spring.redis.lettuce.pool.max-idle=16
spring.redis.lettuce.pool.min-idle=8
spring.redis.cluster.nodes=127.0.0.1:6379,127.0.0.1:6378,127.0.0.1:6377,127.0.0.1:7001,127.0.0.1:7003
spring.redis.cluster.max-redirects=3
```

* 配置类

*基本不变，只需要把RedisConnectionFactory改为LettuceConnectionFactory就行*

```java
@Configuration
@EnableCaching
public class RedisConfig {
    @Autowired
    private LettuceConnectionFactory lettuceConnectionFactory;

    @Bean
    public RedisCacheManager redisCacheManager() {
        RedisCacheConfiguration config = RedisCacheConfiguration
                .defaultCacheConfig()
                .entryTtl(Duration.ofMinutes(30))
                .serializeKeysWith(RedisSerializationContext.SerializationPair.fromSerializer(new StringRedisSerializer()))
                .serializeValuesWith(RedisSerializationContext.SerializationPair.fromSerializer(new GenericJackson2JsonRedisSerializer()));
        RedisCacheManager build = RedisCacheManager
                .builder(lettuceConnectionFactory)
                .cacheDefaults(config)
                .build();
        return build;
    }

    @Bean
    public RedisTemplate<String, Serializable> serializableRedisTemplate() {
        RedisTemplate<String, Serializable> serializableRedisTemplate = new RedisTemplate<>();
        serializableRedisTemplate.setConnectionFactory(lettuceConnectionFactory);
        serializableRedisTemplate.setKeySerializer(new StringRedisSerializer());
        serializableRedisTemplate.setHashKeySerializer(new StringRedisSerializer());
        serializableRedisTemplate.setHashValueSerializer(new GenericJackson2JsonRedisSerializer());
        serializableRedisTemplate.setValueSerializer(new GenericJackson2JsonRedisSerializer());
        return serializableRedisTemplate;
    }
}
```

* 我的相关shell脚本

`cluster-info.sh`
```shell
#!/bin/bash

ports=(6377 6378 6379 7000 7001 7002 7003)

for i in {0..6}
do
        port=${ports[${i}]}
        str1=`redis-cli -p ${port} cluster nodes | grep ${port}`
        str2=`redis-cli --cluster info 127.0.0.1:${port} | grep ${port}`
        master_id=${str1%% *}
        pre=${str2%%(*}
        post=${str2##*)}
        slave_id_=${str1##*slave }
        slave_id=${slave_id_%% *}
        a=`expr length "${pre}"`
        b=`expr length "${post}"`


        if [ ${a} -ne 0 -a ${b} -ne 0 ]
        then
                echo ${pre}"("${master_id}")"${post}
        else
                str3=`redis-cli -p ${port} cluster nodes | grep ${slave_id}`
                aa=${str3##*:}
                bb=${aa%%@*}
                echo "127.0.0.1:"${port}" ("${master_id}") slave of ("${slave_id}")"
        fi

done
```

`ls-redis.sh`
```shell
#!/bin/bash

echo "UID        PID  PPID  C STIME TTY          TIME CMD"
ps -ef | grep redis
```

* 集群创建完毕效果图如下

![cluster](./source/redis/cluster.png)


## Redis配置项汇总

#### 基本配置

```conf
port 6379  # 监听端口号，默认为6379，如果你设为 0 ，redis 将不在 socket 上监听任何客户端连接。

#daemonize:yes:redis采用的是单进程多线程的模式。当redis.conf中选项daemonize设置成yes时，
#代表开启守护进程模式。在该模式下，redis会在后台运行，
#并将进程pid号写入至redis.conf选项pidfile设置的文件中，此时redis将一直运行，除非手动kill该进程。

daemonize no #指定redis是否以守护线程的方式启动
databases 16 #创建database的数量，默认为0库

save 900 1 #刷新快照到硬盘中。必须满足下列三个要求之一才会触发，即900秒内至少有1个key发生变化。
save 300 10 #在300秒内至少10个key发生变化。
save 60 10000 #在60秒之内至少有10000个可以发生变化。

stop-writes-on-bgsave-error yes #后台存储错误并停止写入命令。
rdbcompression yes #使用LZF方式压缩rdb文件。如果你想节省一些CPU可设置成'no'
rdbchecksum yes #在存储、加载rdb文件时进行校验。
dbfilename dump.rdb #设置rdb文件名。
dir ./ #设置工作目录，rdb文件会自动存放在该目录。
```

#### 主从服务配置

```conf
slaveof <masterip> <masterport> #将本机设为某台机器的从服务器
masterauth <master-password> #连接主服务器的密码
slave-serve-stale-data yes # 当主机和从机断开时或这正处于在复制过程中，是否让从服务器是应答请求
slave-read-only yes #设置从服务器为只读模式
repl-diskless-sync no  #是否同时向多个从服务器节点同时发数据
repl-diskless-sync-delay 5 #发送数据的延迟时间
repl-ping-slave-period 10 #主节点默认每隔 10 秒对从节点发送 ping 命令
repl-timeout 60 #主从服务器超时时间(超时认为断线了),要比period设置的时间大

#如果master不能再正常工作，那么会在多个slave中，选择优先值最小的一个slave提升为master，
#优先值为0表示不能提升为master，一般在哨兵sentinel环境中使用。
slave-priority 100 

#在slave和master同步后，后续的同步是否设置成TCP_NODELAY，设置成no，则redis master会立即发送同步数据，没有延迟
repl-disable-tcp-nodelay no 
min-slaves-to-write 3 #主节点仅允许当能够通信的从节点数量大于等于此处的值时，才允许接受写操作；
min-slaves-max-lag 10 #从节点延迟时长超出此处指定的时间时，主节点会拒绝写入操作；
```

#### 安全配置

```conf
requirepass foobared # 用来配置密码
rename-command CONFIG b84 #在公共环境下重命名部分敏感命令 如config、flushall等
```

#### 限制配置

```conf
maxclients 10000 #最大连接数
maxmemory <bytes> #最大使用内存
maxmemory-policy volatile-lru #内存到极限后的处理策略

#内存处理策略，用于在超出内存限制时，删除一些key
volatile-lru # LRU算法删除过期key
allkeys-lru # LRU算法删除key(不区分过不过期)
volatile-random # 随机删除过期key
allkeys-random # 随机删除key(不区分过不过期)
volatile-ttl # 删除快过期的key
noeviction # 禁止删除key,这如果内存不足，会直接返回错误。默认配置

#用于提高LRU/TTL算法的精准度，在自动清理内存时，指定的数字越大，CPU消耗就越多，默认为5。
maxmemory-samples 5
```

#### AOF日志模式

```conf
appendonly no #是否启用日志模式
appendfsync no # 有系统决定何时写,统一写,速度快
appendfsync always # 系统不缓冲,一直写,但是慢,这种方式几乎不丢失数据
appendfsync everysec #每秒写1次

no-appendfsync-on-rewrite no #相当于将appendfsync设置为no，不存在磁盘操作，只是将数据写入了缓冲区，写入速度非常快
auto-AOF-rewrite-percentage 100 #触发aof重写操作，要求本次文件大小比上次重写时要增加1（100%）倍
auto-AOF-rewrite-min-size 64mb #触发aof重写操作，至少要达到的aof文件大小
```

#### 慢查询配置

Redis slowlog 是一个记录 Redis 执行查询命令时所耗费时间的日志系统，它仅记录执行一个查询命令所耗费的时间，不记录其他内容
```conf
slowlog-log-slower-than 10000 #记录响应时间大于10000微秒的慢查询
slowlog-max-len 128 # 最多记录128条
```

#### 服务端命令

```conf
time #返回时间戳+微秒
dbsize #返回key的数量
bgrewriteaof #重写aof
bgsave #后台开启子进程来执行数据持久化
save #以阻塞的方式对数据进行持久化
lastsave #返回最近一次 Redis 成功将数据保存到磁盘上的时间，以 UNIX 时间戳格式表示。

slaveof host port #设置为host:port的从服务器(数据清空,复制新的主服务器内容)
slaveof no one   #变成主服务器(原数据不丢失,一般用于主服失败后)

flushdb 清空当前数据库的所有数据
flushall 清空所有数据库的所有数据

shutdown [save/nosave] 关闭服务器,保存数据,修改AOF

slowlog get 获取慢查询日志
slowlog len 获取慢查询日志条数
slowlog reset 清空慢查询
```

#### 客户端命令

```conf
#以易于理解和阅读的方式返回Redis服务器的各种信息、统计数值
info [server|clients|memory|stats|]
config get [配置项]    #获取配置文件选项
config set [配置项] [参数值] #重新设置配置文件选项和对应参数
config rewrite  #对启动Redis服务器时所指定的配置文件进行改写
config resetstat #重置info命令中的某些统计信息

debug object key #调试选项,看一个key的情况
debug segfault #该命令能够让服务器崩溃
object key (refcount|encoding|idletime)
monitor #调试用，打开控制台,观察命令
client list #列出所有连接
client kill #杀死某个连接 CLIENT KILL 127.0.0.1:6379
client getname #获取连接的名称 默认nil
client setname  #设置连接名称,便于调试
```

#### 连接命令

```conf
auth 密码  #验证登录密码(如果设置了密码)
ping      #测试服务器是否可用
echo "hello www.biancheng.net" #测试服务器是否正常交互
select 0/1/2/3/4...  #选择数据库0-15
quit  #退出连接
```

## [Redis源码解读](./source/redis/Redis%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)