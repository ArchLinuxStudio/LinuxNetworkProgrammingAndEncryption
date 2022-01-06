# 异步 I/O 模型 io_uring

此处所说的异步 I/O 模型指的是 UNIX 规范中所描述的通用异步 I/O 机制。区别于信号驱动式的 I/O 模型，此种模型是真正的异步 I/O 实现。这种模型与信号驱动模型的主要区别在于：信号驱动 I/O 是由内核通知应用程序何时启动一个 I/O 操作，而异步 I/O 模型是由内核通知应用程序 I/O 操作何时完成。目前 Linux 中有三种异步 I/O 模式，分别如下

- [glibc](https://archlinux.org/packages/core/x86_64/glibc/)在用户空间供了以 aio\_为前缀的一系列函数，通过 pthread 多线程的方式以模拟的形式来实现异步 I/O 模型，从而不依赖于内核。对应头文件为 aio.h。
- [libaio](https://archlinux.org/packages/core/x86_64/libaio/)函数库提供了对于内核异步 I/O 的调用方式，其为一个 POSIX 规范在 Linux 下的实现。对应头文件为 libaio.h。
- kernel 5.X 时代的异步模型 io_uring，原生 io_uring 接口相对应的库为[liburing](https://archlinux.org/packages/extra/x86_64/liburing/)。

然而前两种模式均不支持在套接字方面的应用，并且并没有被广泛应用和采纳，而且它们已经逐渐被新的 io_uring 模式取代，所以本文不再描述这两种模式。

io_uring 是 linux 内核 5.X 时代加入的全新异步 I/O 模型，大概在 5.4 版本正式可用，在 5.7 和 5.12 版本逐渐完善，io_uring 普遍被认为是 linux 下对标 windows 的 IOCP 的、真正的异步 I/O 模型将来的趋势。异步 I/O 不仅在等待数据阶段是非阻塞的，同时在读取数据阶段也是非阻塞的，这是其他模型没办法做到的，接收到通知后，可以直接在用户空间取得数据，而不是还需要进行从内核空间到用户空间的拷贝。

到目前为止，我们介绍了阻塞 IO，非阻塞 IO，信号驱动式 IO，IO 复用，我们打个形象的比方，来对这几种 IO 做下区分。我们去网上买东西，下完单之后，你可以有如下几种处理方式：

- 下完单之后，在门口一直等待快递小哥把快递送上门，这就是同步阻塞 IO；
- 下完单之后不停的，或者隔几秒种就下楼看看快递到没到，这就是同步非阻塞 IO；
- 下完单之后就不管了，直到快递小哥打电话给你通知你去取快递，这就是同步非阻塞 IO 里面的信号驱动式 IO；
- 下完单之后，你定时的去物流 app 上面查看你所有快递的状态，只要有快递送到了寄存点，你就去取，这就是同步非阻塞 IO 里面的 IO 复用；
- 下完单之后，你就不管了，直到快递小哥给你送上门，你直接拿到了快递，你不用出门就可以拿到快递了，这就是异步 IO。

---

如下是一个使用 io_uring 的简单回声服务端。使用`gcc io_uring_echo.c -luring`进行编译。可以看到，其流程部分和 I/O 复用中的回声服务器流程类似，但是也有不同。

不同主要体现在得到 cqe 后的处理方式。由于 io_uring 是真正的异步 I/O 模型，比如在得到 ACCEPT 类型的 cqe 后，accept 动作实际已经进行完毕，可以直接对返回的 conn_fd 进行添加准备读的操作。并且在接收到 ACCEPT 类型后，还需要重新添加对 ACCEPT 类型的准备。比如在接收到 READ 类型的 cqe 后，由于 READ 操作已经完成，继续需要进行的即为写操作。接收到 WRITE 类型的 cqe 后同理。

[io_uring_echo](../src/network/io_uring/io_uring_echo.c ':include')

---

Ref:

https://github.com/shuveb/io_uring-by-example  
https://github.com/frevib/io_uring-echo-server  
http://www.mathcs.emory.edu/~cheung/Courses/455/Syllabus/9-netw-prog/timeout6.html)
