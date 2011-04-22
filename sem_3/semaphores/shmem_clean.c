
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>

int semid;
key_t sem_key, shm_key;

void sem_set();
void sem_wait();
void sem_create();

int main()
{
	int shmid;
	char pathname[] = "shmem_b.c"; 	
	int new = 1;
	if((sem_key = ftok(pathname,0)) < 0){
	printf("Can\'t generate key\n");
	exit(-1);
	}
	if((shm_key = ftok(pathname,1)) < 0){
		printf("Can\'t generate key\n");
		exit(-1);
	}
	//printf("sem key = %d,shmem key = %d\n",sem_key,shm_key);
	sem_create();
	
	if((shmid = shmget( shm_key, 3*sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) < 0 ){
		if(errno != EEXIST){
			perror("can't create shared memory\n");
			exit(-1);
		}
		else{
			if((shmid = shmget( shm_key, 3*sizeof(int), 0)) < 0){
				perror("can't get created memory\n");
				exit(-1);
			}
			new = 0;
		}
	}

	int * array;
	if((array = (int *)shmat( shmid, NULL, 0)) == (int *)(-1)){
		perror("can't attach sh. memory!\n");
		exit(-1);
	}
	
		sem_wait();
		array[0] = 0;
		array[1] = 0;
		array[2] = 0;
		sem_set();
	printf( "Program A was spawn %d times\n\
	Program B was spawn %d times\n\
	and total - %d times\n", array[0],array[1],array[2]);

	if(shmdt(array) < 0){
		perror("Can't detach sh mem\n");
		exit(-1);
	}

	return 0;
}

void sem_create()
{
	if( (semid = semget( sem_key, 1, 0666 | IPC_CREAT)) < 0){
		printf( "semaphore get error\n");
		exit( -1);
	}
	sem_set();
}
void sem_set()
{	
	struct sembuf sbuf;
	sbuf.sem_op = 1;
	sbuf.sem_flg = 0;
	sbuf.sem_num = 0;
	if( semop( semid, &sbuf, 1) < 0){
		printf( "semaphore set error\n");
		exit( -1);
	}
}

void sem_wait()
{	
	struct sembuf sbuf;
	sbuf.sem_op = -1;
	sbuf.sem_flg = 0;
	sbuf.sem_num = 0;
	if( semop( semid, &sbuf, 1) < 0){
		printf( "semaphore wait error\n");
		exit( -1);
	}
}
