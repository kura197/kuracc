
CFLAGS = -Wall -O0 -g -std=c11

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OUTPUT = mycc

HEADER = tokenizer.h \
		 vector_map.h \
		 parser.h \
		 codegen.h

$(OUTPUT): $(OBJS)
	cc -o $@ $^

$(OBJS): $(HEADER)

test: $(OUTPUT)
	./test.sh

clean:
	rm $(OUTPUT) *.o


