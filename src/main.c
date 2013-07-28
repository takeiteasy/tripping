#include "main.h"

/* TODO
 *  Optimize make_trip
 *  Add comments for reference
 *  Finish print_help & update README
 */

int main (int argc, const char *argv[]) {
    srand((unsigned int)time(NULL));
    signal(SIGTERM, signal_handler);
    signal(SIGINT,  signal_handler);

    modes_e      mode       = M_SINGLE;
    int          extra_args = 2;
    unsigned int total_gen  = 0;
    char*        mine_test  = NULL;

    unsigned int total_threads  = 1,
                 min_rnd        = 2,
                 max_rnd        = 14,
                 timeout        = 0;
    bool         benchmark      = false,
                 ascii          = false,
                 mine_is_regex  = false,
                 caseless       = false;

    if (argc > 1) {
        if (strcmp(argv[1], "help") == 0) {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        }
        else if (!strcmp(argv[1], "test"))
            mode = M_TEST;
        else if (!strcmp(argv[1], "gen")) {
            total_gen = (argc > 2 ? (unsigned)atoi(argv[2]) : 0);
            if (total_gen && argc > 2)
                ++extra_args;
            mode = M_GEN;
        } else if (!strcmp(argv[1], "bench")){
            total_gen = (argc > 2 ? (unsigned)atoi(argv[2]) : 0);
            if (total_gen && argc > 2)
                ++extra_args;
            mode = M_BENCH;
            benchmark = true;
        } else if (!strcmp(argv[1], "mine")) {
            if (argc > 2) {
                size_t arg_len = strlen(argv[2]);
                char* tmp_str = malloc(arg_len);
                strncpy(tmp_str, argv[2], arg_len);
                tmp_str[arg_len] = '\0';
                mine_is_regex = (tmp_str[0] == 47 && tmp_str[arg_len - 1] == 47);

                if (mine_is_regex) {
                    char* tmp_substr = malloc(arg_len - 2);
                    memcpy(tmp_substr, tmp_str + 1, arg_len - 2);
                    free(tmp_str);
                    mine_test = tmp_substr;
                } else
                    mine_test = tmp_str;

                ++extra_args;
                mode = M_MINE;
            } else {
                printf("ERROR! Mine mode requires another argument\n\n");
                print_help(argv[0]);
                return EXIT_FAILURE;
            }
        } else {
            printf("ERROR! Invalid mode entered!\n\n");
            print_help(argv[0]);
            return EXIT_FAILURE;
        }
    }

#if defined DEBUGGING
    printf("MODE:       %d\n",   mode);
    printf("EXTRA_ARGS: %d\n",   extra_args);
    printf("TOTAL_GEN:  %d\n",   total_gen);
    printf("MINE_TEST:  %s\n",   mine_test);
    printf("MINE_REGEX: %d\n\n", mine_is_regex);
#endif

    if (argc > extra_args && mode != M_SINGLE) {
        if (mode == M_TEST) {
            iconv_t cd = iconv_open("SJIS//IGNORE", "UTF-8");
            if (argc - extra_args == 1) {
                char* out = gen_trip_sjis(cd, argv[extra_args], strlen(argv[extra_args]));
                printf("%s\n", out);
                free(out);
            } else {
                for (int i = extra_args; i < argc; ++i) {
                    char* out = gen_trip_sjis(cd, argv[i], strlen(argv[i]));
                    printf("%s => %s\n", argv[i], out);
                    free(out);
                }
            }
            iconv_close(cd);
            mode = M_NOMODE;
        } else {
            for (int i = extra_args; i < argc; ++i) {
                if (!strcmp("--benchmark", argv[i]) || !strcmp("-b", argv[i]))
                    benchmark = true;
                else if (!strcmp("--ascii", argv[i]) || !strcmp("-a", argv[i]))
                    ascii = true;
                else if (!strcmp("--caseless", argv[i]) || !strcmp("-i", argv[i]))
                    caseless = true;
                else if (!strcmp("--threads", argv[i]) || !strcmp("-th", argv[i])) {
                    if ((i + 1) > (argc - 1)) {
                        printf("WARNING! No argument passed after %s! Skipping!\n", argv[i]);
                        continue;
                    }

                    if (mode == M_GEN) {
                        printf("WARNING! Threads are disabled in generate mode!\n");
                        i += 1;
                    } else {
                        total_threads = (unsigned)atoi(argv[++i]);
                        if (total_threads <= 1) {
                            printf("WARNING! Invalid number of threads entered! Using single-threading!\n");
                            total_threads = 1;
                        }
                    }
                } else if (!strcmp("--timeout", argv[i]) || !strcmp("-to", argv[i])) {
                    if ((i + 1) > (argc - 1)) {
                        printf("WARNING! No argument passed after %s! Skipping!\n", argv[i]);
                        continue;
                    }

                    timeout      = (unsigned)atoi(argv[++i]);
                    if (timeout <= 0) {
                        printf("WARNING! Invalid timeout entered! Disabling timeout!\n");
                        timeout  = 0;
                    }
                } else if (!strcmp("--min-rnd", argv[i]) || !strcmp("-mi", argv[i])) {
                    if ((i + 1) > (argc - 1)) {
                        printf("WARNING! No argument passed after %s! Skipping!\n", argv[i]);
                        continue;
                    }
                    min_rnd = (unsigned)atoi(argv[++i]);
                } else if (!strcmp("--max-rnd", argv[i]) || !strcmp("-mx", argv[i])) {
                    if ((i + 1) > (argc - 1)) {
                        printf("WARNING! No argument passed after %s! Skipping!\n", argv[i]);
                        continue;
                    }
                    max_rnd = (unsigned)atoi(argv[++i]);
                } else
                    printf("WARNING! %s is not a valid argument!\n", argv[i]);
            }
        }
    }

    if (min_rnd > max_rnd) {
        printf("WARNING! min-rnd was greater than max-rnd! Resettings to default values (1 to 14)\n");
        min_rnd = 1;
        max_rnd = 14;
    }

    if (timeout && !(mode == M_MINE || (mode == M_GEN && total_gen == 0)))
        printf("WARNIG! Timed mode is only enabled in mining and non-stop generate mode!\n");

#if defined DEBUGGING
    printf("THREADS:      %d\n",   total_threads);
    printf("MIN_RND:      %d\n",   min_rnd);
    printf("MAX_RND:      %d\n",   max_rnd);
    printf("BENCHMARK:    %d\n",   benchmark);
    printf("ASCII_ONLY:   %d\n",   ascii);
    printf("TIMEOUT:      %d\n",   timeout);
    printf("CASELESS:     %d\n\n", caseless);
#endif

    long         start_time  = get_time();
    unsigned int total_trips = 0;
    if (mode == M_MINE) {
        if (mine_test == NULL) {
            printf("ERROR! Blank mining string passed!\n");
            return EXIT_FAILURE;
        } else if (strlen(mine_test) <= 0) {
            printf("ERROR! Blank mining string passed!\n");
            return EXIT_FAILURE;
        }

        void (*mine_mode)(void*) = (ascii ?
                (mine_is_regex ? mine_mode_ascii_regexp : mine_mode_ascii) :
                (mine_is_regex ? mine_mode_sjis_regex : mine_mode_sjis));
        mine_arg t_arg = { min_rnd, max_rnd, mine_test, caseless, NULL };

        mtx_t t_mtx;
        mtx_init(&t_mtx, NULL);
        mtx_lock(&t_mtx);
        t_arg.mtx = &t_mtx;

        thrd_t* t = malloc(total_threads * sizeof(thrd_t*));
        for (int i = 0; i < total_threads; ++i)
            thrd_create(&t[i], mine_mode, (void*)&t_arg);

        if (timeout)
            SLEEP(timeout);
        else
            while (!exit_loops);

        mtx_unlock(&t_mtx);
        for (int j = 0; j < total_threads; ++j) {
            int tmp_ret = 0;
            thrd_join(t[j], (void**)&tmp_ret);
            total_trips += tmp_ret;
        }

        free(t);
    } else if (mode == M_TEST)
        test_mode();
    else if (mode == M_GEN) {
        if (total_gen <= 0) {
            printf("WARNING! Total generation amount is 0, enabling non-stop mode!\n");

            void (*nstop_gen_mode)(void*) = (ascii ? nstop_gen_mode_ascii : nstop_gen_mode_sjis);
            nstop_gen_arg t_arg = { min_rnd, max_rnd, NULL };

            mtx_t t_mtx;
            mtx_init(&t_mtx, NULL);
            mtx_lock(&t_mtx);
            t_arg.mtx = &t_mtx;

            thrd_t t;
            thrd_create(&t, nstop_gen_mode, (void*)&t_arg);

            if (timeout)
                SLEEP(timeout);
            else
                while (!exit_loops);

            mtx_unlock(&t_mtx);
            thrd_join(t, (void**)&total_trips);
        } else {
            void (*gen_mode)(int, int, int) = (ascii ? gen_mode_ascii : gen_mode_sjis);
            gen_mode(total_gen, min_rnd, max_rnd);
            total_trips = total_gen;
        }
    } else if (mode == M_BENCH) {
        if (total_gen <= 0) {
            printf("WARNING! Total benchmark generation amount is 0, enabling non-stop mode!\n");

            void (*nstop_bench_mode)(void*) = (ascii ? nstop_bench_mode_ascii : nstop_bench_mode_sjis);
            bench_arg t_arg = { min_rnd, max_rnd, 0, NULL };

            mtx_t t_mtx;
            mtx_init(&t_mtx, NULL);
            mtx_lock(&t_mtx);
            t_arg.mtx = &t_mtx;

            thrd_t* t = malloc(total_threads * sizeof(thrd_t*));
            for (int i = 0; i < total_threads; ++i)
                thrd_create(&t[i], nstop_bench_mode, (void*)&t_arg);

            if (timeout)
                SLEEP(timeout);
            else
                while (!exit_loops);

            mtx_unlock(&t_mtx);
            for (int j = 0; j < total_threads; ++j) {
                int tmp_ret = 0;
                thrd_join(t[j], (void**)&tmp_ret);
                total_trips += tmp_ret;
            }

            free(t);
        } else {
            void (*bench_mode)(void*) = (ascii ? bench_mode_ascii : bench_mode_sjis);
            bench_arg t_arg = { min_rnd, max_rnd, total_gen, NULL };

            if (total_threads == 1) {
                bench_mode((void*)&t_arg);
            } else {
                int trips_per_thread = total_gen / total_threads;
                int first_thread_total = trips_per_thread;
                if ((trips_per_thread * total_threads) != total_gen)
                    first_thread_total += (total_gen - (trips_per_thread * total_threads));

                thrd_t* t = malloc(total_threads * sizeof(thrd_t*));
                bench_arg t_args[total_threads];
                for (int i = 0; i < total_threads; ++i) {
                    t_args[i].target = (!i ? first_thread_total : trips_per_thread);
                    t_args[i].min    = min_rnd;
                    t_args[i].max    = max_rnd;
                }

                for (int j = 0; j < total_threads; ++j)
                    thrd_create(&t[j], bench_mode, (void*)&t_args[j]);
                for (int k = 0; k < total_threads; ++k)
                    thrd_join(t[k], NULL);

                free(t);
            }
            total_trips = total_gen;
        }
    } else if (mode == M_SINGLE)
        single_mode();

    if (benchmark) {
        float exec_time = ((get_time() - start_time) / 1000000.f);
        printf("\nexec time:   %f\ntotal trips: %d\ntrips p/s:   %f\n",
                exec_time, total_trips, (total_trips / exec_time));
    }
    return EXIT_SUCCESS;
}

