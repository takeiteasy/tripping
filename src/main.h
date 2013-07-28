#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

#include "tripcode.h"
#include "random.h"
#include "../lib/threads/threads.h"

#include <pcre.h>
#define MVEC_LEN 14

#if ((__APPLE__ & __MACH__) || macintosh || Macintosh)
#   define PLAT_OSX
#   include <mach/mach_time.h>
#elif (__WIN32__ || _WIN32 || _WIN64 || __WINDOWS__)
#   define PLAT_WIN
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#else
#   define PLAT_NIX
#endif

#if defined PLAT_WIN
#   define SLEEP(x) Sleep((x) / 1000)
#else
#   include <unistd.h>
#   define SLEEP(x) sleep((x))
#endif

typedef enum {
    M_TEST,
    M_GEN,
    M_MINE,
    M_SINGLE,
    M_BENCH,
    M_NOMODE
} modes_e;

/* General functions */
void print_help (const char*);
void signal_handler (int);
long get_time (void);
void u_sleep (unsigned int);

/* Mode functions */
void single_mode (unsigned int, unsigned int, bool);
void test_mode (void);

void gen_mode_ascii (unsigned int, unsigned int, unsigned int);
void gen_mode_sjis (unsigned int, unsigned int, unsigned int);

typedef struct {
    unsigned int min, max;
    mtx_t* mtx;
} nstop_gen_arg;

void* nstop_gen_mode_ascii (void*);
void* nstop_gen_mode_sjis (void*);

typedef struct {
    unsigned int min, max, target;
    mtx_t* mtx;
} bench_arg;

void bench_mode_ascii (void*);
void bench_mode_sjis (void*);

void* nstop_bench_mode_ascii (void*);
void* nstop_bench_mode_sjis (void*);

typedef struct {
    unsigned int min, max;
    const char* search;
    size_t search_len;
    bool caseless;
    mtx_t* mtx;
} mine_arg;

void str_to_lower (char*, const char*, size_t);
bool str_contains (const char*, size_t, const char*, size_t);

void* mine_mode_ascii (void*);
void* mine_mode_ascii_regexp (void*);

void* mine_mode_sjis (void*);
void* mine_mode_sjis_regex (void*);

static volatile sig_atomic_t exit_loops = 0;

