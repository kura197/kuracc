
CFLAGS = -Wall -O0 -g -std=c11

#SRCS = $(wildcard *.c)
SRCS = main.c \
	   vector_map.c \
	   tokenizer.c \
	   parser.c \
	   sem_analy.c \
	   codegen.c

OBJS = $(SRCS:.c=.o)
OUTPUT = kuracc

HEADER = kuracc.h 

.PHONY: test
.PHONY: self
.PHONY: wip

$(OUTPUT): $(OBJS)
	cc -o  $@ $^

$(OBJS): $(HEADER)

test: $(OUTPUT)
	./run_test.sh $(OUTPUT)

self: $(OUTPUT)
	./run_selftest.sh

self_test: self
	./run_test.sh self_kuracc

wip: $(OUTPUT)
	./run_test.sh wip

clean:
	rm $(OUTPUT) *.o *.s


