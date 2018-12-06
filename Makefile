CFLAGS=-Wall -std=c11 -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

shun2cc: $(OBJS)
	cc -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): shun2cc.h

test: shun2cc
	./test.sh

clean:
	rm -f shun2cc *.o *~ tmp*