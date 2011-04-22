/*Written by Dmitry Khodakov - 817 gr. 09.2009.*/
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUF 0x1000

int main ()
{
	int segment_id;
	char * shared_memory;
	struct shmid_ds shmbuffer;
	int segment_size;
	const int shared_segment_size = 0x64000;
	key_t key;
	char pathname[] = "quine.c";
	/* Allocate a shared memory segment. */
	if((key = ftok(pathname,0)) < 0){
		printf("Can\'t generate key\n");
		exit(-1);
	}
	if((segment_id = shmget( key, shared_segment_size, 0666 | IPC_CREAT | IPC_EXCL)) < 0 ){
		if(errno != EEXIST){
			perror("can't create shared memory\n");
			exit(-1);
		}
		else{
			if((shmid = shmget( key, shared_segment_size, 0)) < 0){
				perror("can't get created memory\n");
				exit(-1);
			}
			new = 0;
		}
	}

	/* Attach the shared memory segment. */
	shared_memory = (char*) shmat (segment_id, 0, 0);
	printf ("shared memory attached at address %p\n", shared_memory);

	/* Determine the segment’s size. */
	shmctl (segment_id, IPC_STAT, &shmbuffer);
	segment_size = shmbuffer.shm_segsz;
	printf ("segment id: %d\n", segment_id);

	/* Write to shared memory*/
	FILE * f = fopen("quine.c","r");
	if( f == NULL){
		perror("file error\n");
		exit(1);
	}
	int size = 0;
	int tmp = 0;
	char c;
	while (1){
		tmp = fscanf(f,"%c", &c);
		if( tmp != 1)
			break;
		shared_memory[size] = c;
		size += 1;
	}
	int i;
	for( i = 0; i < size; i++){
		
		printf("%c", shared_memory[i]);
	}
	/* Detach the shared memory segment. */
	shmdt (shared_memory);
	char  arg_id[BUF];
	sprintf (arg_id, "%d", segment_id);
	char  arg_size[BUF];
	sprintf (arg_size, "%d", size);
	char *arglist[4] = {"./quine_out", arg_id, arg_size, NULL };
	execvp ("./quine_out", arglist);

	perror("error, can't run queen_out\n");
	return 0;
}

