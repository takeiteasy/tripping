#if ((__APPLE__ & __MACH__) || macintosh || Macintosh)
#	define PLAT_OSX
#elif (__WIN32__ || _WIN32 || _WIN64 || __WINDOWS__)
#	define PLAT_WIN
#else
#	define PLAT_NIX
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#if defined PLAT_OSX
#	include <mach/mach_time.h> /* OSX */
#elif defined PLAT_WIN
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif
#include "tripcode.h"
#include "random.h"

typedef enum {
	M_TEST,
	M_GEN,
	M_MINE,
	M_SINGLE
} modes_e;

/* General functions */
void print_help(void);
void signal_handler(int);
long get_time(void);

/* Mode functions */
void single_mode(void);
void test_mode(void);

static volatile sig_atomic_t exit_loops = 0;

