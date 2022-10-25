CC      = clang
LINKER  = -pthread -liconv -lpcre
SRC     = src/*.c
UNAME  := $(shell uname -s)
ifeq ($(UNAME),Darwin)
INC		= -I/opt/homebrew/include -L/opt/homebrew/lib
else
INC		=
endif

all: $(SRC)
	$(CC) $(SRC) $(INC) $(OUT) $(LINKER) -o tripping

debug: $(SRC)
	$(CC) -DDEBUGGING $(SRC) $(INC) $(LINKER) -o tripping

