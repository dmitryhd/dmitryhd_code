CC = gcc
CFLAGS = -Wall -g
OBJS = binary_tree.o tree_sample.o test.o rb_tree.o

rgtree: ${OBJS}
	${CC} ${CFLAGS} -o $@ $^

binary_tree.o: tree.h
rb_tree.o: binary_tree.o tree.h
test.o: tree.h test.h binary_tree.o rb_tree.o
tree_sample.o: tree.h test.o
clean:
	rm *.o
