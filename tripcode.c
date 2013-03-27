#include "tripcode.h"

size_t conv_sjis(iconv_t cd, char* src, size_t src_len, char* dst, size_t dst_len) {
	size_t d_len = dst_len;
	size_t ret = iconv(cd, &src, &src_len, &dst, &d_len);
	return dst_len - d_len;
}

size_t htmlspecialchars(const char* str, size_t str_len, char* dst) {
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
	return j;
}

char* gen_trip_sjis(iconv_t cd, char* src, size_t src_len) {
	char* sjis_ret = (char*)malloc(32 * sizeof(char));
	printf("%s %d ", src, src_len);
	src_len = conv_sjis(cd, src, src_len, sjis_ret, 32);
	printf("%s %d\n", sjis_ret, src_len);
	return sjis_ret;
}

char* gen_trip_no_sjis(char* src, size_t src_len) {
	return NULL;
}

