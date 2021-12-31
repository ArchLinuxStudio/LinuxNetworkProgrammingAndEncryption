#include <stdio.h>
#include <string.h>
int main() {
  char info[90];
  scanf("%89[^\n]", info);
  puts(info);
}