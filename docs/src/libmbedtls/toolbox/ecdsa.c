#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/entropy.h>
#include <mbedtls/md.h>
#include <mbedtls/platform.h>

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
  char buf[97];
  uint8_t hash[32], msg[100];
  char *pers = "simple_ecdsa";
  size_t rlen, slen, qlen, dlen;
  memset(msg, 0x12, sizeof(msg));

  //签名的两部分结果
  mbedtls_mpi r, s;
  mbedtls_ecdsa_context ctx;
  mbedtls_md_context_t md_ctx;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;

  mbedtls_mpi_init(&r);
  mbedtls_mpi_init(&s);
  mbedtls_ecdsa_init(&ctx);
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                              (const uint8_t *)pers, strlen(pers));
  assert_exit(ret == 0, ret);
  mbedtls_printf("\n  . setup rng ... ok\n\n");

  //进行消息摘要结算
  mbedtls_md_init(&md_ctx);
  mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), msg, sizeof(msg),
             hash);
  mbedtls_printf("  1. hash msg ... ok\n");

  //生成密钥对
  ret = mbedtls_ecdsa_genkey(&ctx, MBEDTLS_ECP_DP_SECP256R1,
                             mbedtls_ctr_drbg_random, &ctr_drbg);
  assert_exit(ret == 0, ret);

  //写入公钥
  mbedtls_ecp_point_write_binary(&ctx.private_grp, &ctx.private_Q,
                                 MBEDTLS_ECP_PF_UNCOMPRESSED, &qlen,
                                 (unsigned char *)buf, sizeof(buf));
  dlen = mbedtls_mpi_size(&ctx.private_d);
  //在公钥后写入私钥
  mbedtls_mpi_write_binary(&ctx.private_d, (unsigned char *)buf + qlen, dlen);
  dump_buf("  2. ecdsa generate keypair:", (unsigned char *)buf, qlen + dlen);

  //用私钥进行签名，r、s为签名的两部分结果
  ret = mbedtls_ecdsa_sign(&ctx.private_grp, &r, &s, &ctx.private_d, hash,
                           sizeof(hash), mbedtls_ctr_drbg_random, &ctr_drbg);
  assert_exit(ret == 0, ret);
  rlen = mbedtls_mpi_size(&r);
  slen = mbedtls_mpi_size(&s);

  //写入签名结果r
  mbedtls_mpi_write_binary(&r, (unsigned char *)buf, rlen);
  //在签名结果r后写入签名结果s
  mbedtls_mpi_write_binary(&s, (unsigned char *)buf + rlen, slen);
  dump_buf("  3. ecdsa generate signature:", (unsigned char *)buf, rlen + slen);

  //用公钥验证签名
  ret = mbedtls_ecdsa_verify(&ctx.private_grp, hash, sizeof(hash),
                             &ctx.private_Q, &r, &s);
  assert_exit(ret == 0, ret);
  mbedtls_printf("  4. ecdsa verify signature ... ok\n\n");

cleanup:
  mbedtls_mpi_free(&r);
  mbedtls_mpi_free(&s);
  mbedtls_md_free(&md_ctx);
  mbedtls_ecdsa_free(&ctx);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);

  return (ret != 0);
}
