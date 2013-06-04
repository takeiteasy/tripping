#ifndef __RANDOM_H
#define __RANDOM_H
#include <stdlib.h>

char* to_utf8 (unsigned short c);

typedef struct {
	unsigned short min, max;
} range_t;

#endif

