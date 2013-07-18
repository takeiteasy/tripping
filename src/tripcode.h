#ifndef __TRIPCODE_H
#define __TRIPCODE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/des.h>
#include <iconv.h>

size_t conv_sjis (iconv_t, char*, size_t, char*, size_t);
char*  gen_trip_sjis (iconv_t, const char*, size_t);
char* make_trip (const char*, size_t);
#define gen_trip_ascii(a, b) make_trip(a, b)

#endif

