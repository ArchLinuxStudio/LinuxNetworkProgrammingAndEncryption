#include <sodium.h>
#include <stdio.h>
int main() {
  char b1_[6] = "hello";
  char b2_[6] = "hello";
  char b3_[6] = "Hello";

  if (sodium_memcmp(b1_, b2_, 5) == -1) {
    puts("No match");
  } else {
    puts("Match");
  }

  if (sodium_memcmp(b1_, b3_, 5) == -1) {
    puts("No match");
  } else {
    puts("Match");
  }
}