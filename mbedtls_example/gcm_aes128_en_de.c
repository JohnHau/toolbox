#if 0
#include "mbedtls/build_info.h"
#include <test/helpers.h>
#include <test/macros.h>
#include <test/random.h>
#include <test/bignum_helpers.h>
#include <test/psa_crypto_helpers.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "mbedtls/gcm.h"

typedef struct data_tag
{
	uint8_t *x;
	uint32_t len;
}data_t;


/**
 * \brief       Verifies that string is in string parameter format i.e. "<str>"
 *              It also strips enclosing '"' from the input string.
 *
 * \param str   String parameter.
 *
 * \return      0 if success else 1
 */
#if 0

int verify_string(char **str)
{
    if ((*str)[0] != '"' ||
        (*str)[strlen(*str) - 1] != '"') {
        mbedtls_fprintf(stderr,
                        "Expected string (with \"\") for parameter and got: %s\n", *str);
        return -1;
    }

    (*str)++;
    (*str)[strlen(*str) - 1] = '\0';

    return 0;
}


#endif




/**
 * \brief       Verifies that string is an integer. Also gives the converted
 *              integer value.
 *
 * \param str   Input string.
 * \param value Pointer to int for output value.
 *
 * \return      0 if success else 1
 */
#if 0



int verify_int(char *str, int32_t *value)
{
    size_t i;
    int minus = 0;
    int digits = 1;
    int hex = 0;

    for (i = 0; i < strlen(str); i++) {
        if (i == 0 && str[i] == '-') {
            minus = 1;
            continue;
        }

        if (((minus && i == 2) || (!minus && i == 1)) &&
            str[i - 1] == '0' && (str[i] == 'x' || str[i] == 'X')) {
            hex = 1;
            continue;
        }

        if (!((str[i] >= '0' && str[i] <= '9') ||
              (hex && ((str[i] >= 'a' && str[i] <= 'f') ||
                       (str[i] >= 'A' && str[i] <= 'F'))))) {
            digits = 0;
            break;
        }
    }

    if (digits) {
        if (hex) {
            *value = strtol(str, NULL, 16);
        } else {
            *value = strtol(str, NULL, 10);
        }

        return 0;
    }

    mbedtls_fprintf(stderr,
                    "Expected integer for parameter and got: %s\n", str);
    return KEY_VALUE_MAPPING_NOT_FOUND;
}



#endif

/**
 * \brief       Read a line from the passed file pointer.
 *
 * \param f     FILE pointer
 * \param buf   Pointer to memory to hold read line.
 * \param len   Length of the buf.
 *
 * \return      0 if success else -1
 */
int get_line(FILE *f, char *buf, size_t len)
{
    char *ret;
    int i = 0, str_len = 0, has_string = 0;

    /* Read until we get a valid line */
    do {
        ret = fgets(buf, len, f);
        if (ret == NULL) {
            return -1;
        }

        str_len = strlen(buf);

        /* Skip empty line and comment */
        if (str_len == 0 || buf[0] == '#') {
            continue;
        }
        has_string = 0;
        for (i = 0; i < str_len; i++) {
            char c = buf[i];
            if (c != ' ' && c != '\t' && c != '\n' &&
                c != '\v' && c != '\f' && c != '\r') {
                has_string = 1;
                break;
            }
        }
    } while (!has_string);

    /* Strip new line and carriage return */
    ret = buf + strlen(buf);
    if (ret-- > buf && *ret == '\n') {
        *ret = '\0';
    }
    if (ret-- > buf && *ret == '\r') {
        *ret = '\0';
    }

    return 0;
}

/**
 * \brief       Splits string delimited by ':'. Ignores '\:'.
 *
 * \param buf           Input string
 * \param len           Input string length
 * \param params        Out params found
 * \param params_len    Out params array len
 *
 * \return      Count of strings found.
 */

#if 0
static int parse_arguments(char *buf, size_t len, char **params,
                           size_t params_len)
{
    size_t cnt = 0, i;
    char *cur = buf;
    char *p = buf, *q;

    params[cnt++] = cur;

    while (*p != '\0' && p < (buf + len)) {
        if (*p == '\\') {
            p++;
            p++;
            continue;
        }
        if (*p == ':') {
            if (p + 1 < buf + len) {
                cur = p + 1;
                TEST_HELPER_ASSERT(cnt < params_len);
                params[cnt++] = cur;
            }
            *p = '\0';
        }

        p++;
    }

    /* Replace newlines, question marks and colons in strings */
    for (i = 0; i < cnt; i++) {
        p = params[i];
        q = params[i];

        while (*p != '\0') {
            if (*p == '\\' && *(p + 1) == 'n') {
                p += 2;
                *(q++) = '\n';
            } else if (*p == '\\' && *(p + 1) == ':') {
                p += 2;
                *(q++) = ':';
            } else if (*p == '\\' && *(p + 1) == '?') {
                p += 2;
                *(q++) = '?';
            } else {
                *(q++) = *(p++);
            }
        }
        *q = '\0';
    }

    return cnt;
}

#endif



void test_gcm_encrypt_and_tag(int cipher_id, data_t *key_str,
                         data_t *src_str, data_t *iv_str,
                         data_t *add_str, data_t *dst,
                         int tag_len_bits, data_t *tag,
                         int init_result)
{
    unsigned char output[128];
    unsigned char tag_output[16];
    mbedtls_gcm_context ctx;
    size_t tag_len = tag_len_bits / 8;
    size_t n1;
    size_t n1_add;

    mbedtls_gcm_init(&ctx);

    memset(output, 0x00, 128);
    memset(tag_output, 0x00, 16);


    mbedtls_gcm_setkey(&ctx, cipher_id, key_str->x, key_str->len * 8);
    mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT, src_str->len, iv_str->x,
                                              iv_str->len, add_str->x, add_str->len, src_str->x,
                                              output, tag_len, tag_output);

    mbedtls_gcm_free(&ctx);
}




void test_gcm_decrypt_and_verify(int cipher_id, data_t *key_str,
                            data_t *src_str, data_t *iv_str,
                            data_t *add_str, int tag_len_bits,
                            data_t *tag_str, char *result,
                            data_t *pt_result, int init_result)
{
    unsigned char output[128];
    mbedtls_gcm_context ctx;
    int ret;
    size_t tag_len = tag_len_bits / 8;
    size_t n1;
    size_t n1_add;

    mbedtls_gcm_init(&ctx);

    memset(output, 0x00, 128);


    mbedtls_gcm_setkey(&ctx, cipher_id, key_str->x, key_str->len * 8);
    mbedtls_gcm_auth_decrypt(&ctx,
                                       src_str->len,
                                       iv_str->x,
                                       iv_str->len,
                                       add_str->x,
                                       add_str->len,
                                       tag_str->x,
                                       tag_len,
                                       src_str->x,
                                       output);

    mbedtls_gcm_free(&ctx);
}



int main(int argc, const char *argv[])
{




	return 0;

}
