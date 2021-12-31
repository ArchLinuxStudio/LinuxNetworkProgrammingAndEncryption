#include <sodium.h>
#include <stdio.h>
int main() {
  unsigned char a[8] = {1};
  unsigned char b[8] = {1};
  printf("%d\n", *(int *)a);

  sodium_add(a, b, sizeof a); // a = a + b
  printf("%d\n", *(int *)a);
}
