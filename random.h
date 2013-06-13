#ifndef __RANDOM_H
#define __RANDOM_H
#include <stdlib.h>
#include <string.h>

#define RAND_RANGE(x, y) (rand() % (y - x + 1) + x)
#define RAND_ASCII RAND_RANGE(33, 126)

char* to_utf8 (unsigned long);
char* rndstr (unsigned short);
char* rndstr_uni (unsigned short);

typedef struct {
	unsigned short min, max;
} range_t;

#endif

