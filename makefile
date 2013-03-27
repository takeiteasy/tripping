CC     = clang
CFLAGS = -Weverything -O3
LINKER = -pthread -lcrypto
OUT    = ~/bin/xtrip

all:
	$(CC) $(CFLAGS) *.c $(AFLAGS) -o $(OUT) $(LINKER)
