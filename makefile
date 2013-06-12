CC     = clang
CFLAGS = -W -Wall
LINKER = -pthread -lcrypto -liconv

all: main.c tripcode.c
	$(CC) $(CFLAGS) main.c tripcode.c $(AFLAGS) $(LINKER)

debug: main.c tripcode.c
	$(CC) $(CFLAGS) -DDEBUGGING -g main.c tripcode.c$(AFLAGS) $(LINKER)

