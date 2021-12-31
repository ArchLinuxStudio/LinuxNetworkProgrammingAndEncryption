# Socket 编程简要说明

Socket 编程是完成一个能够突破互联网封锁的软件的基础内容。

如今互联网上有相当多的 C 语言 Socket 编程相关内容，也有很多书籍讲解 Socket 编程的内容。本章对于很多你可以轻易学到的内容不会赘述，仅描述需要注意的点和较为进阶的点。同时，对于 Socket 编程的各种 I/O 模型(包括 iouring)，以及各种逻辑处理模型，本章会分别给出可以运行的例子，这是本书的特点，实际的可运行例子可以加深你对 Socket 编程的理解。

如下列出我们认为较好的 Socket 编程书籍以及网络文章。

- TCP/IP 网络编程 -- 尹圣雨
- Linux 高性能服务器编程 -- 游双
- [网络编程必备知识：图解 Socket 核心内幕以及五大 IO 模型 | 阻塞 IO,非阻塞 IO,IO 复用,信号驱动式 IO,异步 IO](https://www.itzhai.com/articles/necessary-knowledge-of-network-programming-graphic-socket-core-insider-and-five-io-models.html)
- [似乎没那么完美的信号驱动式 IO](https://www.itzhai.com/articles/it-seems-not-so-perfect-signal-driven-io.html)
- [彻底弄懂 IO 复用：IO 处理杀手锏，带您深入了解 select，poll，epoll](https://www.itzhai.com/articles/thoroughly-understand-io-reuse-take-you-in-depth-understanding-of-select-poll-epoll.html)
- Linux/Unix 系统编程手册
- UNIX 网络编程
- UNIX 环境高级编程
- Linux 设备驱动开发详解
- 深入理解计算机系统
