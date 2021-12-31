#include <sodium.h>
#include <stdio.h>

int main() {
  char hex[9]; // 2*4 + 1 = 9
  unsigned char bin[5] = "AAAA";

  sodium_bin2hex(hex, 9, bin, 4);
  puts(hex);
}