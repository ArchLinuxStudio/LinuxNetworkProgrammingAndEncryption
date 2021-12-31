#include <stdio.h>
int main() {
  char s[] = "How big is it?";
  char *t = s;
  printf("%s\n", s);          // How big is it?
  printf("%s\n", t);          // How big is it?
  printf("%lu\n", sizeof(s)); // 15
  printf("%lu\n", sizeof(t)); // 字符串指针大小为8
  return 0;
}