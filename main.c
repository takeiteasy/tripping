/*
 * main.c - xtrip
 * Created by Rusty Shackleford on 22/12/2012.
 * Copyright (c) 2012, Rusty Shackleford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. All advertising materials mentioning features or use of this software
 *     must display the following acknowledgement:
 *     This product includes software developed by the <organization>.
 *  4. Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY RUSTY SHACKLEFORD ''AS IS'' AND AN
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL RUSTY SHACKLEFORD BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "main.h"

int main(int argc, const char* argv[]) {
	srand((unsigned int)(time(NULL)));

	bool  search_mode   = false,
	      ignore_case   = false,
	      test_mode     = false,
	      gen_mode      = false,
	      non_stop_gen  = false,
	      benchmark     = false,
	      verboose      = false;
	int   total_gen     = 0,
	      total_threads = 1,
	      min_rnd_len   = DEF_MIN_RND_LEN,
	      max_rnd_len   = DEF_MAX_RND_LEN;
	char* arg_val       = NULL;

	if (argc == 1) {
		gen_mode  = true;
		total_gen = 1;
	}
	else {
		for (int i = 1; i < argc; ++i) {
			if (argv[i][0] == '-') {
				if (argv[i][1] == '-') { /* longer argument */
					if (strcmp_fast(argv[i], "--benchmark"))
						benchmark = true;
					else if (strcmp_fast(argv[i], "--verboose")) {
						verboose  = true;
						benchmark = true;
					}
					else if (strcmp_fast(argv[i], "--generate")) {
						test_mode    = false;
						gen_mode     = true;
						search_mode  = false;
						non_stop_gen = true;
					}
					else { /* argument with a value */
						int equals_loc = str_contains_char(argv[i], '=');
						if (equals_loc != -1) { /* Confirm it */
							/* Get string before first '=' */
							size_t cut_arg_len = (size_t)(equals_loc - 2);
							char* t_arg = (char*)malloc(cut_arg_len);
							memcpy(t_arg, argv[i] + 2, cut_arg_len);

							/* Get everything else */
							cut_arg_len += 3;
							size_t cut_val_len = strlen(argv[i]) - cut_arg_len;
							char* t_val = (char*)malloc(cut_val_len);
							memcpy(t_val, argv[i] + cut_arg_len, cut_val_len);

							if (strcmp_fast(t_arg, "test")) {
								test_mode   = true;
								gen_mode    = false;
								search_mode = false;
								
								arg_val = (char*)malloc(cut_val_len);
								memcpy(arg_val, t_val, cut_val_len);
							}
							else if (strcmp_fast(t_arg, "generate")) {
								test_mode    = false;
								gen_mode     = true;
								non_stop_gen = false;
								search_mode  = false;

								total_gen = atoi(t_val);
							}
							else if (strcmp_fast(t_arg, "search")) {
								test_mode   = false;
								gen_mode    = false;
								search_mode = true;
								ignore_case = true;
								
								arg_val = (char*)malloc(cut_val_len);
								memcpy(arg_val, t_val, cut_val_len);
							}
							else if (strcmp_fast(t_arg, "Search")) {
								test_mode   = false;
								gen_mode    = false;
								search_mode = true;
								ignore_case = true;
								
								arg_val = (char*)malloc(cut_val_len);
								memcpy(arg_val, t_val, cut_val_len);
							}
							else if (strcmp_fast(t_arg, "threads")) {
								total_threads = atoi(t_val);
								if (total_threads < 1)
									total_threads = 1;
							}
							else if (strcmp_fast(t_arg, "min-len")) {
								min_rnd_len = atoi(t_val);
								if (min_rnd_len < 1)
									min_rnd_len = DEF_MIN_RND_LEN;
							}
							else if (strcmp_fast(t_arg, "max-len")) {
								max_rnd_len = atoi(t_val);
								if (max_rnd_len <= 1)
									max_rnd_len = DEF_MAX_RND_LEN; 
							}

							free(t_arg);
							free(t_val);
						}
					}
				}
				else { /* single character argument */
					if (strlen(argv[i]) == 2) { /* Confirm it */
						if (argv[i][1] == 'b')
							benchmark = true;
						else if (argv[i][1] == 'v') {
							verboose  = true;
							benchmark = true;
						}
						else if (argv[i][1] == 'g') {
							test_mode    = false;
							gen_mode     = true;
							non_stop_gen = true;
							search_mode  = false;
						}
					}
				}
			}
		}
	}

	if (test_mode && strlen(arg_val) <= 0) {
		printf("ERROR! No value passed to test!\n");
		print_usage();
		return EXIT_FAILURE;
	}

	if (search_mode) {
		size_t arg_val_len = strlen(arg_val);
		if (arg_val_len <= 0) {
			printf("ERROR! No value passed to search!\n");
			print_usage();
			return EXIT_FAILURE;
		}
		else if (arg_val_len > 11) {
			printf("ERROR! Search value is longer than 11\n");
			print_usage();
			return EXIT_FAILURE;
		}
		else {
			if (ignore_case)
				str_contains = str_contains_ignore_case;
			else
				str_contains = str_contains_normal;
		}
	}

	if (gen_mode && total_gen == 0)
		non_stop_gen = true;

	if (!search_mode && !test_mode && !gen_mode) { /* No mode argument passed  */
		gen_mode  = true;
		total_gen = 1;
	}

	if (gen_mode && (total_threads > total_gen))
		total_threads = 1;

	if (max_rnd_len <= min_rnd_len) {
		min_rnd_len = DEF_MIN_RND_LEN;
		max_rnd_len = DEF_MAX_RND_LEN;
	}

	/* Only time the generating, not the argument parsing */
	long start_time = get_time();
	if (test_mode) {
		char* test_trip = gen_trip(arg_val, strlen(arg_val));
		printf("%s => %s\n", arg_val, test_trip);
		free(arg_val);
		free(test_trip);
	}
	else if (gen_mode) {
		if (non_stop_gen) {
		
		}
		else {
			if (total_threads == 1) {
				thread_arg t_arg = { total_gen, min_rnd_len, max_rnd_len };
				gen_thread((void*)&t_arg);
			}
			else {
				/* Determine how many trips each thread will generate */
				int trips_per_thread = total_gen / total_threads;
				int first_thread_total = trips_per_thread;
				if ((trips_per_thread * total_threads) != total_gen) /* Account for remainder */
					first_thread_total += (total_gen - (trips_per_thread * total_threads)); /* Add remainder to first thread total */

				pthread_t threads[total_threads];
				thread_arg thread_arg;
				int thread_ret = -1;
				
				for (int i = 0; i < total_threads; ++i) {
					thread_arg.total = (i == 0 ? first_thread_total : trips_per_thread);
					thread_arg.min   = min_rnd_len;
					thread_arg.max   = max_rnd_len;

					thread_ret = pthread_create(&threads[i], NULL, gen_thread, (void*)&thread_arg);
					if (thread_ret != 0) {
						printf("ERROR! FAILED TO CREATE THREADS!\n");
						return EXIT_FAILURE;
					}
				}
				for (int i = 0; i < total_threads; ++i) {
					thread_ret = pthread_join(threads[i], NULL);
					if (thread_ret != 0) {
						printf("ERROR! FAILED TO JOIN THREADS!\n");
						return EXIT_FAILURE;
					}
				}
			}
			
		}
	}
	else if (search_mode) {
	
	}
		
	if (benchmark) {
		float exec_time = ((get_time() - start_time) / 1000000.f);
		printf("\nexec time:   %f\ntotal trips: %d\ntrips p/s:   %f\n",
			exec_time, total_gen, (total_gen / exec_time));
	}
	return EXIT_SUCCESS;
}

