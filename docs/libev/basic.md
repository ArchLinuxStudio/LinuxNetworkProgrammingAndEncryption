# libev

在学习完网络编程一章中的五种网络 I/O 模型以及使用方式后，是时候来学习 libev 这个网络库了，它被 shadowsocks-libev 等众多软件使用。如果你已经掌握了网络编程一章的知识，那么学习使用 libev 并不会是一件很难的事情。

还是以经典的回声服务端为例，libev 的实现方式其实是和 select 等多路复用模型类似的。大致也分为以下几步

- 创建 socket，绑定 socket 地址
- 监听服务端 socket
- 创建一个 watcher，用来承载服务端 socket 的 accept 事件
- 写一个 callback 用来做实际的 accept 调用
- 创建并初始化一个 watcher 用来从 client socket 中读取请求
- 写一个 callback 用来 read
- 启动 event loop

[echo_server](../src/libev/echo_server.c ':include')

---

Ref:

1. [[1]](https://luohaha.github.io/Chinese-uvbook/source/basics_of_libuv.html)
2. [[2]](https://blog.csdn.net/drdairen/article/details/53694550)
3. [[3]](https://blog.csdn.net/drdairen/article/details/53746944)
4. [[4]](https://segmentfault.com/a/1190000003063859)
5. [[5]](https://www.itzhai.com/categories/%E7%BD%91%E7%BB%9C/)
6. [[6]](https://zhuanlan.zhihu.com/p/62682475)
7. [[7]](https://www.cnblogs.com/lsgxeva/p/11777459.html)
8. [[8]](https://my.oschina.net/u/917596/blog/176658)
9. [使用 libev 构建 TCP 响应服务器（echo server）的简单流程](https://segmentfault.com/a/1190000006691243)
