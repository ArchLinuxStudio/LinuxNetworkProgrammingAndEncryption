# 密钥的派生

在实际应用中，不应从始至终都使用同一个密钥，更不能直接使用密码（通常是简短的字符串）作为密钥，否则很容易遭受「字典攻击」。应当为每次会话专门准备一个子密钥。这就需要一种能够产生大量子密钥的机制。

## KDF

KDF 是 Key Derivation Function 的缩写，即密钥派生函数。能够满足上述需求。这类函数通过引入随机数、增加散列迭代次数，增加暴力破解难度。常用的 KDF 有：

- PBKDF2
- Scrypt
- Argon2

Argon2 是最新的算法，也是 Libsodium 首推及其底层默认使用的算法。

## 基于密码派生密钥

根据给定的密码和一个长度固定的随机数生成指定长度的密钥。

```c
int crypto_pwhash(unsigned char * const out,
                  unsigned long long outlen,
                  const char * const passwd,
                  unsigned long long passwdlen,
                  const unsigned char * const salt,
                  unsigned long long opslimit,
                  size_t memlimit, int alg);
```

函数 crypto_pwhash() 根据 passwdlen 字节的密码 passwd 和 crypto_pwhash_SALTBYTES 字节的随机数 salt 派生出 outlen 字节的密钥并储存到 out 中。全部参数相同时，生成相同的密钥。

| \      | passwdlen                | outlen                  |
| ------ | ------------------------ | ----------------------- |
| 最小值 | crypto_pwhash_PASSWD_MIN | crypto_pwhash_BYTES_MIN |
| 最大值 | crypto_pwhash_PASSWD_MAX | crypto_pwhash_BYTES_MAX |

倒数两个参数 opslimit 和 memlimit 与性能和内存占用有关，取值如下：

| \         | opslimit                           | memlimit                           |
| --------- | ---------------------------------- | ---------------------------------- |
| 最小值    | crypto_pwhash_OPSLIMIT_MIN         | crypto_pwhash_MEMLIMIT_MIN         |
| 较快 / 小 | crypto_pwhash_OPSLIMIT_INTERACTIVE | crypto_pwhash_MEMLIMIT_INTERACTIVE |
| 中等      | crypto_pwhash_OPSLIMIT_MODERATE    | crypto_pwhash_MEMLIMIT_MODERATE    |
| 较慢 / 大 | crypto_pwhash_OPSLIMIT_SENSITIVE   | crypto_pwhash_MEMLIMIT_SENSITIVE   |
| 最大值    | crypto_pwhash_OPSLIMIT_MAX         | crypto_pwhash_MEMLIMIT_MAX         |

最后一个参数 alg 决定选用的算法，只有下列 3 种取值可选：

- crypto_pwhash_ALG_DEFAULT Libsodium 推荐的选项。
- crypto_pwhash_ALG_ARGON2I13 Argon2i 1.3。
- crypto_pwhash_ALG_ARGON2ID13 Argon2id 1.3。

函数返回 0 表示成功；返回 -1 表示失败（这通常是由于操作系统拒绝分配请求的内存）。

[crypto_pwhash](../src/libsodium/crypto_pwhash.c ':include')

```txt
key: a5c2d5ca23026834f7ff177fb8137b62
```

## 基于主密钥派生子密钥

根据一个主密钥生成多个子密钥。Libsodium 专门为此提供了两个函数 crypto_kdf\_\*()。

这两个函数可以根据一个主密钥 key 和一个被称为上下文的参数 ctx 派生出 2^64 个密钥，并且单个子密钥的长度可以在 128（16 字节）到 512 位（64 字节）之间。

```c
void crypto_kdf_keygen(uint8_t key[crypto_kdf_KEYBYTES]);
```

函数 crypto_kdf_keygen() 的作用是生成一个主密钥。

```c
int crypto_kdf_derive_from_key(unsigned char *subkey, size_t subkey_len,
                               uint64_t subkey_id,
                               const char ctx[crypto_kdf_CONTEXTBYTES],
                               const unsigned char key[crypto_kdf_KEYBYTES]);
```

函数 crypto_kdf_derive_from_key() 可以根据主密钥 key 和上下文 ctx 派生出长度为 subkey_len 字节的子密钥。subkey_id 是子密钥的编号，可以是不大于 2^64 - 1 的任意值。

主密钥的长度必须是 crypto_kdf_KEYBYTES。子密钥的长度 subkey_len 必须介于 crypto_kdf_BYTES_MIN（含）和 crypto_kdf_BYTES_MAX（含）之间。

上下文 ctx 是一个 8 字符的字符串，应能描述子密钥的用途。不需要保密，并且强度可以很低。比如 "UserName"、"**auth**"、"pictures" 和 "userdata" 等。但其长度必须是 crypto_kdf_CONTEXTBYTES 字节。

使用相同的密钥，但使用不同的 ctx，就会得到不同的输出。正如其名，ctx 可以和程序的上下文对应。当然，就算一个程序从头到尾只使用一个 ctx，那也有防止密钥被不同程序重复使用的作用。

[derive_key](../src/libsodium/derive_key.c ':include')

```txt
subkey1: 0440b65332dc5f6b4a46d262996af08e
subkey2: 73e6d9bbfb25c25d3898ba435f16b710
subkey3: 9fdffff7fd9d4ba7a8b1172c79cdf86b7a823256b418e9a61cb8e21f1170ef1f
```
