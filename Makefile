shun2cc: shun2cc.c

test: shun2cc
	./test.sh

clean:
	rm -f shun2cc *.o *~ tmp*