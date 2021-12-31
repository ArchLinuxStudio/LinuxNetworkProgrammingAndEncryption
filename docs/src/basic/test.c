#include <sodium.h>
#include <sodium/core.h>
#include <sodium/crypto_aead_xchacha20poly1305.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
int count = 0;

int main() {
  // static int count = 1;
  // count++;
  // printf("%i", count);
  // typedef union {
  //   float lemon;
  //   int aaa;
  // } lemon_lime;

  // typedef struct {
  //   float a;
  //   lemon_lime citrus;
  // } mara;

  // mara m = {2.0, .citrus.lemon = 2};

  // printf("%2.1f", m.citrus.lemon);
#if 0
  time_t seconds;

  seconds = time(&seconds);
  printf("自 1970-01-01 起的小时数 = %ld\n", seconds / 3600);

  pid_t a = fork();

  if (!a) {
    puts("子进程开始");
    int bbb = getpid();
    printf("%d", bbb);
    puts("子进程结束");
  }
  puts("夫进程开始");
  int bbb = getpid();
  printf("%d", bbb);
  puts("夫进程结束");

  printf("%ld", random());
#endif

#if 0
  char a;
  scanf("%c", &a);
  switch (a) {
  case 'a':
    puts("a");
    break;
  case 'b':
    puts("b");
    break;
  case '?':
    puts("unknow");
  }

#endif

  if (sodium_init() < 0) {
    return -1;
  }

  //密钥
  unsigned char k[crypto_aead_xchacha20poly1305_IETF_KEYBYTES] = "123456";
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

  //获取密文和报文鉴别码的十六进制表示
  char buf[1024];
  sodium_bin2hex(buf, sizeof buf, c, 5);
  printf("Ciphertext: %s\n", buf);

  sodium_bin2hex(buf, sizeof buf, mac, maclen);
  printf("MAC: %s\n", buf);

  return 0;
}
