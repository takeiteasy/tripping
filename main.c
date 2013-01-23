/*
 * main.c
 * xtrip
 * Created by Rusty on 22/12/2012.
 * Copyright (c) 2012 Denko Corp. All rights reserved.
 */

#if ((__APPLE__ & __MACH__) || macintosh || Macintosh)
#   define __AVAILABILITYMACROS__
#   define DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#   define _OS_OSX
#elif (__WIN32__ || _WIN32 || _WIN64 || __WINDOWS__)
#   define WIN32_LEAN_AND_MEAN
#   define NOMINMAX
#   define VC_EXTRALEAN
#   define _OS_WIN
#endif

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#if defined _OS_OSX
#   include <mach/mach_time.h> /* For OSX systime() */
#endif

#if defined _OS_WIN
#   include <windows.h> /* For Windows systime()       */
#   include <conio.h>   /* For kbhit() & getch()       */
#else                   /* For POSIX kbhit() & getch() */
#   include <termios.h>
#   include <unistd.h>
#   include <fcntl.h>
#endif

#include <openssl/des.h>

#define MAX_ENCRYPT_LEN 15
#define MIN_ENCRYPT_LEN 3

#define ESCAPE_CHAR 27
#define TRUE        1   
#define FALSE       0
typedef unsigned int uint32_t;
typedef uint32_t     bool_t;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

static char char_pool[] = { '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', ']', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}' };
#define CHAR_POOL_LEN (ARRAY_SIZE(char_pool))

#define RAND_STR_LEN ((rand() % ((MAX_ENCRYPT_LEN + 1) - MIN_ENCRYPT_LEN)) + MIN_ENCRYPT_LEN)
// #define RAND_STR_LEN2(a) ((rand() % ((a + 1) - MIN_ENCRYPT_LEN)) + MIN_ENCRYPT_LEN)

/* Prototypes */
int64_t sys_time();
char*   rand_str(uint32_t len);
size_t  htmlspecialchars(char* dst, const char* src, size_t src_len);
void    salt_gen(char* salt, const char* src, size_t src_len);
char*   trip_gen(const char* src, size_t src_len);
void    srt_to_lower(char* dst, const char* src, size_t src_len);
bool_t  str_contains(const char* hay, const char* needle);
bool_t  str_contains_ignore_case(const char* hay, const char* needle);
bool_t  is_number(const char* str);
void    usage();

/* Get system time */
int64_t sys_time() {
#if defined _OS_OSX
    static mach_timebase_info_data_t freq = {0, 0};
    if (freq.denom == 0)
        mach_timebase_info(&freq);
    return ((mach_absolute_time() * freq.numer / freq.denom) / 1000);
#elif defined _OS_WIN
    HANDLE cur_thread   = GetCurrentThread();
    DWORD_PTR prev_mask = SetThreadAffinityMask(cur_thread, 1);
    
    static LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    
    SetThreadAffinityMask(cur_thread, prev_mask);
    return (1000000 * time.QuadPart / freq.QuadPart);
#else /* Linux/BSD */
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return ((int64_t)(time.tv_sec) * 1000000 + time.tv_nsec / 1000);
#endif
}

/* For benchmarking */
static int32_t gen_count = 0;

/* Construct random string from char pool */
char* rand_str(uint32_t len) {
    char* ret = (char*)malloc(len);
    for (uint32_t i = 0; i < len; ++i)
        ret[i] = char_pool[(rand() % CHAR_POOL_LEN)];
    ret[len] = '\0';
    return ret;
}

/* PHP htmlspecialchars-like function */
size_t htmlspecialchars(char* dst, const char* src, size_t src_len) {
    size_t nlen = src_len, j = 0;
    for (uint32_t i = 0; i < src_len; ++i) {
        switch (src[i]) {
            case '&':
                dst[j]     = '&';
                dst[j + 1] = 'a';
                dst[j + 2] = 'm';
                dst[j + 3] = 'p';
                dst[j + 4] = ';';
                
                nlen += 4;
                j += 5;
                break;
            case '<':
                dst[j]     = '&';
                dst[j + 1] = 'g';
                dst[j + 2] = 't';
                dst[j + 3] = ';';
                
                nlen += 3;
                j += 4;
                break;
            case '>':
                dst[j]     = '&';
                dst[j + 1] = 'l';
                dst[j + 2] = 't';
                dst[j + 3] = ';';
                
                nlen += 3;
                j += 4;
                break;
            case '"':
                dst[j]     = '&';
                dst[j + 1] = 'q';
                dst[j + 2] = 'u';
                dst[j + 3] = 'o';
                dst[j + 4] = 't';
                dst[j + 5] = ';';
                
                nlen += 5;
                j += 6;
                break;
            default:
                dst[j++] = src[i];
                break;
        }
    }
    return nlen;
}

