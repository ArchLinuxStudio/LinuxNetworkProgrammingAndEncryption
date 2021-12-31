#include <sodium.h>

int main() {
  if (sodium_init() == -1) {
    return -1;
  }
  //...
}