# 使用 chacha20-poly1305 加解密

Libsodium 为 ChaCha20-Poly1305 的三种版本分别提供了三组函数：

- crypto_aead_chacha20poly1305\_\*()
- crypto_aead_chacha20poly1305_ietf\_\*()
- crypto_aead_xchacha20poly1305_ietf\_\*()

这三组函数在用法上完全一致，因此只要掌握了其中一种，自然也就掌握了其余两种。

## 加密

```c
int crypto_aead_xchacha20poly1305_ietf_encrypt_detached(unsigned char *c,
                                                        unsigned char *mac,
                                                        unsigned long long *maclen_p,
                                                        const unsigned char *m,
                                                        unsigned long long mlen,
                                                        const unsigned char *ad,
                                                        unsigned long long adlen,
                                                        const unsigned char *nsec,
                                                        const unsigned char *npub,
                                                        const unsigned char *k)
```

函数 crypto_aead_xchacha20poly1305_ietf_encrypt_detached() 使用密钥 k 和
不重数 npub 对 mlen 字节的报文 m 进行加密，并根据密文和 adlen 字节的附加数据 ad 计算报文鉴别码。密文将被写到 c，而报文鉴别码将被写到 mac，maclen 会被设为 mac 的长度。

密文和明文等长。而密钥、不重数、报文鉴别码的长度都是固定的，它们分别等于：

- crypto_aead_xchacha20poly1305_ietf_KEYBYTES
- crypto_aead_xchacha20poly1305_ietf_NPUBBYTES
- crypto_aead_xchacha20poly1305_ietf_ABYTES

若没有关联的数据，则把 ad 设为 NULL，并把 adlen 设为 0。

> 此处 nsec 必须始终设为 NULL，下同。

[encrypt_detached](../src/libsodium/encrypt_detached.c ':include')

```txt
Ciphertext: 5abc40d737
MAC: 0be7cd4beaf9ec2a063170aab65fa5aa
```

> 函数 sodium_bin2hex() 是 Libsodium 提供的「辅助函数」，具体用法详见辅助函数一节。

在密钥不变的情况下，不重数必须每次都不一样。建议用 randombytes_buf() 函数产生第一条报文的不重数，再用 sodium_increment() 函数对其进行递增。

## 解密

解密必须提供相同的密钥 k、不重数 npub 和附加数据 ad。

```c

int crypto_aead_xchacha20poly1305_ietf_decrypt_detached(unsigned char *m,
                                                        unsigned char *nsec,
                                                        const unsigned char *c,
                                                        unsigned long long clen,
                                                        const unsigned char *mac,
                                                        const unsigned char *ad,
                                                        unsigned long long adlen,
                                                        const unsigned char *npub,
                                                        const unsigned char *k)
```

函数 crypto_aead_xchacha20poly1305_ietf_decrypt_detached() 首先验证 c 中包含的 tag 是否合法。若函数返回 -1 表示验证未通过；若验证通过，则返回 0，并将解密得到的报文写到 m。

[decrypt_detached](../src/libsodium/decrypt_detached.c ':include')

```txt
Message: hello
```

## 合并模式

以上这种将密文和报文鉴别码分开储存的方式称为分开模式。由于大多数需求都是将报文鉴别码直接追加到密文后面，即合并模式。因此，Libsodium 实际上为每种 AEAD 方案都提供两组函数：一组实现分开模式；另一组实现合并模式。

为合并模式设计的函数，相比于分开模式的函数，函数名少了后缀 \_detached。

```c
int crypto_aead_xchacha20poly1305_ietf_encrypt(unsigned char *c,
                                               unsigned long long *clen_p,
                                               const unsigned char *m,
                                               unsigned long long mlen,
                                               const unsigned char *ad,
                                               unsigned long long adlen,
                                               const unsigned char *nsec,
                                               const unsigned char *npub,
                                               const unsigned char *k)
```

密钥、不重数、附加数据、明文等参数的含义同上。在合并模式下，报文鉴别码直接追加到密文后面，因此减少了 mac 和 maclen 两个参数，但参数 c 必须为报文鉴别码预留存储空间。

[encrypt](../src/libsodium/encrypt.c ':include')

```txt
Ciphertext: 5abc40d737
MAC: 0be7cd4beaf9ec2a063170aab65fa5aa
Ciphertext: 5abc40d7370be7cd4beaf9ec2a063170aab65fa5aa
```
