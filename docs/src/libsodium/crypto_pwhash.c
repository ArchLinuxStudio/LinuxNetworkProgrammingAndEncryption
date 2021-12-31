#include <sodium.h>
#include <string.h>
int main(void) {
  if (sodium_init() == -1) {
    return 1;
  }

  // 密码
  char passwd[] = "secret";
  // 长度固定的随机数
  unsigned char salt[crypto_pwhash_SALTBYTES] = {0};
  // 密钥
  unsigned char key[16];

  if (crypto_pwhash(key, sizeof key, passwd, strlen(passwd), salt,
                    crypto_pwhash_OPSLIMIT_INTERACTIVE,
                    crypto_pwhash_MEMLIMIT_INTERACTIVE,
                    crypto_pwhash_ALG_DEFAULT) == -1)
    return -1;

  char buf[1024];
  sodium_bin2hex(buf, sizeof buf, key, sizeof key);
  printf("key: %s\n", buf);

  return 0;
}