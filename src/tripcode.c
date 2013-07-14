#include "tripcode.h"

static char salt_table [] =
    "................................"
    ".............../0123456789ABCDEF"
    "GABCDEFGHIJKLMNOPQRSTUVWXYZabcde"
    "fabcdefghijklmnopqrstuvwxyz....."
    "................................"
    "................................"
    "................................"
    "................................";

size_t conv_sjis (iconv_t cd, char* src, size_t src_len, char* dst, size_t dst_len) {
    size_t d_len = dst_len;
    iconv(cd, &src, &src_len, &dst, &d_len);
    return dst_len - d_len;
}

char* gen_trip_sjis (iconv_t cd, char* src, size_t src_len) {
    size_t sjis_len = (src_len + 2) * 5;
    char* sjis_ret = malloc(sjis_len);
    src_len = conv_sjis(cd, src, src_len, sjis_ret, sjis_len);
    char* ret = make_trip(sjis_ret, src_len);
    free(sjis_ret);
    return ret;
}

char* make_trip (const char* str, size_t str_len) {
    char* dst = malloc((str_len + 2) * 5);
    size_t j = 0;
    for (size_t i = 0; i < str_len; ++i) {
        switch (str[i]) {
            case '&':
                dst[j]   = '&';
                dst[++j] = 'a';
                dst[++j] = 'm';
                dst[++j] = 'p';
                dst[++j] = ';';
                break;
            case '<':
                dst[j]   = '&';
                dst[++j] = 'l';
                dst[++j] = 't';
                dst[++j] = ';';
                break;
            case '>':
                dst[j]   = '&';
                dst[++j] = 'g';
                dst[++j] = 't';
                dst[++j] = ';';
                break;
            case '"':
                dst[j]   = '&';
                dst[++j] = 'q';
                dst[++j] = 'u';
                dst[++j] = 'o';
                dst[++j] = 't';
                dst[++j] = ';';
                break;
            default:
                dst[j] = str[i];
                ++j;
                break;
        }
    }

    /* Create salt based on source */
    char salt[] = "HH";
    switch (j) {
        case 1:
            salt[1] = '.';
            break;
        default:
            salt[1] = salt_table[(unsigned char)dst[2]];
        case 2:
            salt[0] = salt_table[(unsigned char)dst[1]];
            break;
    }

    /* DES encrypt */
    char* des_dst = malloc(14);
    DES_fcrypt(dst, salt, des_dst);
    free(dst);

    /* Return tripcode - first 11 characters */
    char* ret = malloc(11);
    memcpy(ret, des_dst + 3, 11);
    free(des_dst);
    return ret;
}

