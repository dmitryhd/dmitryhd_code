OBJECTS=main.o genetic.o word.o crypt.o 
CFLAGS=-Wall -g3
main: $(OBJECTS)
	$(CC) -o $@ $^
clean:	
	rm -rf *.o main
# DO NOT DELETE
word.o: word.c word.h 
crypt.o: crypt.c crypt.h word.h
genetic.o: genetic.c genetic.h my_template.h crypt.h word.h
main.o: main.c my_template.h word.h genetic.h crypt.h word.o genetic.o crypt.o
