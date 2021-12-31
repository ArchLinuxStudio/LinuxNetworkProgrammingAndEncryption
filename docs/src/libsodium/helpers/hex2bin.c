#include <sodium.h>
#include <stdio.h>

int main() {
  unsigned char bin[5] = {0};
  char hex[12] = "61*62636472";
  size_t bin_len = 0;
  const char *hex_end;
  sodium_hex2bin(bin, 4, hex, 9, "*", &bin_len, &hex_end);
  printf("%zu: %s, %c\n", bin_len, bin, *hex_end);
}