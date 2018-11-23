
CFLAGS = -Wall -O0 -g -std=c11

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OUTPUT = mycc

HEADER = tokenizer.h \
		 vector_map.h \
		 parser.h \
		 codegen.h \
		 semantic.h

.PHONY: test

$(OUTPUT): $(OBJS)
	cc -o $@ $^

$(OBJS): $(HEADER)

test: $(OUTPUT)
	./run_test.sh

wip: $(OUTPUT)
	./run_test.sh wip

clean:
	rm $(OUTPUT) *.o


