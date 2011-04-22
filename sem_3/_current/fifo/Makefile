CFLAGS=-Wall -g

OBJECTS1=fifo_read.o
OBJECTS2=fifo_write.o
fifo_read fifo_write: $(OBJECTS1) $(OBJECTS2)
	$(CC) -o fifo_read fifo_read.o
	$(CC) -o fifo_write fifo_write.o
	rm -rf *.o
clean:	
	rm -rf *.o fifo_read fifo_write
# DO NOT DELETE
fifo_read: fifo_read.o
fifo_write: fifo_write.o

