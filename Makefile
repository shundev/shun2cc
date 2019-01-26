CFLAGS=-Wall -std=c11 -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

shun2cc: $(OBJS)
	gcc -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): shun2cc.h

debug: $(OBJS)
	gcc -O0 -o shun2cc $(OBJS) $(LDFLAGS)

test: shun2cc
	./shun2cc -test
	./test.sh

clean:
	rm -f shun2cc *.o *~ tmp*
