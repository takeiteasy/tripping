CC      = clang
CFLAGS  = -Weverything -ansi -0s -std=c11
LINKER  = -pthread -lcrypto -liconv
INC     = *.c lib/threads/threads_posix.c

all: main.c
	$(CC) $(CFLAGS) $(INC) $(AFLAGS) $(LINKER)

debug: main.c
	$(CC) $(CFLAGS) -DDEBUGGING $(INC) $(AFLAGS) $(LINKER)

