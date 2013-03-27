#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

size_t conv_sjis(iconv_t, char*, size_t, char*, size_t);
size_t htmlspecialchars(const char*, size_t, char*);
char*  gen_trip_sjis(iconv_t, char*, size_t);
char*  gen_trip_no_sjis(char*, size_t);

