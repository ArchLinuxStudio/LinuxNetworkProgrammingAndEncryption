# 组成

shadowsocks-libev 有以下主要依赖项，我们在前面已经分别描述过他们的使用。

- [c-ares](https://archlinux.org/packages/extra/x86_64/c-ares/)
- [libbloom](https://archlinux.org/packages/community/x86_64/libbloom/)
- [libcap](https://archlinux.org/packages/core/x86_64/libcap/)
- [libcorkipset](https://archlinux.org/packages/community/x86_64/libcorkipset/)
- [libev](https://archlinux.org/packages/community/x86_64/libev/)
- [libsodium](https://archlinux.org/packages/extra/x86_64/libsodium/)
- [mbedtls](https://archlinux.org/packages/community/x86_64/mbedtls/)
- [pcre](https://archlinux.org/packages/core/x86_64/pcre/)

## 订阅协议

### SIP002

加密方式与密码进行 base64 可能是因为要减少字符类型，便于分割，因为密码里可能什么字符都有

base64 它们还可能是因为要做到 websafe

还可能是因为要避免搜索引擎搜索 详见这个 [issue](https://github.com/shadowsocks/shadowsocks-org/issues/50) 但是目前这个问题貌似不存在了

至于一些机场订阅整体 base64,应该就是历史客户端实现原因，以及能起到一些混淆视听的效果。

---

Ref:

1. [SIP002-URI-Scheme](https://shadowsocks.org/en/wiki/SIP002-URI-Scheme.html)
2. [SIP008-Online-Configuration-Delivery](https://shadowsocks.org/en/wiki/SIP008-Online-Configuration-Delivery.html)
3. [ss_subscribe_to_surge.js](https://gist.github.com/BirkhoffLee/cba0f8a3cfe5daa452f77383d92e26bd)
