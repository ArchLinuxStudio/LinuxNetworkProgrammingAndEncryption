# Socket 阻塞式 I/O 模型

Unix 网络编程中，存在五种 I/O 模型，它们分别为：

- 阻塞式 I/O 模型
- 非阻塞式 I/O 模型
- I/O 复用模型
- 信号驱动式 I/O 模型
- 异步 I/O 模型

本节首先来描述的是阻塞式 I/O 模型，它是一切的基础。

## socket 的简单例子

先看服务端代码。

[server](../src/network/socket/server.c ':include')

首先，通过 socket 函数创建服务端套接字。第一个参数代表 Address family 为 IPV4，通过点击跳转可以看到 AF_INET 与 PF_INET 是同义的。第二个参数代表当前的 SOCK 类型是流式传输，另外一种与其对应的类型为 SOCK_DGRAM，意为基于 datagram 的类型，99%的情况下，你可以分别把这两种类型理解为 TCP 与 UDP。第三个参数是由前两个参数决定而来。之所以前面说 99%的情况，是因为在 IPV4/IPV6 的协议家族下，SOCK_STREAM 和 SOCK_DGRAM 两种类型相对应的协议就是 TCP 与 UDP。在其他相对不常用的协议家族下，SOCK_STREAM 和 SOCK_DGRAM 两种类型分别代表不同的协议，更多内容可参见[[1]](https://stackoverflow.com/questions/5815675/what-is-sock-dgram-and-sock-stream/10810040)。此外，在第一个参数与第二个参数可以推断出唯一的第三个参数时，第三个参数可以简略的写为0，当然，你也可以像此例一样明确指定为IPPROTO_TCP协议。在linux中一切皆文件，socket文件也是如此。此函数返回socket文件的文件描述符，可以使用 write() / read() 函数进行 I/O 操作。

随后创建了一个`sockaddr_in`结构，用于存储服务器地址与端口。首先先用 0 填充全部内容，这么做是因为此结构的部分内容需用 0 填充。跳转到具体结构，可以看到其注释为“一个描述 ipv4 socket 地址的结构”。第一个字段是一个宏，按其描述即为一个 2 字节的通过`##`粘连的 sin_family 字段，用于存储地址类型，此处的 AF_INET 与之前的同义。第二个字段是一个 2 字节的 sin_port，用于存储端口号。第三个字段为 sin_addr，其套娃了一个结构，用于存储具体的 IP 地址，大小为 4 字节，如此设计可能为历史原因。第四个字段为 sin_zero，字符数组，可以看到此字段即进行补 0 的操作，大小为 16-2-2-4=8 字节。

可以看到，s_addr 的类型其实为 unsigned int，而此处赋值为一个点分十进制的字符串，inet_addr 函数的作用即为将此字符串转换为 int 类型，同时将主机字节序转换为网络字节序，intel 处理器基本均为小端，而网络字节序为大端，所以此处理是必要的。同样，htons 函数也是将 short 类型的变量从主机字节序转为网络字节序。

随后服务器端要用 bind 函数将套接字与特定的 IP 地址和端口绑定起来，只有这样，流经该 IP 地址和端口的数据才能交给套接字处理。可以看到第二个参数将 sockaddr_in 结构显示转换为了通用的 sockaddr 结构，sockaddr 和 sockaddr_in 的长度相同，都是 16 字节，只是将 IP 地址和端口号合并到一起，用一个成员 sa_data 表示。这也就说明了为什么之前要进行补 0 的操作。

随后 listen 让套接字处于被动监听状态。所谓被动监听，是指套接字一直处于“睡眠”中，直到客户端发起请求才会被“唤醒”，随后再调用 accept 函数，就可以随时响应客户端的请求了。 第二个参数为请求队列的最大长度，当套接字正在处理客户端请求时，如果有新的请求进来，套接字是没法处理的，只能把它放进缓冲区，待当前请求处理完毕后，再从缓冲区中读取出来处理。如果不断有新的请求进来，它们就按照先后顺序在缓冲区中排队，直到缓冲区满。这个缓冲区，就称为请求队列（Request Queue）。在一些代码里你会看到其被设置为 SOMAXCONN，意为由系统来决定请求队列长度。

> 注意：listen 只是让套接字处于监听状态，并没有接收请求。接收请求需要使用 accept 函数。

accept 返回一个新的套接字来和客户端通信，clnt_addr 保存了客户端的 IP 地址和端口号，而 serv_sock 是服务器端的套接字，大家注意区分。后面和客户端通信时，要使用这个新生成的套接字，而不是原来服务器端的套接字。再次强调，listen 只是让套接字进入监听状态，并没有真正接收客户端请求，listen 后面的代码会继续执行，直到遇到 accept。accept 会阻塞程序执行（后面代码不能被执行），直到有新的请求到来。如果你使用 printf 调试，记得务必加上\n,或使用`fflush(stdout);`刷新缓存区。

接下来使用 write 函数用来向套接字文件中写入数据，也就是向客户端发送数据。

最后关闭套接字，和普通文件一样，socket 在使用完毕后也要用 close 关闭。

---

接下来看一下客户端代码，很多地方是相同的。

[client](../src/network/socket/client.c ':include')

不同之处是，建立套接字和 sockaddr_in 结构后，用 connect 函数向服务器发起请求，服务器的 IP 地址和端口号保存在 sockaddr_in 结构体中。直到服务器传回数据后，connect() 才运行结束。最后通过 read 从套接字文件中读取数据。

## 处理端口占用问题

如果在 server 代码中没有加入两个 if 的判断代码块，当你执行完一次从 client 到 server 的请求后，如果你尝试再此重启启动 server,会发现程序直接退出了，而没有任何报错。这是因为在 bind 的时候，如果检测到了失败，我们直接返回了-1,并没有任何错误处理。通过`echo $?`也可以看到程序的退出码是 255(-1 转换为无符号八比特数即为 255)。这里我们加上错误处理函数看看报错是什么：

```cpp
if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
  printf("error!%s", strerror(errno));
  return -1;
}
```

errno 是定义在 errno.h 中的全局错误变量。strerror 是 string.h 中用来描述全局错误变量 errno 的函数。重新执行程序我们就可以看到错误，error!Address already in use。这是因为当服务端已经响应了某个客户端的请求，并关闭之后，如果立刻重启服务端，bind 会失败，因为操作系统要求一个绑定延时，在上次响应后的 30 秒，任何程序都不能重新绑定这个端口，包括上次绑定的程序自身。这里就需要在 bind 前通过额外的设置选项来解决这个问题。

```cpp
int reuse = 1;
if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) ==
      -1) {
    printf("error!%s", strerror(errno));
    return -1;
  }
```

这里为服务端套接字通过 setsockopt 设置选项。SOL_SOCKET 代表 socket 层自身，SO_REUSEADDR 表示可重用端口。再次测试，可以发现已经没有错误发生了。

## 双向数据传输

我们再做一些改进。目前服务端只能响应单次请求，而且数据的流向仅仅为服务器发出，客户端收取，实际上套接字数据是可以双向流动的，双方都可以收发数据。同样先看服务端部分代码。

[loop_server](../src/network/socket/loop_server.c ':include')

接受请求部分被放在 while 循环中，服务端接受客户端数据，并原样返回。
下面来看客户端部分代码。

[loop_client](../src/network/socket/loop_client.c ':include')

可以注意到，客户端中需要将 socket 创建与连接等操作放在 while 循环内部。因为服务器中调用 close(clnt_sock);不仅会关闭服务器端的 clnt_sock，还会通知客户端连接已断开，客户端也会清理 socket 相关资源，所以每次请求完毕都会清理 socket，下次发起请求时都需要重新创建。

## socket 缓冲区

每个 socket 被创建后，都会分配两个缓冲区，输入缓冲区和输出缓冲区。

write()/send() 并不立即向网络中传输数据，而是先将数据写入缓冲区中，再由 TCP 协议将数据从缓冲区发送到目标机器。一旦将数据写入到缓冲区，函数就可以成功返回，不管它们有没有到达目标机器，也不管它们何时被发送到网络，这些都是 TCP 协议负责的事情。

TCP 协议独立于 write()/send() 函数，数据有可能刚被写入缓冲区就发送到网络，也可能在缓冲区中不断积压，多次写入的数据被一次性发送到网络，这取决于当时的网络情况、当前线程是否空闲等诸多因素，不由程序员控制。可以通过如下代码来查看发送区 buffer 的大小，这里为 16384 即 16K 字节。

[socket_buffer](../src/network/socket/socket_buffer.c ':include')

## TCP 数据流的无边界性

上面讲到了 socket 缓冲区和数据的传递过程，可以看到数据的接收和发送是无关的，read()/recv() 函数不管数据发送了多少次，都会尽可能多的接收数据。也就是说，read()/recv() 和 write()/send() 的执行次数可能不同。

例如，write()/send() 重复执行三次，每次都发送字符串"abc"，那么目标机器上的 read()/recv() 可能分三次接收，每次都接收"abc"；也可能分两次接收，第一次接收"abcab"，第二次接收"cabc"；也可能一次就接收到字符串"abcabcabc"。

假设我们希望客户端每次发送一位学生的学号，让服务器端返回该学生的姓名、住址、成绩等信息，这时候可能就会出现问题，服务器端不能区分学生的学号。例如第一次发送 1，第二次发送 3，服务器可能当成 13 来处理，返回的信息显然是错误的。

这很好的反映了 TCP 数据流无边界的特性，客户端发送的多个数据包可能被当做一个数据包接收。read()/recv() 函数不知道数据包的开始或结束标志（实际上也没有任何开始或结束标志），只把它们当做连续的数据流来处理。

下面的代码演示了 TCP 数据流无边界的特性，客户端连续三次向服务器端发送数据，服务器端却一次性接收到所有数据。

[borderless_server](../src/network/socket/borderless_server.c ':include')

客户端

[borderless_client](../src/network/socket/borderless_client.c ':include')

服务端有一行 sleep(5)，它让程序暂停执行 5 秒。在这段时间内，client 连续三次发送字符串"abc"，由于 server 被阻塞，数据只能堆积在缓冲区中，5 秒后，server 开始运行，从缓冲区中一次性读出所有积压的数据，并返回给客户端。

另外还需要说明的是 client 执行到 recv() 函数，由于输入缓冲区中没有数据，所以会被阻塞，直到 5 秒后 server 传回数据才开始执行。用户看到的直观效果就是，client 暂停一段时间才输出 server 返回的结果。

client 的 send() 发送了三个数据包，而 server 的 recv() 却只接收到一个数据包，这很好的说明了 TCP 数据流无边界的特性。

## 一个文件传输功能的实现

这节我们来完成 socket 文件传输程序，这是一个非常实用的例子。要实现的功能为：client 从 server 下载一个文件并保存到本地。

文件大小不确定，有可能比缓冲区大很多，调用一次 write()/send() 函数不能完成文件内容的发送。接收数据时也会遇到同样的情况。要解决这个问题，可以使用 while 循环。对于 Server 端的代码，当读取到文件末尾，fread() 会返回 0，结束循环。对于 Client 端代码，有一个关键的问题，就是文件传输完毕后让 recv() 返回 0，结束 while 循环。注意，读取完缓冲区中的数据 recv() 并不会返回 0，而是被阻塞，直到缓冲区中再次有数据。最简单的结束 while 循环的方法当然是文件接收完毕后让 recv() 函数返回 0，那么，如何让 recv() 返回 0 呢？recv() 返回 0 的唯一时机就是收到 FIN 包时。FIN 包表示数据传输完毕，计算机收到 FIN 包后就知道对方不会再向自己传输数据，当调用 read()/recv() 函数时，如果缓冲区中没有数据，就会返回 0，表示读到了”socket 文件的末尾“。

这里我们调用 shutdown() 来发送 FIN 包：server 端直接调用 close()/closesocket() 会使输出缓冲区中的数据失效，文件内容很有可能没有传输完毕连接就断开了，而调用 shutdown() 会等待输出缓冲区中的数据传输完毕。

服务端:

[download_server](../src/network/socket/download_server.c ':include')

客户端：

[download_client](../src/network/socket/download_client.c ':include')

## 基于 UDP 的 socket 传输

UDP 不像 TCP，无需在连接状态下交换数据，因此基于 UDP 的服务器端和客户端也无需经过连接过程。也就是说，不必调用 listen() 和 accept() 函数。UDP 中只有创建套接字的过程和数据交换的过程。

TCP 中，套接字是一对一的关系。如要向 10 个客户端提供服务，那么除了负责监听的套接字外，还需要创建 10 套接字。但在 UDP 中，不管是服务器端还是客户端都只需要 1 个套接字。UDP 原理的可以类比邮寄包裹，负责邮寄包裹的快递公司可以比喻为 UDP 套接字，只要有 1 个快递公司，就可以通过它向任意地址邮寄包裹。同样的，只需 1 个 UDP 套接字就可以向任意主机传送数据。

创建好 TCP 套接字后，传输数据时无需再添加地址信息，因为 TCP 套接字将保持与对方套接字的连接。换言之，TCP 套接字知道目标地址信息。但 UDP 套接字不会保持连接状态，每次传输数据都要添加目标地址信息，这相当于在邮寄包裹前填写收件人地址。

UDP 连接主要使用 sendto 和 recvfrom 两个函数。sendto 函数的各个参数含义如下所示：

- sock：用于传输 UDP 数据的套接字；
- buf：保存待传输数据的缓冲区地址；
- nbytes：带传输数据的长度（以字节计）；
- flags：可选项参数，若没有可传递 0；
- to：存有目标地址信息的 sockaddr 结构体变量的地址；
- addrlen：传递给参数 to 的地址值结构体变量的长度。

recvfrom 函数的各个参数含义如下所示：

- sock：用于接收 UDP 数据的套接字；
- buf：保存接收数据的缓冲区地址；
- nbytes：可接收的最大字节数（不能超过 buf 缓冲区的大小）；
- flags：可选项参数，若没有可传递 0；
- from：存有发送端地址信息的 sockaddr 结构体变量的地址；
- addrlen：保存参数 from 的结构体变量长度的变量地址值。

下面结合之前的内容实现回声客户端。需要注意的是，UDP 不同于 TCP，不存在请求连接和受理过程，因此在某种意义上无法明确区分服务器端和客户端，只是因为其提供服务而称为服务器端，希望各位读者不要误解。

需要注意的是，在创建套接字时，向 socket() 第二个参数传递 SOCK_DGRAM，以指明使用 UDP 协议。同时在服务端代码中指定 ip 地址时，使用 htonl(INADDR_ANY)来自动获取 IP 地址。

利用常数 INADDR_ANY 自动获取 IP 地址有一个明显的好处，就是当软件安装到其他服务器或者服务器 IP 地址改变时，不用再更改源码重新编译，也不用在启动软件时手动输入。而且，如果一台计算机中已分配多个 IP 地址（例如路由器），那么只要端口号一致，就可以从不同的 IP 地址接收数据。所以，服务器中优先考虑使用 INADDR_ANY；而客户端中除非带有一部分服务器功能，否则不会采用。

服务端

[udp_server](../src/network/socket/udp_server.c ':include')

客户端

[udp_client](../src/network/socket/udp_client.c ':include')

在 TCP 客户端程序中，其通过 connect 函数完成把 IP 和端口分配给套接字的工作。而在 UDP 客户端程序中，是通过 sendto 完成此工作。如果调用 sendto 函数时，发现尚未给套接字分配 IP 和端口，则会进行自动分配，IP 使用主机 IP,端口号随机选择任意未使用端口。分配后的地址可以一直使用到程序结束为止，可用来与其他套接字继续进行数据交换。在代码中，可以通过 getsockname 函数观察此过程。

## 在 socket 中使用域名

当需要直接使用域名，而不是 ip 地址时，可以使用 getaddrinfo 函数，其同时支持 ipv4 与 ipv6。老旧的 gethostbyname 函数仅支持 ipv4,且已被 man 标记为 obsolete 状态。

使用 gethostbyname 这个函数可将域名转换出 ip 地址等数据，参阅如下下代码。

[gethostbyname](../src/network/socket/gethostbyname.c ':include')

getaddrinfo 会在堆上创建一个新的 addrinfo 结构用来存储 ip 地址等信息。随后便可用此结构中的数据进行新建 socket 与 connect 操作。由于此结构在堆上创建，最后要用 freeaddrinfo 来清除它。

[getaddrinfo](../src/network/socket/getaddrinfo.c ':include')

## 阻塞式 I/O 配合多进程

在前面全部的例子中，我们在服务端均只使用一个进程，当有多个客户端进行连接时，后面的客户端只能等待服务端处理完当前的客户端后，重新调用 accept 才可以被处理。这里提供一个使用服务端 fork 子进程的方式来同时服务多个客户端的例子。阻塞式 I/O 配合多进程可以较好的应用于所需要处理的文件描述符不多的场景。需要明确的是，服务端父进程只用来处理主要监听套接字，而服务端子进程只用来处理 accept 创建的套接字。可以配合之前的循环客户端测试效果。

执行服务端后，再执行`ps al`命令，可以看到父进程和其子进程的条目。

[fork_server](../src/network/socket/fork_server.c ':include')

## 阻塞式 I/O 配合多线程

在应用场景对系统开销很敏感时，可以尝试使用多线程配合阻塞式 I/O,这种方式系统开销更短。同时使用线程，共享数据更为方便，在使用多进程时，进程间数据的共享需要借助管道一类的工具，较为麻烦。注意，编译时需要加入-lpthread 选项。可以配合之前的循环客户端测试效果。

[pthread_server](../src/network/socket/pthread_server.c ':include')

除此之外，还可以使用线程池或 Leader Follower 的方式改进使用体验。更多关于逻辑处理的内容会在随后的相关章节进行展开。
