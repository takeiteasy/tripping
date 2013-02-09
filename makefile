CC     = clang
CFLAGS = -Weverything -O3
LINKER = -pthread -lcrypto

xtrip:
	$(CC) $(CFLAGS) main.c -o xtrip $(LINKER)
