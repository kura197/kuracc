
CFLAGS = -Wall -O0 -g -std=c11

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OUTPUT = mycc


$(OUTPUT): $(OBJS)
	cc -o $@ $<

$(OBJS): 

test: $(OUTPUT)
	./test.sh

clean:
	rm $(OUTPUT) *.o