void print_help (const char* prog_name) {
    printf("http://www.github.com/badassmofo/tripping\n");
    printf("usage: %s [mode] [option] [options]\n\n", prog_name);
    printf("modes\n");
    printf("  single -> If no arguments are entered, it'll spit out a random tripcode\n");
    printf("  test   -> Interactive test mode, or test specified trips\n");
    printf("            %s test these tripcodes or %s test\n", prog_name, prog_name);
    printf("  gen    -> Generate a specified amount of tripcodes, or until quit\n");
    printf("            %s gen 100 or %s gen\n", prog_name, prog_name);
    printf("  mine   -> Try and bruteforce match a tripcode using regex or just substring search\n");
    printf("            %s mine /^test/ or %s mine test\n", prog_name, prog_name);
    printf("  bench  -> Benchmark mode, same as gen without printing out the trips\n");
    printf("  help   -> Prints this message!\n");
    printf("\noptions\n");
    printf("  --benchmark/-b -> Benchmark operation (different to bench mode)\n");
    printf("  --ascii/-a     -> ASCII only mode, don't use SJIS at all\n");
    printf("  --caseless/-i  -> Caseless matching (mine mode only)\n");
    printf("  --threads/-th  -> Specify amount of threads to use\n");
    printf("  --timeout/-to  -> Specify timeout for non-stop gen/bench and mine (secs)\n");
    printf("  --min-rnd/-mi  -> Specify the minimum random string length\n");
    printf("  --max-rnd/-mx  -> Specify the maximum random string length\n");
    printf("\nexamples:\n");
    printf("  %s mine /^hello/ -a -b -i\n", prog_name);
    printf("    Search for tripcodes that begin with \"hello\", only using ASCII\n");
    printf("    but ignore the case and benchmark it.\n\n");
    printf("  %s mine /^(\d+)$/ -th 8\n", prog_name);
    printf("    Search for tripcodes that is just a number using 8 threads.\n\n");
    printf("  %s gen -to 10 -mi 5 -mx 10\n", prog_name);
    printf("    Generate random tripcodes for 10 seconds using the range 5 to 10\n\n");
    printf("  %s test all of these tripcodes for me\n", prog_name);
    printf("    Turn \"all\", \"of\", \"these\", \"tripcodes\", \"for\" and \"me\" into tripcodes\n");
}