void print_usage() {
	printf("help!\n");
}

long get_time() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((long)(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000);
}

char* gen_trip(const char* src, size_t src_len) {
	char* src_new = (char*)malloc((src_len + 2) * 5); /* Create safe buffer for htmlreplacechars */

	/* PHP-like htmlreplacechars */
	size_t nlen = src_len, j = 0;
	for (unsigned int i = 0; i < src_len; ++i) {
		switch (src[i]) {
			case '&':
				src_new[j]     = '&';
				src_new[j + 1] = 'a';
				src_new[j + 2] = 'm';
				src_new[j + 3] = 'p';
				src_new[j + 4] = ';';
				
				nlen += 4;
				j += 5;
				break;
			case '<':
				src_new[j]     = '&';
				src_new[j + 1] = 'l';
				src_new[j + 2] = 't';
				src_new[j + 3] = ';';
				
				nlen += 3;
				j += 4;
				break;
			case '>':
				src_new[j]     = '&';
				src_new[j + 1] = 'g';
				src_new[j + 2] = 't';
				src_new[j + 3] = ';';
				
				nlen += 3;
				j += 4;
				break;
			case '"':
				src_new[j]     = '&';
				src_new[j + 1] = 'q';
				src_new[j + 2] = 'u';
				src_new[j + 3] = 'o';
				src_new[j + 4] = 't';
				src_new[j + 5] = ';';
				
				nlen += 5;
				j += 6;
				break;
			default:
				src_new[j++] = src[i];
				break;
		}
	}

	/* Create salt based on source */
	char salt[3];
	switch (nlen) {
		case 1:
			salt[0] = 'H';
			salt[1] = '.';
			break;
		case 2:
			salt[0] = src_new[1];
			salt[1] = 'H';
			break;
		default:
			salt[0] = src_new[1];
			salt[1] = src_new[2];
			break;
	}
	salt[2] = '\0';
	
	/* Replace chars */
	for (int i = 0; i < 2; ++i) {
		if (salt[i] < '.' || salt[i] > 'z')
			salt[i] = '.';
		else if (salt[i] >= ':' && salt[i] <= '@')
			salt[i] = (char)(salt[i] + ('A' - ':'));
		else if (salt[i] >= '[' && salt[i] <= '`')
			salt[i] = (char)(salt[i] + ('a' - '['));
	}

	/* DES encrypt */
	char* des_dst = (char*)malloc(14);
	DES_fcrypt(src_new, salt, des_dst);
	free(src_new);

	/* Return tripcode - last 11 characters */
	char* ret = (char*)malloc(11);
	memcpy(ret, des_dst + 3, 11);
	free(des_dst);
	return ret;
}

