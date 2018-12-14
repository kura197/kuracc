
CFLAGS = -Wall -O0 -g -std=c11

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OUTPUT = mycc

HEADER = mycc.h 

.PHONY: test
.PHONY: self
.PHONY: wip

$(OUTPUT): $(OBJS)
	cc -o  $@ $^

$(OBJS): $(HEADER)

test: $(OUTPUT)
	./run_test.sh $(OUTPUT)

self: $(OUTPUT)
	./run_test.sh self

wip: $(OUTPUT)
	./run_test.sh wip

clean:
	rm $(OUTPUT) *.o


