CC     = clang
CFLAGS = -Weverything -O3
LINKER = -pthread -lcrypto -liconv

all: main.c tripcode.c
	$(CC) $(CFLAGS) main.c tripcode.c $(AFLAGS) $(LINKER)
