/*Written by Dmitry Khodakov - 817 gr. 09.2009.*/
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF 100

int main (int argc, char *argv[])
{
	int segment_id = atoi(argv[1]);
	int size = atoi(argv[2]);
	char * shared_memory;
	struct shmid_ds shmbuffer;
	int segment_size;
	
	shared_memory = (char*) shmat (segment_id, 0, 0);
	printf ("out: shared memory id=%d size = %d attached at address %p\n", segment_id, size, shared_memory);

	/* Determine the segment’s size. */
	shmctl (segment_id, IPC_STAT, &shmbuffer);
	segment_size = shmbuffer.shm_segsz;
	printf ("out: segment size: %d\n", segment_size);
	printf ("out: segment id: %d\n", segment_id);

	/* Print out the string from shared memory. */
	int i;
	for( i = 0; i < size; i++){
		printf("%c",shared_memory[i]);
	}

	/* Detach the shared memory segment. */
	shmdt (shared_memory);
	/* Deallocate the shared memory segment.  */
	shmctl (segment_id, IPC_RMID, 0);
	return 0;
}