void signal_handler (int signal) {
    switch (signal) {
        case SIGTERM:
            printf(" EXITING! SIGTERM (terminated)\n");
            break;
        case SIGINT:
            printf(" EXITING! SIGINT (interrupted)\n");
            break;
        default:
            printf(" EXITING! UNKNOWN (%d)\n", signal);
    }
    exit_loops = 1;
}

long get_time() {
#if defined PLAT_OSX
    static mach_timebase_info_data_t freq = {0, 0};
    if (freq.denom == 0)
        mach_timebase_info(&freq);
    return (mach_absolute_time() * freq.numer / freq.denom) / 1000;
#elif defined PLAT_WIN
    HANDLE cur_thread   = GetCurrentThread();
    DWORD_PTR prev_mask = SetThreadAffinityMask(cur_thread, 1);

    static LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    SetThreadAffinityMask(cur_thread, prev_mask);
    return (1000000 * time.QuadPart / freq.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((long)(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000);
#endif
}

void single_mode () {
    iconv_t cd = iconv_open("SJIS//IGNORE", "UTF-8");
    size_t len = RAND_RANGE(2, 14);
    char*  rnd = malloc(len * 5);
           len = rndstr_sjis(len, rnd);
    char*  out = gen_trip_sjis(cd, rnd, len);
    printf("%s => %s\n", rnd, out);
    free(rnd);
    free(out);
    iconv_close(cd);
}

void test_mode() {
    iconv_t cd = iconv_open("SJIS//IGNORE", "UTF-8");

#define BUF_MAX 128
#define MAX_LEN 24

    char buf[BUF_MAX];
    size_t len = 0;

    while (!exit_loops) {
#if defined PLAT_WIN
        printf("> ");
#else
        printf("\e[01;32m>\e[0m ");
#endif

        if (fgets(buf, BUF_MAX, stdin) == NULL)
            break;
        len = strlen(buf);

        if (len > MAX_LEN) {
            buf[MAX_LEN   - 1] = '\0';
            len = MAX_LEN - 1;
        } else
            buf[len - 1] = '\0';

        if (strcmp(buf, "exit") == 0)
            break;

        char* out = gen_trip_sjis(cd, buf, len);
#if defined PLAT_WIN
        printf("< %s\n", out);
#else
        printf("\e[01;33m>\e[0m %s\n", out);
#endif
        free(out);
    }

    iconv_close(cd);
}

bool thread_quit (mtx_t* mtx) {
    switch (mtx_trylock(mtx)) {
        case thrd_success:
            mtx_unlock(mtx);
            return true;
        case thrd_busy:
            return false;
    }
    return true;
}

void gen_mode_ascii (unsigned int total, unsigned int rnd_min, unsigned int rnd_max) {
    for (int i = 0; i < total; ++i) {
        size_t len = RAND_RANGE(rnd_min, rnd_max);
        char*  rnd = rndstr_ascii(len);
        char*  out = gen_trip_ascii(rnd, len);

        printf("%s => %s\n", rnd, out);
        free(rnd);
        free(out);
    }
}

void gen_mode_sjis (unsigned int total, unsigned int rnd_min, unsigned int rnd_max) {
    iconv_t cd = iconv_open("SJIS//IGNORE", "UTF-8");
    for (int i = 0; i < total; ++i) {
        size_t len = RAND_RANGE(rnd_min, rnd_max);
        char*  rnd = malloc(len * 5);
        len = rndstr_sjis(len, rnd);
        char*  out = gen_trip_sjis(cd, rnd, len);

        printf("%s => %s\n", rnd, out);
        free(rnd);
        free(out);
    }
    iconv_close(cd);
}

void* nstop_gen_mode_ascii (void* arg) {
    nstop_gen_arg t_arg = *((nstop_gen_arg*)arg);
    int total_gen = 0;

    while (!thread_quit(t_arg.mtx)) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = rndstr_ascii(len);
        char*  out = gen_trip_ascii(rnd, len);

        printf("%s => %s\n", rnd, out);
        free(rnd);
        free(out);

        total_gen += 1;
    }

    return (void*)total_gen;
}

void* nstop_gen_mode_sjis (void* arg) {
    nstop_gen_arg t_arg = *((nstop_gen_arg*)arg);
    iconv_t cd = iconv_open("SJIS//IGNORE", "UTF-8");
    int total_gen = 0;

    while (!thread_quit(t_arg.mtx)) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = malloc(len * 5);
        len = rndstr_sjis(len, rnd);
        char*  out = gen_trip_sjis(cd, rnd, len);

        printf("%s => %s\n", rnd, out);
        free(rnd);
        free(out);

        total_gen += 1;
    }

    iconv_close(cd);
    return (void*)total_gen;
}

