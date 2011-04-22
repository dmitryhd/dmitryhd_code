CC = gcc
CFLAGS = -Wall -g
OBJS = tree_impl.o tree_sample.o test.o

rgtree: ${OBJS}
	${CC} ${CFLAGS} -o $@ $^

tree_impl.o: tree.h
tree_sample.o: tree.h tree_impl.o
test.o: tree.h
clean:
	rm *.o
