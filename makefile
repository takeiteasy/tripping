CC     = clang
CFLAGS = -Weverything -O3
LINKER = -pthread -lcrypto
OBJS   = main.c
OUT    = ~/bin/xtrip

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(AFLAGS) -o $(OUT) $(LINKER)
