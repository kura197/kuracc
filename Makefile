
CFLAGS = -Wall -O0 -g -std=c11

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OUTPUT = mycc

HEADER = token.h

$(OUTPUT): $(OBJS)
	cc -o $@ $^

$(OBJS): $(HEADER)

test: $(OUTPUT)
	./test.sh

clean:
	rm $(OUTPUT) *.o


