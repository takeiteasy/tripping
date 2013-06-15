CC     = clang
CFLAGS = -Weverything -ansi -0s -std=c99
LINKER = -pthread -lcrypto -liconv

all: main.c
	$(CC) $(CFLAGS) *.c $(AFLAGS) $(LINKER)

debug: main.c
	$(CC) $(CFLAGS) -DDEBUGGING *.c $(AFLAGS) $(LINKER)

