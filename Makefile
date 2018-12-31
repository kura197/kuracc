
CFLAGS = -Wall -O0 -g -std=c11

SRCS = $(wildcard *.c)
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
	./run_test.sh self

wip: $(OUTPUT)
	./run_test.sh wip

clean:
	rm $(OUTPUT) *.o *.s


