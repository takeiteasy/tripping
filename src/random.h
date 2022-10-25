#ifndef __RANDOM_H
#define __RANDOM_H
#include <stdlib.h>
#include <string.h>

#define RAND_RANGE(x, y) (rand() % (y - x + 1) + x)
#define RAND_ASCII RAND_RANGE(33, 126)

char *to_utf8(unsigned long);
char *rndstr_ascii(unsigned short);
size_t rndstr_sjis(unsigned short, char *);

typedef struct {
    unsigned short min, max;
} range_t;

#endif
