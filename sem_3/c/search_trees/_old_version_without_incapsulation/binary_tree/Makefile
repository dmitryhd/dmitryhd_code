CC = gcc
CFLAGS = -Wall
OBJS = tree_impl.o tree_sample.o

main: ${OBJS}
	${CC} ${CFLAGS} -o $@ $^

tree_impl.o: tree.h
tree_sample.o: tree.h tree_impl.o
clean:
	rm *.o
