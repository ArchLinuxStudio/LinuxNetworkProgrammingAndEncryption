# 异步 I/O 模型

此处所说的异步 I/O 模型指的是 UNIX 规范中所描述的通用异步 I/O 机制。区别于信号驱动式的 I/O 模型，此种模型是真正的异步 I/O 实现。这种模型与信号驱动模型的主要区别在于：信号驱动 I/O 是由内核通知应用程序何时启动一个 I/O 操作，而异步 I/O 模型是由内核通知应用程序 I/O 操作何时完成。目前 Linux 中有三种异步 I/O 模式，分别如下

- [glibc](https://archlinux.org/packages/core/x86_64/glibc/)在用户空间供了以 aio\_为前缀的一系列函数，通过 pthread 多线程的方式以模拟的形式来实现异步 I/O 模型，从而不依赖于内核。
- [libaio](https://archlinux.org/packages/core/x86_64/libaio/)函数库提供了对于内核异步 I/O 的调用方式，其为一个 POSIX 规范在 Linux 下的实现。
- kernel 5.X 时代的异步模型 io_uring，相应的库为[liburing](https://archlinux.org/packages/extra/x86_64/liburing/)。

然而前两种模式均不支持在套接字方面的应用，并且并没有被广泛应用和采纳，而且它们已经逐渐被新的 io_uring 模式取代，所以本文不再描述这两种模式。

io_uring 是 linux 内核 5.X 时代加入的全新异步 I/O 模型，大概在 5.4 版本正式可用，在 5.7 和 5.12 版本逐渐完善，io_uring 普遍被认为是 linux 下对标 windows 的 IOCP 的、真正的异步 I/O 模型将来的趋势。

---

Ref:
[[2]](http://www.mathcs.emory.edu/~cheung/Courses/455/Syllabus/9-netw-prog/timeout6.html)
