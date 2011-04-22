CFLAGS=-Wall -g

OBJECTS=pipe_other_program.o
pipe_other_program: $(OBJECTS)
	$(CC) -o pipe_other_program pipe_other_program.o
	rm -rf *.o
clean:	
	rm -rf *.o pipe_other_program
# DO NOT DELETE
pipe_other_program.o: pipe_other_program.c
