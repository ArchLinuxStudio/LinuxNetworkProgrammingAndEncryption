#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <mbedtls/bignum.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/platform.h>

// https://stackoverflow.com/questions/6325576/how-many-iterations-of-rabin-miller-should-i-use-for-cryptographic-safe-primes

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

int main(void) {
  int ret = 0;
  uint8_t prime[256];
  mbedtls_mpi P, Q;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  char *pers = "CTR_DRBG";

  mbedtls_mpi_init(&P);
  mbedtls_mpi_init(&Q);
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                              (const unsigned char *)pers, strlen(pers));
  assert_exit(ret == 0, ret);
  mbedtls_printf("\n  . setup rng ... ok\n");

  mbedtls_printf("\n  ! Generating large primes may take minutes!\n");
  ret = mbedtls_mpi_gen_prime(&P, sizeof(prime) * 8,
                              MBEDTLS_MPI_GEN_PRIME_FLAG_DH,
                              mbedtls_ctr_drbg_random, &ctr_drbg);
  assert_exit(ret == 0, ret);

  ret = mbedtls_mpi_sub_int(&Q, &P, 1);
  assert_exit(ret == 0, ret);

  ret = mbedtls_mpi_div_int(&Q, NULL, &Q, 2);
  assert_exit(ret == 0, ret);

  ret = mbedtls_mpi_is_prime_ext(&Q, 64, mbedtls_ctr_drbg_random, &ctr_drbg);
  assert_exit(ret == 0, ret);
  mbedtls_printf("\n  . Verifying that Q = (P-1)/2 is prime ... ok\n");

  mbedtls_mpi_write_binary(&P, prime, sizeof(prime));
  dump_buf("\n  . generate 2048 bit prime data ... ok", prime, sizeof(prime));

cleanup:
  mbedtls_mpi_free(&P);
  mbedtls_mpi_free(&Q);
  mbedtls_entropy_free(&entropy);
  mbedtls_ctr_drbg_free(&ctr_drbg);

  return 0;
}