/* Generate salt from source string */
void salt_gen(char* salt, const char* src, size_t src_len) {
    /* Create salt, handle if too short, or take 1st & 2nd chars */
    switch (src_len) {
        case 1:
            salt[0] = 'H';
            salt[1] = '.';
            break;
            
        case 2:
            salt[0] = src[1];
            salt[1] = 'H';
            break;
            
        default:
            salt[0] = src[1];
            salt[1] = src[2];
            break;
    }
    salt[3] = '\0';
    
    /* Replace chars */
    for (uint32_t i = 0; i < 2; ++i) {
        char cur_c = salt[i];
        
        if (cur_c < '.' || cur_c > 'z')
            salt[i] = '.';
        else if (cur_c >= ':' && cur_c <= '@')
            salt[i] = (char)(cur_c + ('A' - ':'));
        else if (cur_c >= '[' && cur_c <= '`')
            salt[i] = (char)(cur_c + ('a' - '['));
    }
}

char* trip_gen(const char* src, size_t src_len) {
    /* Make salt & converted source */
    char salt[3];
    char* dst = (char*)malloc((src_len + 2) * 5);
    htmlspecialchars(dst, src, src_len);
    salt_gen(salt, dst, src_len);
    
    /* DES encrypt */
    char* des_ret = (char*)malloc(14);
    DES_fcrypt(dst, salt, des_ret);
    free(dst);
    
    /* Get & return last 11 characters */
    char* ret = (char*)malloc(11);
    memcpy(ret, des_ret + 3, 11);
    free(des_ret);
    
    gen_count++;
    return ret;
}

void srt_to_lower(char* dst, const char* src, size_t src_len) {
    for (uint32_t i = 0; i < src_len; ++i)
        dst[i] = (char)tolower(src[i]);
    dst[src_len] = '\0';
}

bool_t str_contains(const char* hay, const char* needle) {
    size_t h_s = strlen(hay);
    size_t n_s = strlen(needle);
    bool_t  matches = TRUE;
    
    for (uint32_t i = 0; i < h_s; ++i) {
        if (hay[i] == needle[0]) {
            for (uint32_t j = 1; j < n_s; ++j)
                if (hay[i + j] != needle[j])
                    matches = FALSE;
            if (matches)
                return TRUE;
        }
    }
    return FALSE;
}

bool_t str_contains_ignore_case(const char* hay, const char* needle) {
    size_t h_s = strlen(hay);
    char* hay_lower = (char*)malloc(h_s);
    srt_to_lower(hay_lower, hay, h_s);
    size_t n_s = strlen(needle);
    char* needle_lower = (char*)malloc(n_s);
    srt_to_lower(needle_lower, needle, n_s);
    bool_t  ret = str_contains(hay_lower, needle_lower);
    free(hay_lower);
    free(needle_lower);
    return ret;
}

static bool_t (*str_contains_func)(const char*, const char*);

bool_t is_number(const char* str) {
    for (uint32_t i = 0; i < strlen(str); i++)
	if (!isdigit(str[i]))
		return FALSE;
    return TRUE;
}

#if !defined _OS_WIN
int32_t getch() {
	struct termios o, n;
	int32_t ch;
    
	tcgetattr(STDIN_FILENO, &o);
	n = o;
	n.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &n);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &o);
	return toascii(ch);
}

int32_t kbhit() {
	struct termios o, n;
    int32_t ch, f;
    
    tcgetattr(STDIN_FILENO, &o);
    n = o;
    n.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &n);
    f = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, f | O_NONBLOCK);
    
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &o);
    fcntl(STDIN_FILENO, F_SETFL, f);
    
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
#endif

void usage() {
    printf("Arguments:\n-h  -help:        Print help\n-g  -generate:    Generate tripcode\n-s  -search:      Search for tripcode\n-ic -ignore-case: Ignore case when searching\n-b  -benchmark:   Time & measure program\n-v  -verboose:    Verboose mode\n-f  -file:        Print to file\n\nExamples:\n./xtrip -g 10 -b:      Generates 10 random trips & time\n./xtrip or ./xtrip -g: The same as ./xtrip -g 1\n./xtrip -g test:       Generates trip from \"test\"\n./xtrip -s test -ic:   Search for trips with \"test\" in them & ignore case\n\nNotes:\nSearch mode will continue searching until ESC key is pressed.\nUsing -f with search mode will print to both terminal & file.\n");
}

