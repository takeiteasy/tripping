CC      = clang
CFLAGS  = -Weverything -02 -std=gnu99
LINKER  = -pthread -lcrypto -liconv -lpcre
INC     = src/*.c lib/threads/threads_posix.c

all: src/main.c
	$(CC) $(CFLAGS) $(INC) $(LINKER)

debug: src/main.c
	$(CC) $(CFLAGS) -DDEBUGGING $(INC) $(LINKER)

