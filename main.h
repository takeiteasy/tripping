#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include "tripcode.h"

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

