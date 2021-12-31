#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Mbed TLS
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls/entropy.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/ssl.h>

// Root CA cart pem
#define ca_cert_pem                                                            \
  "-----BEGIN CERTIFICATE-----\r\n"                                            \
  "MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\r\n"       \
  "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\r\n"       \
  "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\r\n"       \
  "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\r\n"       \
  "MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\r\n"       \
  "BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\r\n"       \
  "aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\r\n"       \
  "dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\r\n"       \
  "AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\r\n"       \
  "3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\r\n"       \
  "tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\r\n"       \
  "Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\r\n"       \
  "VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\r\n"       \
  "79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\r\n"       \
  "c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\r\n"       \
  "Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\r\n"       \
  "c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\r\n"       \
  "UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\r\n"       \
  "Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\r\n"       \
  "BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\r\n"       \
  "A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\r\n"       \
  "Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\r\n"       \
  "VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\r\n"       \
  "ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\r\n"       \
  "8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\r\n"       \
  "iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\r\n"       \
  "Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\r\n"       \
  "XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\r\n"       \
  "qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\r\n"       \
  "VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\r\n"       \
  "L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\r\n"       \
  "jjxDah2nGN59PRbxYvnKkKj9\r\n"                                               \
  "-----END CERTIFICATE-----\r\n"

#define ca_mid_cert_pem                                                        \
  "-----BEGIN CERTIFICATE-----\r\n"                                            \
  "MIIG1TCCBL2gAwIBAgIQbFWr29AHksedBwzYEZ7WvzANBgkqhkiG9w0BAQwFADCB\r\n"       \
  "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\r\n"       \
  "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\r\n"       \
  "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMjAw\r\n"       \
  "MTMwMDAwMDAwWhcNMzAwMTI5MjM1OTU5WjBLMQswCQYDVQQGEwJBVDEQMA4GA1UE\r\n"       \
  "ChMHWmVyb1NTTDEqMCgGA1UEAxMhWmVyb1NTTCBSU0EgRG9tYWluIFNlY3VyZSBT\r\n"       \
  "aXRlIENBMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAhmlzfqO1Mdgj\r\n"       \
  "4W3dpBPTVBX1AuvcAyG1fl0dUnw/MeueCWzRWTheZ35LVo91kLI3DDVaZKW+TBAs\r\n"       \
  "JBjEbYmMwcWSTWYCg5334SF0+ctDAsFxsX+rTDh9kSrG/4mp6OShubLaEIUJiZo4\r\n"       \
  "t873TuSd0Wj5DWt3DtpAG8T35l/v+xrN8ub8PSSoX5Vkgw+jWf4KQtNvUFLDq8mF\r\n"       \
  "WhUnPL6jHAADXpvs4lTNYwOtx9yQtbpxwSt7QJY1+ICrmRJB6BuKRt/jfDJF9Jsc\r\n"       \
  "RQVlHIxQdKAJl7oaVnXgDkqtk2qddd3kCDXd74gv813G91z7CjsGyJ93oJIlNS3U\r\n"       \
  "gFbD6V54JMgZ3rSmotYbz98oZxX7MKbtCm1aJ/q+hTv2YK1yMxrnfcieKmOYBbFD\r\n"       \
  "hnW5O6RMA703dBK92j6XRN2EttLkQuujZgy+jXRKtaWMIlkNkWJmOiHmErQngHvt\r\n"       \
  "iNkIcjJumq1ddFX4iaTI40a6zgvIBtxFeDs2RfcaH73er7ctNUUqgQT5rFgJhMmF\r\n"       \
  "x76rQgB5OZUkodb5k2ex7P+Gu4J86bS15094UuYcV09hVeknmTh5Ex9CBKipLS2W\r\n"       \
  "2wKBakf+aVYnNCU6S0nASqt2xrZpGC1v7v6DhuepyyJtn3qSV2PoBiU5Sql+aARp\r\n"       \
  "wUibQMGm44gjyNDqDlVp+ShLQlUH9x8CAwEAAaOCAXUwggFxMB8GA1UdIwQYMBaA\r\n"       \
  "FFN5v1qqK0rPVIDh2JvAnfKyA2bLMB0GA1UdDgQWBBTI2XhootkZaNU9ct5fCj7c\r\n"       \
  "tYaGpjAOBgNVHQ8BAf8EBAMCAYYwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHSUE\r\n"       \
  "FjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwIgYDVR0gBBswGTANBgsrBgEEAbIxAQIC\r\n"       \
  "TjAIBgZngQwBAgEwUAYDVR0fBEkwRzBFoEOgQYY/aHR0cDovL2NybC51c2VydHJ1\r\n"       \
  "c3QuY29tL1VTRVJUcnVzdFJTQUNlcnRpZmljYXRpb25BdXRob3JpdHkuY3JsMHYG\r\n"       \
  "CCsGAQUFBwEBBGowaDA/BggrBgEFBQcwAoYzaHR0cDovL2NydC51c2VydHJ1c3Qu\r\n"       \
  "Y29tL1VTRVJUcnVzdFJTQUFkZFRydXN0Q0EuY3J0MCUGCCsGAQUFBzABhhlodHRw\r\n"       \
  "Oi8vb2NzcC51c2VydHJ1c3QuY29tMA0GCSqGSIb3DQEBDAUAA4ICAQAVDwoIzQDV\r\n"       \
  "ercT0eYqZjBNJ8VNWwVFlQOtZERqn5iWnEVaLZZdzxlbvz2Fx0ExUNuUEgYkIVM4\r\n"       \
  "YocKkCQ7hO5noicoq/DrEYH5IuNcuW1I8JJZ9DLuB1fYvIHlZ2JG46iNbVKA3ygA\r\n"       \
  "Ez86RvDQlt2C494qqPVItRjrz9YlJEGT0DrttyApq0YLFDzf+Z1pkMhh7c+7fXeJ\r\n"       \
  "qmIhfJpduKc8HEQkYQQShen426S3H0JrIAbKcBCiyYFuOhfyvuwVCFDfFvrjADjd\r\n"       \
  "4jX1uQXd161IyFRbm89s2Oj5oU1wDYz5sx+hoCuh6lSs+/uPuWomIq3y1GDFNafW\r\n"       \
  "+LsHBU16lQo5Q2yh25laQsKRgyPmMpHJ98edm6y2sHUabASmRHxvGiuwwE25aDU0\r\n"       \
  "2SAeepyImJ2CzB80YG7WxlynHqNhpE7xfC7PzQlLgmfEHdU+tHFeQazRQnrFkW2W\r\n"       \
  "kqRGIq7cKRnyypvjPMkjeiV9lRdAM9fSJvsB3svUuu1coIG1xxI1yegoGM4r5QP4\r\n"       \
  "RGIVvYaiI76C0djoSbQ/dkIUUXQuB8AL5jyH34g3BZaaXyvpmnV4ilppMXVAnAYG\r\n"       \
  "ON51WhJ6W0xNdNJwzYASZYH+tmCWI+N60Gv2NNMGHwMZ7e9bXgzUCZH5FaBFDGR5\r\n"       \
  "S9VWqHB73Q+OyIVvIbKYcSc2w/aSuFKGSA==\r\n"                                   \
  "-----END CERTIFICATE-----\r\n"

