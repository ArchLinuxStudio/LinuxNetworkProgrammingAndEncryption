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
  unsigned char m[6];
  // 附加的数据
  int adlen = 4;
  unsigned char ad[5] = "2020";

  // 密文
  int clen = 5;
  unsigned char c[6];
  sodium_hex2bin(c, clen, "5abc40d737", 10, NULL, NULL, NULL);

  // 报文鉴别码
  unsigned char mac[crypto_aead_xchacha20poly1305_ietf_ABYTES];
  sodium_hex2bin(mac, crypto_aead_xchacha20poly1305_ietf_ABYTES,
                 "0be7cd4beaf9ec2a063170aab65fa5aa", 32, NULL, NULL, NULL);

  // 解密
  if (crypto_aead_xchacha20poly1305_ietf_decrypt_detached(
          m, NULL, c, clen, mac, ad, adlen, npub, k) == -1)
    return -1;
  printf("Message: %s\n", m);

  return 0;
}