void bench_mode_ascii (void* arg) {
    bench_arg t_arg = *((bench_arg*)arg);
    for (int i = 0; i < t_arg.target; ++i) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = rndstr_ascii(len);
        char*  out = gen_trip_ascii(rnd, len);
        free(rnd);
        free(out);
    }
}

void bench_mode_sjis (void* arg) {
    bench_arg t_arg = *((bench_arg*)arg);
    iconv_t cd = iconv_open("SJIS//IGNORE", "UTF-8");
    for (int i = 0; i < t_arg.target; ++i) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = malloc(len * 5);
        len = rndstr_sjis(len, rnd);
        char*  out = gen_trip_sjis(cd, rnd, len);
        free(rnd);
        free(out);
    }
    iconv_close(cd);
}

void* nstop_bench_mode_ascii (void* arg) {
    bench_arg t_arg = *((bench_arg*)arg);
    int total_gen = 0;

    while (!thread_quit(t_arg.mtx)) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = rndstr_ascii(len);
        char*  out = gen_trip_ascii(rnd, len);
        free(rnd);
        free(out);

        total_gen += 1;
    }

    return (void*)total_gen;
}

void* nstop_bench_mode_sjis (void* arg) {
    bench_arg t_arg = *((bench_arg*)arg);
    iconv_t cd = iconv_open("SJIS//IGNORE", "UTF-8");
    int total_gen = 0;

    while (!thread_quit(t_arg.mtx)) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = malloc(len * 5);
        len = rndstr_sjis(len, rnd);
        char*  out = gen_trip_sjis(cd, rnd, len);
        free(rnd);
        free(out);

        total_gen += 1;
    }

    iconv_close(cd);
    return (void*)total_gen;
}

