CFLAGS=-Wall -g

OBJECTS1=quine.o
OBJECTS2=quine_out.o
quine quine_out: $(OBJECTS1) $(OBJECTS2)
	$(CC) -o quine quine.o
	$(CC) -o quine_out quine_out.o 
	rm -rf *.o
clean:	
	rm -rf *.o quine quine_out
# DO NOT DELETE
quine.o: quine.c

quine_out.o: quine_out.c