void gen_rand_str(char* dst, size_t len) {
	for (size_t i = 0; i < len; ++i)
		dst[i] = char_pool[((unsigned)rand() % CHAR_POOL_LEN)];
	dst[len] = '\0';
}

bool strcmp_fast(const char* a, const char* b) {
	size_t len = strlen(a);
	if (strlen(b) != len)
		return false;

	for (size_t i = 0; i < len; ++i)
		if (a[i] != b[i])
			return false;
	return true;
}

void str_to_lower(char* dst, const char* src, size_t src_len) {
	for (size_t i = 0; i < src_len; ++i)
		dst[i] = (char)tolower(src[i]);
	dst[src_len] = '\0';
}

int str_contains_char(const char* hay, char needle) {
	for (int i = 0; i < (int)strlen(hay); ++i)
		if (hay[i] == needle)
			return i;
	return -1;
}

bool str_contains_normal(const char* a, const char* b) {
	size_t hay_len    = strlen(a),
	       needle_len = strlen(b);
	if (needle_len > hay_len)
		return false;

	bool matches = true;
	for (size_t i = 0; i < hay_len; ++i) {
		if (a[i] == b[0]) {
			for (size_t j = 1; j < needle_len; ++j)
				matches = (a[i + j] != b[j]);
			if (matches)
				return true;
		}
	}
	return false;
}

bool str_contains_ignore_case(const char* a, const char* b) {
	size_t hay_len    = strlen(a),
	       needle_len = strlen(b);
	if (needle_len > hay_len)
		return false;
	
	char* a_lower = (char*)malloc(hay_len);
	str_to_lower(a_lower, a, hay_len);
	char* b_lower = (char*)malloc(needle_len);
	str_to_lower(b_lower, b, needle_len);

	bool ret = str_contains_normal(a_lower, b_lower);
	free(a_lower);
	free(b_lower);
	return ret;
}

void* gen_thread(void* arg) {
	thread_arg t_arg = *((thread_arg*)arg);
	size_t str_len  = 0;
	for (int i = 0; i < t_arg.total; ++i) {
		str_len = (size_t)RAND_STR_LEN(t_arg.min, t_arg.max);
		char* str = (char*)malloc(str_len);
		gen_rand_str(str, str_len);
		char* trip = gen_trip(str, str_len);
		printf("%s => %s\n", str, trip);
		free(str);
		free(trip);
	}
	return NULL;
}