#define ca_self_cert_pem                                                       \
  "-----BEGIN CERTIFICATE-----\r\n"                                            \
  "MIIGZzCCBE+gAwIBAgIQUKfaG3JicyCH7FxlFusZgzANBgkqhkiG9w0BAQwFADBL\r\n"       \
  "MQswCQYDVQQGEwJBVDEQMA4GA1UEChMHWmVyb1NTTDEqMCgGA1UEAxMhWmVyb1NT\r\n"       \
  "TCBSU0EgRG9tYWluIFNlY3VyZSBTaXRlIENBMB4XDTIxMDcxMTAwMDAwMFoXDTIx\r\n"       \
  "MTAwOTIzNTk1OVowFzEVMBMGA1UEAxMMaWZ1Y2tnZncuY29tMIIBIjANBgkqhkiG\r\n"       \
  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA76XprrnGtp4pGjHvRA8WEHcmM/9EDjuoJS94\r\n"       \
  "EWSBQTT8Hs1iy5lbFuxE/1ye4B0+kzqY8CWLVLh9vAK9E1pKZVxgnd97S6l/RLAK\r\n"       \
  "5xiT4j+BQYagGRUhEwh2lBCXlmYhFAmZkb0Ko3aNN0AUC+UJ/MOQCZp06MFYIi1g\r\n"       \
  "kklSroK4VpNtiUQODLLQZkmEAWbJCq3ePy1cx3nIGaudXI93oFarGy2Dgm33lHsg\r\n"       \
  "7+3YATwA+4HIdl9MBh5JNepIbqJpq9LhIg4ntZR0cVCGUvMAd7jbxZ78X84VXiut\r\n"       \
  "2qGvqpqWru8JxrtIK3scpkkUj+FKc931w9VTItKTNjf/lnypsQIDAQABo4ICeTCC\r\n"       \
  "AnUwHwYDVR0jBBgwFoAUyNl4aKLZGWjVPXLeXwo+3LWGhqYwHQYDVR0OBBYEFP7H\r\n"       \
  "O5/kG6ILTe8CZDb+bp8+9F2dMA4GA1UdDwEB/wQEAwIFoDAMBgNVHRMBAf8EAjAA\r\n"       \
  "MB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjBJBgNVHSAEQjBAMDQGCysG\r\n"       \
  "AQQBsjEBAgJOMCUwIwYIKwYBBQUHAgEWF2h0dHBzOi8vc2VjdGlnby5jb20vQ1BT\r\n"       \
  "MAgGBmeBDAECATCBiAYIKwYBBQUHAQEEfDB6MEsGCCsGAQUFBzAChj9odHRwOi8v\r\n"       \
  "emVyb3NzbC5jcnQuc2VjdGlnby5jb20vWmVyb1NTTFJTQURvbWFpblNlY3VyZVNp\r\n"       \
  "dGVDQS5jcnQwKwYIKwYBBQUHMAGGH2h0dHA6Ly96ZXJvc3NsLm9jc3Auc2VjdGln\r\n"       \
  "by5jb20wggEFBgorBgEEAdZ5AgQCBIH2BIHzAPEAdgB9PvL4j/+IVWgkwsDKnlKJ\r\n"       \
  "eSvFDngJfy5ql2iZfiLw1wAAAXqUyBdjAAAEAwBHMEUCIQC8UONPAPGUiPd3Grb2\r\n"       \
  "HeThcTlGxcRjZRMyb5Y5EMRKHgIgeKNLKbwVwSM2MRpTIkWF7cK0yftn+BSpRlQl\r\n"       \
  "acL3dz0AdwBElGUusO7Or8RAB9io/ijA2uaCvtjLMbU/0zOWtbaBqAAAAXqUyBd9\r\n"       \
  "AAAEAwBIMEYCIQC5L6Dbp3MXuJvoPKxsdF82/dFPJwtYZ35KxDLlA2oIDAIhANAR\r\n"       \
  "qPbGgkwJDrKbAWICj2OyHifAUri5xTA90vqxKXh3MBcGA1UdEQQQMA6CDGlmdWNr\r\n"       \
  "Z2Z3LmNvbTANBgkqhkiG9w0BAQwFAAOCAgEATlCFYtAE4+rDfSnIfZxAS/5iB5Qz\r\n"       \
  "cRUEQQKEt5lL+se3bmOu+OUI/q2amguJnDzulkMDzTDFRJSgSbJB08MtzV89us1z\r\n"       \
  "Pthk6sjpcpLagjP8QzEOKa5pHe4oUFQvixAWIqONv6NOQXn7M0fPnxY5EaIxMb58\r\n"       \
  "27AJDaaFnnptZGIMiTGNI/qYrCwXOhDkZ2ga5Lu7grO1d/gYfxAgUxYavIZfvH44\r\n"       \
  "Vr2uRU50iJpKK/D8X9X08bzhSvduOujCC0Get+OQfv1j3Y/M4NfXZuxKLePw1SQ+\r\n"       \
  "g28mCVfQQKSr4P2LcxZF3v0GDKxp6r2/anqUu9sh5fSvpy8DSLLybTfDi+3xblZ2\r\n"       \
  "rcp5kbXQ8xV6u+fgNgbk/luRMYegSgSZp7/vJSc4Sm3/xBazg9rqRhBxVjqdvctd\r\n"       \
  "vfwSmiaGG88z8NtYok6X7QwfOhTaF5tWaGZC9G43iweOLfYIGGVTX2GOJWTxk2DA\r\n"       \
  "nk5f/AB9ZpvpZ271t+hQm56QzC91tlEXM1+hGsvMSJ0j+jUlOOqOtevhpT1wZN9H\r\n"       \
  "zfjFcYvmcbkBUEgtvjcv8ZPEWumwsN6tw0bGW5pymSZoAL4BWYtjXwkxfpryaYN5\r\n"       \
  "sVVkYsLRD7K9BO+/ur0TaOv+QCduvzSK3TWQ7AJpLsI/X4Npsr65Z0zYJ5pbe2g7\r\n"       \
  "rryetJA1LWxEnOs=\r\n"                                                       \
  "-----END CERTIFICATE-----\r\n"

