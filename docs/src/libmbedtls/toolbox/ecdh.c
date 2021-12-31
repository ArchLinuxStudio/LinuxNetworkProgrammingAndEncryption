/**
 *椭圆曲线标准:https://datatracker.ietf.org/doc/html/rfc4492
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/entropy.h>
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
  size_t olen;
  char buf[65];
  mbedtls_ecp_group grp;
  mbedtls_mpi cli_secret, srv_secret;
  mbedtls_mpi cli_pri, srv_pri;
  mbedtls_ecp_point cli_pub, srv_pub;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  char *pers = "simple_ecdh";

  // 初始化大数结构
  mbedtls_mpi_init(&cli_pri);
  mbedtls_mpi_init(&srv_pri);
  mbedtls_mpi_init(&cli_secret);
  mbedtls_mpi_init(&srv_secret);

  // 初始化椭圆曲线群结构体
  mbedtls_ecp_group_init(&grp);
  // 初始化椭圆曲线点结构体
  mbedtls_ecp_point_init(&cli_pub);
  mbedtls_ecp_point_init(&srv_pub);

  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                        (const uint8_t *)pers, strlen(pers));
  mbedtls_printf("\n  . setup rng ... ok\n");

  // 加载椭圆曲线 SECP256R1
  ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
  mbedtls_printf("\n  . select ecp group SECP256R1 ... ok\n");

  // 生成客户端 ECDH 密钥对, pri 与 pub
  ret = mbedtls_ecdh_gen_public(&grp, &cli_pri, &cli_pub,
                                mbedtls_ctr_drbg_random, &ctr_drbg);
  assert_exit(ret == 0, ret);
  //公钥采取非压缩模式 公钥04开头即为非压缩
  mbedtls_ecp_point_write_binary(&grp, &cli_pub, MBEDTLS_ECP_PF_UNCOMPRESSED,
                                 &olen, (unsigned char *)buf, sizeof(buf));
  dump_buf("  1. ecdh client generate public parameter:", (unsigned char *)buf,
           olen);

  // 生成服务端 ECDH 密钥对, pri 与 pub
  ret = mbedtls_ecdh_gen_public(&grp, &srv_pri, &srv_pub,
                                mbedtls_ctr_drbg_random, &ctr_drbg);
  assert_exit(ret == 0, ret);
  mbedtls_ecp_point_write_binary(&grp, &srv_pub, MBEDTLS_ECP_PF_UNCOMPRESSED,
                                 &olen, (unsigned char *)buf, sizeof(buf));
  dump_buf("  2. ecdh server generate public parameter:", (unsigned char *)buf,
           olen);

  // 客户端计算共享密钥
  ret = mbedtls_ecdh_compute_shared(&grp, &cli_secret, &srv_pub, &cli_pri,
                                    mbedtls_ctr_drbg_random, &ctr_drbg);
  assert_exit(ret == 0, ret);
  mbedtls_mpi_write_binary(&cli_secret, (unsigned char *)buf,
                           mbedtls_mpi_size(&cli_secret));
  dump_buf("  3. ecdh client generate secret:", (unsigned char *)buf,
           mbedtls_mpi_size(&cli_secret));

  // 服务器计算共享密钥
  ret = mbedtls_ecdh_compute_shared(&grp, &srv_secret, &cli_pub, &srv_pri,
                                    mbedtls_ctr_drbg_random, &ctr_drbg);
  assert_exit(ret == 0, ret);
  mbedtls_mpi_write_binary(&srv_secret, (unsigned char *)buf,
                           mbedtls_mpi_size(&srv_secret));
  dump_buf("  4. ecdh server generate secret:", (unsigned char *)buf,
           mbedtls_mpi_size(&srv_secret));

  // 比较计算出的共享密钥
  ret = mbedtls_mpi_cmp_mpi(&cli_secret, &srv_secret);
  assert_exit(ret == 0, ret);
  mbedtls_printf("  5. ecdh checking secrets ... ok\n");

cleanup:
  mbedtls_mpi_free(&cli_pri);
  mbedtls_mpi_free(&srv_pri);
  mbedtls_mpi_free(&cli_secret);
  mbedtls_mpi_free(&srv_secret);
  mbedtls_ecp_group_free(&grp);
  mbedtls_ecp_point_free(&cli_pub);
  mbedtls_ecp_point_free(&srv_pub);
  mbedtls_entropy_free(&entropy);
  mbedtls_ctr_drbg_free(&ctr_drbg);

  return 0;
}
