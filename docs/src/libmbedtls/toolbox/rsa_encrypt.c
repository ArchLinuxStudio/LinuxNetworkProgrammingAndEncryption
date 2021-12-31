#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/platform.h>
#include <mbedtls/rsa.h>

#define assert_exit(cond, ret)                                                 \
  do {                                                                         \
    if (!(cond)) {                                                             \
      printf("  !. assert: failed [line: %d, error: -0x%04X]\n", __LINE__,     \
             -ret);                                                            \
      goto cleanup;                                                            \
    }                                                                          \
  } while (0)

static void dump_buf(char *info, uint8_t *buf, uint32_t len) {
  mbedtls_printf("%s", info);
  for (int i = 0; i < len; i++) {
    mbedtls_printf("%s%02X%s", i % 16 == 0 ? "\n     " : " ", buf[i],
                   i == len - 1 ? "\n" : "");
  }
}

static void dump_rsa_key(mbedtls_rsa_context *ctx) {
  size_t olen;
  uint8_t buf[516];
  mbedtls_printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");
  mbedtls_mpi_write_string(&ctx->private_N, 16, (char *)buf, sizeof(buf),
                           &olen);
  mbedtls_printf("N: %s\n", buf);

  mbedtls_mpi_write_string(&ctx->private_E, 16, (char *)buf, sizeof(buf),
                           &olen);
  mbedtls_printf("E: %s\n", buf);

  mbedtls_mpi_write_string(&ctx->private_D, 16, (char *)buf, sizeof(buf),
                           &olen);
  mbedtls_printf("D: %s\n", buf);

  mbedtls_mpi_write_string(&ctx->private_P, 16, (char *)buf, sizeof(buf),
                           &olen);
  mbedtls_printf("P: %s\n", buf);

  mbedtls_mpi_write_string(&ctx->private_Q, 16, (char *)buf, sizeof(buf),
                           &olen);
  mbedtls_printf("Q: %s\n", buf);

  mbedtls_mpi_write_string(&ctx->private_DP, 16, (char *)buf, sizeof(buf),
                           &olen);
  mbedtls_printf("DP: %s\n", buf);

  mbedtls_mpi_write_string(&ctx->private_DQ, 16, (char *)buf, sizeof(buf),
                           &olen);
  mbedtls_printf("DQ: %s\n", buf);

  mbedtls_mpi_write_string(&ctx->private_QP, 16, (char *)buf, sizeof(buf),
                           &olen);
  mbedtls_printf("QP: %s\n", buf);
  mbedtls_printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");
}

int main(void) {
  int ret;
  size_t olen = 0;
  uint8_t out[2048 / 8];

  mbedtls_rsa_context ctx;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  const char *pers = "simple_rsa";
  const char *msg = "Hello, World!";

  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_rsa_init(&ctx);
  //指定填充方式为OAEP,单向散列算法为SHA256
  mbedtls_rsa_set_padding(&ctx, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
  ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                              (const uint8_t *)pers, strlen(pers));
  assert_exit(ret == 0, ret);
  mbedtls_printf("\n  . setup rng ... ok\n");

  mbedtls_printf("\n  ! RSA Generating large primes may take minutes! \n");
  ret = mbedtls_rsa_gen_key(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg, 2048,
                            65537);
  assert_exit(ret == 0, ret);
  mbedtls_printf("\n  1. RSA generate key ... ok\n");
  dump_rsa_key(&ctx);

  ret = mbedtls_rsa_pkcs1_encrypt(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg,
                                  strlen(msg), (unsigned char *)msg, out);
  assert_exit(ret == 0, ret);
  dump_buf("\n  2. RSA encryption ... ok", out, sizeof(out));

  ret = mbedtls_rsa_pkcs1_decrypt(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg,
                                  &olen, out, out, sizeof(out));
  assert_exit(ret == 0, ret);

  //'\0'截断
  out[olen] = 0;
  mbedtls_printf("\n  3. RSA decryption ... ok\n     %s\n", out);

  ret = memcmp(out, msg, olen);
  assert_exit(ret == 0, ret);
  mbedtls_printf("\n  4. RSA Compare results and plaintext ... ok\n");

cleanup:
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
  mbedtls_rsa_free(&ctx);

  return ret;
}
