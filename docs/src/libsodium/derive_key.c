#include <sodium.h>

int main(void) {
  if (sodium_init() == -1) {
    return 1;
  }

  char ctx[] = "Examples";

  uint8_t master_key[crypto_kdf_KEYBYTES];
  uint8_t subkey1[16];
  uint8_t subkey2[16];
  uint8_t subkey3[32];

  // 创建主密钥
  crypto_kdf_keygen(master_key);

  // 派生子密钥
  crypto_kdf_derive_from_key(subkey1, sizeof subkey1, 1, ctx, master_key);
  crypto_kdf_derive_from_key(subkey2, sizeof subkey2, 2, ctx, master_key);
  crypto_kdf_derive_from_key(subkey3, sizeof subkey3, 3, ctx, master_key);

  // 获取子密钥的十六进制表示
  char buf[1024];
  sodium_bin2hex(buf, sizeof buf, subkey1, sizeof subkey1);
  printf("subkey1: %s\n", buf);

  sodium_bin2hex(buf, sizeof buf, subkey2, sizeof subkey2);
  printf("subkey2: %s\n", buf);

  sodium_bin2hex(buf, sizeof buf, subkey3, sizeof subkey3);
  printf("subkey3: %s\n", buf);

  return 0;
}