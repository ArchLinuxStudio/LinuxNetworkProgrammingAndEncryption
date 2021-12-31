#include <sodium.h>
#include <stdio.h>
#include <string.h>
int main() {
  size_t bin_len;
  unsigned char bin[6];
  char b64[9] = "aGVsbG8=";
  sodium_base642bin(bin, sizeof bin, b64, strlen(b64), "", &bin_len, NULL,
                    sodium_base64_VARIANT_ORIGINAL);
  printf("%zu: %s\n", bin_len, bin);
}