#include <sodium.h>

int main(void) {
  if (sodium_init() == -1) {
    return 1;
  }

  // 密钥
  unsigned char k[crypto_aead_xchacha20poly1305_ietf_KEYBYTES] = "123456";
  // 不重数
  unsigned char npub[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES] = {0};

  // 明文
  int mlen = 5;
  unsigned char m[6] = "hello";
  // 附加的数据
  int adlen = 4;
  unsigned char ad[5] = "2020";

  // 密文
  unsigned char c[6];
  // 报文鉴别码
  unsigned char mac[crypto_aead_xchacha20poly1305_ietf_ABYTES];
  unsigned long long maclen;

  // 加密
  crypto_aead_xchacha20poly1305_ietf_encrypt_detached(c, mac, &maclen, m, mlen,
                                                      ad, adlen, NULL, npub, k);
  // 获取密文和报文鉴别码的十六进制表示
  char buf[1024];
  sodium_bin2hex(buf, sizeof buf, c, 5);
  printf("Ciphertext: %s\n", buf);

  sodium_bin2hex(buf, sizeof buf, mac, maclen);
  printf("MAC: %s\n", buf);

  return 0;
}