int main(int argc, const char* argv[]) {
    srand((uint32_t)(time(NULL)));
    
    bool_t  benchmark = FALSE,
              ignore_case   = FALSE,
              verboose      = FALSE,
              gen_mode      = FALSE,
              gen_from_src  = FALSE,
              search_mode   = FALSE,
              p2file        = FALSE;
    
    int32_t total_gen = 0;
    char*   arg_str   = NULL; /* Freed at end */
    
#if (defined DEBUG || defined NDEBUG)
    verboose = TRUE;
#endif
    
    if (argc == 1) {
        total_gen = 1;
        gen_mode  = TRUE;
    }
    else {
        /* Parse program arguments */
        for (int32_t i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "-help")        == 0 || strcmp(argv[i], "-h")  == 0) {
                usage();
                return EXIT_SUCCESS;
            }
            if (strcmp(argv[i], "-benchmark")   == 0 || strcmp(argv[i], "-b")  == 0)
                benchmark   = TRUE;
            if (strcmp(argv[i], "-ignore-case") == 0 || strcmp(argv[i], "-ic") == 0)
                ignore_case = TRUE;
            if (strcmp(argv[i], "-verboose")    == 0 || strcmp(argv[i], "-v")  == 0)
                verboose    = TRUE;
            if (strcmp(argv[i], "-file")        == 0 || strcmp(argv[i], "-f")  == 0)
                p2file      = TRUE;
            if (strcmp(argv[i], "-generate")    == 0 || strcmp(argv[i], "-g")  == 0) {
                if ((i + 1) >= argc) {
                    total_gen = 1;
                    gen_mode  = TRUE;
                }
                else {
                    if (is_number(argv[i + 1])) {
                        total_gen = atoi(argv[i + 1]);
                        gen_mode  = (total_gen != 0);
                    }
                    else {
                        arg_str = (char*)malloc(ARRAY_SIZE(argv[i + 1]));
                        strcpy(arg_str, argv[i + 1]);
                        gen_from_src = (arg_str != NULL);
                        gen_mode     = gen_from_src;
                    }
                }
            }
            if (strcmp(argv[i], "-search")      == 0 || strcmp(argv[i], "-s")  == 0) {
                if ((i + 1) >= argc) {
                    usage();
                    return EXIT_FAILURE;
                }
                
                arg_str = (char*)malloc(ARRAY_SIZE(argv[i + 1]));
                strcpy(arg_str, argv[i + 1]);
                search_mode = (arg_str != NULL);
            }
        }
    }
    
    /* Can't have no mode or both modes */
    if ((!gen_mode && !search_mode) || (gen_mode && search_mode)) {
        usage();
        return EXIT_FAILURE;
    }
    
    /* Print enabled options */
    if (verboose) {
        benchmark = TRUE;
        printf("OPT ENABLED: VERBOSE MODE\n");
        if (benchmark)    printf("OPT ENABLED: BENCHMARK\n");
        if (ignore_case)  printf("OPT ENABLED: IGNORE-CASE\n");
        if (p2file)       printf("OPT ENABLED: PRINT2FILE\n");
        if (gen_mode)     printf("OPT ENABLED: GENERATE MODE\n");
        if (gen_from_src) printf("OPT ENABLED: GENERATE FROM SOURCE\n");
        if (search_mode)  printf("OPT ENABLED: SEARCH MODE\n");
        printf("\n");
    }
    
    /* Avoid having to test every loop */
    if (search_mode) {
        if (ignore_case)
            str_contains_func = str_contains_ignore_case;
        else
            str_contains_func = str_contains;
    }
    else
        str_contains_func = NULL;
    
    FILE* fp = NULL;
    if (p2file) {
        /* Generate file name */
        time_t rt;
        time(&rt);
        struct tm *ti;
        ti = localtime(&rt);
        char tb[128];
        strftime(tb, sizeof(tb), "xtrip_%Y-%m-%d_at_%H-%M-%S.txt", ti);
        
        /* Open file stream */
        fp = fopen(tb, "w");
        if (!fp) {
            printf("ERROR! Failed to make file!\n");
            return EXIT_FAILURE;
        }
    }
    else fp = stdout;
    
    int64_t start_time = sys_time();
    if (gen_mode) {
        if (gen_from_src) {
            size_t arg_str_len = strlen(arg_str);
            char* dst = trip_gen(arg_str, arg_str_len);
            printf("%s => %s\n", arg_str, dst);
            free(dst);
        }
        else {
            for (int32_t i = 0; i < total_gen; ++i) {
                uint32_t src_len = RAND_STR_LEN;
                char* src = rand_str(src_len);
                char* dst = trip_gen(src, src_len);
                fprintf(fp, "%s => %s\n", src, dst);
                free(src);
                free(dst);
            }
        }
    }
    else if (search_mode) {
        bool_t running = TRUE;
        while (running) {
            while (!kbhit()) { /* Loop until keyboard press */
                uint32_t src_len = RAND_STR_LEN;
                char* src = rand_str(src_len);
                char* dst = trip_gen(src, src_len);
                if (str_contains_func(dst, arg_str)) {
                    if (p2file)
                        fprintf(fp, "%s => %s\n", src, dst);
                    printf("%s => %s\n", src, dst);
                }
                free(src);
                free(dst);
            }
            
            /* If key hit is ESC exit loop */
            if (getch() == ESCAPE_CHAR) {
                running = FALSE;
                printf(" exiting search\n");
            }
        }
    }
    
    if (p2file)
        fclose(fp);
    
    if (benchmark) {
        float exec_time = ((sys_time() - start_time) / 1000000.f);
        printf("\nexec time:   %f\ntotal trips: %d\ntrips p/s:   %f\n",
               exec_time, gen_count, (gen_count / exec_time));
    }
    return EXIT_SUCCESS;
}