void str_to_lower(char* dst, const char* src, size_t src_len) {
    for (size_t i = 0; i < src_len; ++i)
        dst[i] = (src[i] >= 65 && src[i] <= 90 ? src[i] + 32 : src[i]);
    dst[src_len] = '\0';
}

bool str_contains (const char* a, size_t a_len, const char* b, size_t b_len) {
    if (b_len > a_len || !a || !b)
        return false;

    for (size_t i = 0; i < a_len; ++i) {
        if (a[i] == b[0]) {
            bool match = true;
            for (size_t j = 1; j < b_len; ++j)
                if (a[i + j] != b[j])
                    match = false;
            if (match)
                return true;
        }
    }
    return false;
}

bool str_contains_caseless (const char* a, size_t a_len, const char* b, size_t b_len) {
    char* tmp = malloc(a_len);
    str_to_lower(tmp, a, a_len);
    bool ret = str_contains(tmp, a_len, b, b_len);
    free(tmp);
    return ret;
}

void* mine_mode_ascii (void* arg) {
    mine_arg t_arg = *((mine_arg*)arg);
    int total_gen = 0;

    size_t search_len = strlen(t_arg.search);
    if (t_arg.caseless) {
        char* tmp = malloc(search_len);
        str_to_lower(tmp, t_arg.search, search_len);
        free(t_arg.search);
        t_arg.search = tmp;
        printf("%s\n", t_arg.search);
    }
    bool (*substr)(const char*, size_t, const char*, size_t) = (t_arg.caseless ? str_contains_caseless : str_contains);

    while (!thread_quit(t_arg.mtx)) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = rndstr_ascii(len);
        char*  out = gen_trip_ascii(rnd, len);

        if (substr(out, 11, t_arg.search, search_len))
            printf("%s => %s\n", rnd, out);

        free(rnd);
        free(out);

        total_gen += 1;
    }

    return (void*)total_gen;
}

