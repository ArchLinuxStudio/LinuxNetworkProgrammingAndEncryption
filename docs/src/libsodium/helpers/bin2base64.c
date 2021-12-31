#include <sodium.h>
#include <stdio.h>
int main() {
  unsigned char bin[6] = "hello";
  int b64_len = sodium_base64_ENCODED_LEN(5, sodium_base64_VARIANT_ORIGINAL);
  char b64[b64_len];

  sodium_bin2base64(b64, b64_len, bin, 5, sodium_base64_VARIANT_ORIGINAL);
  printf("%d: %s\n", b64_len, b64);
}