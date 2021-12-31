#include <stdio.h>

int main() {
  char *cards = "JQK";
  cards[2] = 'A';
  puts(cards);
  return 0;
}