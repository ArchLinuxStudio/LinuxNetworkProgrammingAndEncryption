cmake_minimum_required(VERSION 3.20.5)
# 定义工程名称
project("mbedtls-hmac")

# 定义依赖动态链接库
set(libs 
    mbedtls mbedcrypto mbedx509)

## 明确src文件
set(src
    ${CMAKE_CURRENT_SOURCE_DIR}/hmac_old.c)

# 生成的可执行文件叫hmac_exec
add_executable(hmac_exec ${src})

# 需要链接的库
target_link_libraries(hmac_exec ${libs})