#define SERVER_PORT "7788"
#define SERVER_NAME "ifuckgfw.com"
#define GET_REQUEST "GET / HTTP/1.0\r\n\r\n"

static void my_debug(void *ctx, int level, const char *file, int line,
                     const char *str) {
  ((void)level);

  fprintf((FILE *)ctx, "%s:%04d: %s", file, line, str);
  fflush((FILE *)ctx);
}

int main(void) {

  int ret, len;
  unsigned char buf[1024];

  /*
   * creation and initialization of the Mbed TLS structures
   */

  mbedtls_net_context server_fd;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;

  mbedtls_x509_crt cacert;

  const char *pers = "ssl_client1";

  mbedtls_net_init(&server_fd);
  mbedtls_ssl_init(&ssl);
  mbedtls_ssl_config_init(&conf);
  mbedtls_x509_crt_init(&cacert);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  mbedtls_entropy_init(&entropy);
  if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                   (const unsigned char *)pers,
                                   strlen(pers))) != 0) {
    printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    goto exit;
  }

  /*
   * Configuring SSL/TLS
   */

  // Set the endpoint and transport type and security parameters.
  if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                         MBEDTLS_SSL_TRANSPORT_STREAM,
                                         MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
    printf(" failed\n ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
    goto exit;
  }

  // Set the random engine and debug function.
  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
  mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

  // Parse trusted cert
  // ca_cert_pem is OK
  // ca_mid_cert_pem is OK
  // ca_self_cert_pem NOT OK
  // https://github.com/ARMmbed/mbedtls/issues/139#issuecomment-270134402
  mbedtls_x509_crt_parse(&cacert, (unsigned char *)ca_cert_pem,
                         sizeof(ca_cert_pem));

  // Set the authentication mode.
  mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);

  // Add cert in chain
  // Similar to the concept of installing a trusted certificate in a browser
  mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);

  // Set up the SSL context to use it.
  if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
    printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
    goto exit;
  }
  if ((ret = mbedtls_ssl_set_hostname(&ssl, SERVER_NAME)) != 0) {
    printf(" failed\n ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
    goto exit;
  }

  // Set Input and output functions it needs to use for sending out network
  // traffic.
  mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv,
                      NULL);

  /*
   * Start the connection with mbedtls
   */
  printf("\n  . Connecting to tcp/%s/%4s...", SERVER_NAME, SERVER_PORT);
  fflush(stdout);

  if ((ret = mbedtls_net_connect(&server_fd, SERVER_NAME, SERVER_PORT,
                                 MBEDTLS_NET_PROTO_TCP)) != 0) {
    printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
    goto exit;
  }

  /*
   * Handshake
   */
  mbedtls_printf("  . Performing the SSL/TLS handshake...");
  fflush(stdout);

  while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
      mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n",
                     (unsigned int)-ret);
      goto exit;
    }
  }

  mbedtls_printf(" ok\n");

  /*
   * Reading and writing data
   */

  /*
   * Write the GET request
   */
  printf("  > Write to server:");
  fflush(stdout);

  len = sprintf((char *)buf, GET_REQUEST);

  while ((ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0) {
    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
      printf(" failed\n  ! write returned %d\n\n", ret);
      goto exit;
    }
  }

  len = ret;
  printf(" %d bytes written\n\n%s", len, (char *)buf);

  /*
   * Read the HTTP response
   */
  printf("  < Read from server:");
  fflush(stdout);

  do {
    len = sizeof(buf) - 1;
    memset(buf, 0, sizeof(buf));
    ret = mbedtls_ssl_read(&ssl, buf, len);

    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
      continue;

    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
      break;

    if (ret < 0) {
      printf("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
      break;
    }

    if (ret == 0) {
      printf("\n\nEOF\n\n");
      break;
    }

    len = ret;
    printf(" %d bytes read\n\n%s", len, (char *)buf);

  } while (1);

exit:

  /*
   * Teardown
   */
  mbedtls_net_free(&server_fd);

  mbedtls_x509_crt_free(&cacert);
  mbedtls_ssl_free(&ssl);
  mbedtls_ssl_config_free(&conf);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);

  return (ret);
}