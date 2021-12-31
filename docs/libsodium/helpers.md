# 辅助函数

> 应尽可能使用这些函数，以抵御「时序攻击」。

## 测试字节序列

函数 sodium_memcmp() 可完成两个等长字节序列的对比。

```c++
int sodium_memcmp(const void * const b1_, const void * const b2_, size_t len);
```

如果位于 b1* 的 len 个字节和位于 b2* 的 len 个字节相同，函数返回 0，否则返回 -1。

[memcmp](../src/libsodium/helpers/memcmp.c ':include')

```txt
Match
No match
```

函数 sodium_is_zero() 可判断给定的字节序列是否全为 0。

```c
int sodium_is_zero(const unsigned char *n, const size_t nlen);
```

若位于 n 的 nlen 个字节是否全为 0，则返回 1，否则返回 0。

## 字节序列的十六进制表示

函数 sodium_bin2hex() 可获取字节序的十六进制表示，并由此得到一个字符串。

```c++
char *sodium_bin2hex(char * const hex, const size_t hex_maxlen,
                     const unsigned char * const bin, const size_t bin_len);
```

函数将字符串写到 hex，这个字符串就是从 bin 开始的 bin_len 个字节的十六进制表示，包括 '\0'，故 hex_maxlen 至少为 2\*bin_len + 1。该函数始终返回 hex。

[bin2hex](../src/libsodium/helpers/bin2hex.c ':include')

```bash
41414141 #41为A的十六进制asc值
```

---

函数 sodium_hex2bin() 作用相反，通过解析字节序列的十六进制表示，还原该字节序列。

```c++
int sodium_hex2bin(unsigned char * const bin, const size_t bin_maxlen,
                   const char * const hex, const size_t hex_len,
                   const char * const ignore, size_t * const bin_len,
                   const char ** const hex_end);
```

函数将字节序列写到 bin。bin_maxlen 表示允许写入的最大字节数。而位于 hex 的字符串应当是一个字节序列的十六进制表示，可以没有 '\0' 结尾，需要解析的长度由 hex_len 指定。

ignore 是需要跳过的字符组成的字符串。比如 ": " 表示跳过冒号和空格。此时 "69:FC"、"69 FC"、"69 : FC" 和 "69FC" 都视为合法的输入，并产生相同的输出。ignore 可以设为 NULL，表示不允许任何非法的字符出现。

函数返回 0 表示转换成功，同时 bin_len 会被设为解析得到的字节数；返回 -1 则表示失败。失败的情况有以下两种：

- 解析的结果超过 bin_maxlen 字节；
- 遇到非法字符时，如果前面的字符都能顺利解析，函数仍然返回 0，否则返回 -1。

无论如何 hex_end 总是会被设为下一个待解析的字符的地址。

[bin2hex](../src/libsodium/helpers/bin2hex.c ':include')

```bash
4: abcd, 7 #7为下一个待解析的字符地址的值
```

## Base64 编码 / 解码

函数 sodium_bin2base64() 可获取字节序列的 Base64 编码。

```c++
char *sodium_bin2base64(char * const b64, const size_t b64_maxlen,
                        const unsigned char * const bin, const size_t bin_len,
                        const int variant);
```

Base64 编码有多种变体，采用哪种变体由 variant 指定，有下列 4 种取值可选：

- sodium_base64_VARIANT_ORIGINAL
- sodium_base64_VARIANT_ORIGINAL_NO_PADDING
- sodium_base64_VARIANT_URLSAFE
- sodium_base64_VARIANT_URLSAFE_NO_PADDING

这些 Base64 编码并不提供任何形式的加密；就像十六进制编码一样，任何人都可以对它们进行解码。

可以令 b64_maxlen 等于宏 sodium_base64_ENCODED_LEN(BIN_LEN, VARIANT)，它表示使用 VARIANT 这种变体时，BIN_LEN 个字节的 Base64 编码（包括 '\0'）的最小长度。

[bin2base64](../src/libsodium/helpers/bin2base64.c ':include')

```bash
9: aGVsbG8=
```

---

函数 sodium_base642bin() 可完成 Base64 解码工作。

```c++
int sodium_base642bin(unsigned char * const bin, const size_t bin_maxlen,
                      const char * const b64, const size_t b64_len,
                      const char * const ignore, size_t * const bin_len,
                      const char ** const b64_end, const int variant);
```

返回 -1 表示错误，返回 0 表示解码成功，同时 bin_len 会被设为解码得到的字节数，其他参数的含义参考前文。

[base642bin](../src/libsodium/helpers/base642bin.c ':include')

```bash
5: hello
```

## 大数的计算

函数 sodium_increment() 用来递增一个任意长度的无符号数。

```c++
void sodium_increment(unsigned char *n, const size_t nlen);
```

位于 n 的 nlen 字节的数字将按小端字节序处理。加密算法中经常提到的不重数 nonce 就可用此函数进行递增。

[increment](../src/libsodium/helpers/increment.c ':include')

```bash
1
2
3
```

---

函数 sodium_add() 可完成大数的加法。

```c++
void sodium_add(unsigned char *a, const unsigned char *b, const size_t len);
```

位于 a 和 b 的两个 nlen 字节的加数均按小端字节序的无符号数处理。计算结果将覆盖 a。

[increment](../src/libsodium/helpers/increment.c ':include')

```bash
1
2
```

---

函数 sodium_sub() 可完成大数减法。

```c++
void sodium_sub(unsigned char *a, const unsigned char *b, const size_t len);
```

位于 a 和 b 的两个 nlen 字节的加数均按小端字节序的无符号数处理。计算结果将覆盖 a。

---

函数 sodium_compare() 可完成两个大数的比较。两个大数均按小端字节序处理。

```c++
int sodium_compare(const void * const b1_, const void * const b2_, size_t len);
```

返回 0 表示相等，返回 -1 表示 b1* 小于 b2\*；返回 1 表示 b1* 大于 b2\*。
