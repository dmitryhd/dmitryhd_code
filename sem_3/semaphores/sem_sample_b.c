#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#define EKEY 1
#define ESEM 2
union semun {
	int val;
	struct semid_ds * buf;
	unsigned short * array;
	struct seminfo *__buf;
};

int main()
{
	int semid;
	char pathname[] = "08-1ca.c";
	key_t key;
	struct sembuf sbuf;
	if((key = ftok(pathname,0)) < 0){
	printf("Can\'t generate key\n");
	exit(-1);
	}

	if( (semid = semget( key, 1, 0666 | IPC_CREAT)) < 0){
		printf( "semaphore get error\n");
		exit( ESEM);
	}
	union semun sem_u;
	printf("get id: %d\n", semid);
	sbuf.sem_op = 1;
	sbuf.sem_flg = 0;
	sbuf.sem_num = 0;
	if( semop( semid, &sbuf, 1) < 0){
		printf( "semaphore wait error\n");
		exit( ESEM);
	}
	printf( "condition set\n");
	return 0;
}
