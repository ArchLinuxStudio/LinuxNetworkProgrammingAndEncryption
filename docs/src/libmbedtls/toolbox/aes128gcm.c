/*
 * @file
 * @brief chapter gcm samples.
 * @note  测试向量参考
 * http://csrc.nist.gov/groups/STM/cavp/documents/mac/gcmtestvectors.zip
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <mbedtls/cipher.h>
#include <mbedtls/platform.h>

#define assert_exit(cond, ret)                                                 \
  do {                                                                         \
    if (!(cond)) {                                                             \
      printf("  !. assert: failed [line: %d, error: -0x%04X]\n", __LINE__,     \
             -ret);                                                            \
      goto cleanup;                                                            \
    }                                                                          \
  } while (0)

int main(void) {
  char *key = "abcdefghijklmnop";
  char *input = "WTF AES GCM Example code!";
  char *iv = "babababababab";
  char *add = "zxczxxxxxxxsdfadascxz";
  unsigned char output[64] = {0};
  unsigned char fin[64] = {0};
  unsigned char tag_buf[16] = {0};
  size_t len;

  mbedtls_cipher_context_t ctx;
  const mbedtls_cipher_info_t *info;

  mbedtls_cipher_init(&ctx);
  info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_GCM);

  mbedtls_cipher_setup(&ctx, info);
  mbedtls_printf("cipher info setup, name: %s, block size: %d\n",
                 mbedtls_cipher_get_name(&ctx),
                 mbedtls_cipher_get_block_size(&ctx));

  mbedtls_cipher_setkey(&ctx, (const unsigned char *)key, strlen(key) * 8,
                        MBEDTLS_ENCRYPT);

  mbedtls_cipher_auth_encrypt_ext(&ctx, (const unsigned char *)iv, strlen(iv),
                                  (const unsigned char *)add, strlen(add),
                                  (const unsigned char *)input, strlen(input),
                                  output, 64, &len, 16);

  // detach tag
  memcpy(tag_buf, output + (len - 16), 16);

  printf("encrypt and tag data:");
  for (int i = 0; i < len; i++) {
    char str[3];
    sprintf(str, "%02x", (int)output[i]);
    printf("%s", str);
  }
  printf("\ntag:");
  for (int i = 0; i < 16; i++) {
    char str[3];
    sprintf(str, "%02x", (int)tag_buf[i]);
    printf("%s", str);
  }

  mbedtls_cipher_setkey(&ctx, (const unsigned char *)key, strlen(key) * 8,
                        MBEDTLS_DECRYPT);

  int ret = mbedtls_cipher_auth_decrypt_ext(
      &ctx, (const unsigned char *)iv, strlen(iv), (const unsigned char *)add,
      strlen(add), output, len, fin, 64, &len, 16);

  assert_exit(ret == 0, ret);
  printf("\ndecrypt:");
  for (int i = 0; i < strlen(input); i++) {
    char str[3];
    sprintf(str, "%c", (int)fin[i]);
    printf("%s", str);
  }
  printf("\n");

cleanup:
  mbedtls_cipher_free(&ctx);

  return (0);
}
