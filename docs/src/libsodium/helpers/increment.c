#include <sodium.h>
#include <stdio.h>
int main() {
  unsigned char nonce[8] = {0};

  sodium_increment(nonce, sizeof(nonce));

  printf("%d\n", *(int *)nonce);

  sodium_increment(nonce, sizeof(nonce));
  printf("%d\n", *(int *)nonce);

  sodium_increment(nonce, sizeof(nonce));
  printf("%d\n", *(int *)nonce);
}
