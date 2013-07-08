CC      = clang
CFLAGS  = -Weverything -ansi -0s -std=gnu99
LINKER  = -lrt -pthread -lcrypto -liconv
INC     = src/*.c lib/threads/threads_posix.c

all: src/main.c
	$(CC) $(CFLAGS) $(INC) $(AFLAGS) $(LINKER)

debug: src/main.c
	$(CC) $(CFLAGS) -DDEBUGGING $(INC) $(AFLAGS) $(LINKER)

