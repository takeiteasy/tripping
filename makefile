CC      = clang
CFLAGS  = -Weverything -02 -std=gnu99
LINKER  = -pthread -lcrypto -liconv
INC     = src/*.c lib/threads/threads_posix.c

all: src/main.c
	$(CC) $(CFLAGS) $(INC) $(AFLAGS) $(LINKER)

debug: src/main.c
	$(CC) $(CFLAGS) -g -DDEBUGGING $(INC) $(AFLAGS) $(LINKER)

