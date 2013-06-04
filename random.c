#include "random.h"

char* to_utf8 (unsigned short c) {
	unsigned long uc = 0x00000000 | (c & 0xFFFF);
	char* ret        = NULL;

	if (uc <= 0x7F) {
		ret    = malloc(2);
		ret[0] = uc                % 0x80;
		ret[1] = '\0';
	} else if (uc <= 0x7FF) {
		ret    = malloc(3);
		ret[0] = 0xC0 + (uc >>  6) % 0x20;
		ret[1] = 0x80 +  uc        % 0x40;
		ret[2] = '\0';
	} else if (uc <= 0xFFFF) {
		ret    = malloc(4);
		ret[0] = 0xE0 + (uc >> 12) % 0x10;
		ret[1] = 0x80 + (uc >>  6) % 0x40;
		ret[2] = 0x80 +  uc        % 0x40;
		ret[3] = '\0';
	} else if (uc <= 0x1FFFFF) {
		ret    = malloc(5);
		ret[0] = 0xF0 + (uc >> 18) % 0x8;
		ret[1] = 0x80 + (uc >> 12) % 0x40;
		ret[2] = 0x80 + (uc >>  6) % 0x40;
		ret[3] = 0x80 +  uc        % 0x40;
		ret[4] = '\0';
	} else if (uc <= 0x3FFFFFF) {
		ret    = malloc(6);
		ret[0] = 0xF8 + (uc >> 24) % 0x4;
		ret[1] = 0x80 + (uc >> 18) % 0x40;
		ret[2] = 0x80 + (uc >> 12) % 0x40;
		ret[3] = 0x80 + (uc >>  6) % 0x40;
		ret[4] = 0x80 +  uc        % 0x40;
		ret[5] = '\0';
	} else if (uc <= 0x7FFFFFFF) {
		ret    = malloc(7);
		ret[0] = 0xFC + (uc >> 30) % 0x2;
		ret[1] = 0x80 + (uc >> 24) % 0x40;
		ret[2] = 0x80 + (uc >> 18) % 0x40;
		ret[3] = 0x80 + (uc >> 12) % 0x40;
		ret[4] = 0x80 + (uc >>  6) % 0x40;
		ret[5] = 0x80 +  uc        % 0x40;
		ret[6] = '\0';
	}
	return ret;
}

