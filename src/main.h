#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include "tripcode.h"
#include "random.h"
#include "../lib/threads/threads.h"

#if ((__APPLE__ & __MACH__) || macintosh || Macintosh)
#	define PLAT_OSX
#	include <mach/mach_time.h>
#elif (__WIN32__ || _WIN32 || _WIN64 || __WINDOWS__)
#	define PLAT_WIN
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#else
#	define PLAT_NIX
#endif

typedef enum {
	M_TEST,
	M_GEN,
	M_MINE,
	M_SINGLE,
	M_NOMODE
} modes_e;

/* General functions */
void print_help (const char*);
void signal_handler (int);
long get_time (void);

/* Mode functions */
void single_mode (void);
void test_mode (void);

static volatile sig_atomic_t exit_loops = 0;

typedef struct {
    unsigned int total, min, max;
} gen_mode_arg;

int gen_mode_sjis (void*);
int gen_mode_ascii (void*);
static int (*gen_mode)(void*);

