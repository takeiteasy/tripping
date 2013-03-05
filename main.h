#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <string.h>

typedef enum {
	M_TEST,
	M_GEN,
	M_MINE,
	M_SINGLE
} modes_e;

void print_help(void);
void signal_handler(int);
long get_time(void);

static volatile sig_atomic_t exit_loops = 0;

