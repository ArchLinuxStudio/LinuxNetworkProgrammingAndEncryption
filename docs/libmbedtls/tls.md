# TLS

本节将实现一个搭配 TLS 实现的 TCP 客户端，用以请求网页内容，这个客户端实际上用到了上一节中描述过的全部内容。首先来看一个没有使用 TLS 的客户端，它将请求一个网页。如果你掌握了网络编程部分一章，那么理解以下代码应该不难。

[client_without_tls](../src/libmbedtls/tls/client_without_tls.c ':include')

接下来将使用 mbedtls 为此程序加入 TLS,从而可以请求在某一端口 https 的网页。需要注意，这里的例子是一个网站的真实例子，其证书链共有三级，而实际上对服务器的验证只进行到中间一级的 CA 认证节点即可完成，不必追溯到顶部的 CA 根节点。

[client_with_tls_with_verify](../src/libmbedtls/tls/client_with_tls_with_verify.c ':include')

---

Ref:

1. [mbedtls discuss about middle cert](https://github.com/ARMmbed/mbedtls/issues/139#issuecomment-270134402)
2. [how-to-validate-tls-certificate](https://cjting.me/2021/03/02/how-to-validate-tls-certificate/)
3. [OpenSSL 简单思路和函数笔记](https://segmentfault.com/a/1190000005933931)
4. [mbedTLS 简单思路和函数笔记](https://segmentfault.com/a/1190000005998141)