void* mine_mode_ascii_regexp (void* arg) {
    mine_arg t_arg = *((mine_arg*)arg);
    int total_gen = 0;

    const char* err;
    int err_off, mvec[MVEC_LEN];
    int options = PCRE_DOTALL;
    if (t_arg.caseless)
        options |= PCRE_CASELESS;
    pcre* r = pcre_compile(t_arg.search, options, &err, &err_off, NULL);

    while (!thread_quit(t_arg.mtx)) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = rndstr_ascii(len);
        char*  out = gen_trip_ascii(rnd, len);

        int rc = pcre_exec(r, NULL, out, 11, 0, 0, mvec, MVEC_LEN);
        if (rc >= 0)
            printf("%s => %s\n", rnd, out);

        free(rnd);
        free(out);

        total_gen += 1;
    }

    return (void*)total_gen;
}

void* mine_mode_sjis (void* arg) {
    mine_arg t_arg = *((mine_arg*)arg);
    iconv_t cd = iconv_open("SJIS//IGNORE", "UTF-8");
    int total_gen = 0;

    size_t search_len = strlen(t_arg.search);
    if (t_arg.caseless) {
        char* tmp = malloc(search_len);
        str_to_lower(tmp, t_arg.search, search_len);
        free(t_arg.search);
        t_arg.search = tmp;
    }
    bool (*substr)(const char*, size_t, const char*, size_t) = (t_arg.caseless ? str_contains_caseless : str_contains);

    while (!thread_quit(t_arg.mtx)) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = malloc(len * 5);
        len = rndstr_sjis(len, rnd);
        char*  out = gen_trip_sjis(cd, rnd, len);

        if (substr(out, 11, t_arg.search, search_len))
            printf("%s => %s\n", rnd, out);

        free(rnd);
        free(out);

        total_gen += 1;
    }

    iconv_close(cd);
    return (void*)total_gen;
}

void* mine_mode_sjis_regex (void* arg) {
    mine_arg t_arg = *((mine_arg*)arg);
    int total_gen = 0;

    const char* err;
    int err_off, mvec[MVEC_LEN];
    int options = PCRE_DOTALL;
    if (t_arg.caseless)
        options |= PCRE_CASELESS;
    pcre* r = pcre_compile(t_arg.search, options, &err, &err_off, NULL);
    iconv_t cd = iconv_open("SJIS//IGNORE", "UTF-8");

    while (!thread_quit(t_arg.mtx)) {
        size_t len = RAND_RANGE(t_arg.min, t_arg.max);
        char*  rnd = malloc(len * 5);
        len = rndstr_sjis(len, rnd);
        char*  out = gen_trip_sjis(cd, rnd, len);

        int rc = pcre_exec(r, NULL, out, 11, 0, 0, mvec, MVEC_LEN);
        if (rc >= 0)
            printf("%s => %s\n", rnd, out);

        free(rnd);
        free(out);

        total_gen += 1;
    }

    iconv_close(cd);
    return (void*)total_gen;
}

