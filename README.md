# AE_EPOLL

IO多路复用程序, 基于`linux epoll`实现，IO多路复用能显著提高程序响应速度已经高并发能力，使用单线程加IO多路复用模型能够免去以往多线程协调所带来的额外开销，并获得更高的并发量而不必去考虑多线程协调的问题，`Nginx`, `Redis`, `Netty`等能够如此快速也是基于此。此项目在学习了`Redis`之后完成，作为`Redis`学习的收尾项目。

**IO多路复用设计思路**

![IO多路复用机制](./ph/IO.png)


附:

[Redis学习笔记](./redis.md)

[Redis源码(基础)](./Redis%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.md)

