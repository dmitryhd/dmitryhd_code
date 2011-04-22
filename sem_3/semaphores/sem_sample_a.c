#include <stdlib.h>
#include <unistd.h>
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
	char pathname[] = "08-1ca.c"; /* Èìÿ ôàéëà, èñïîëüçóþùååñÿ äëÿ ãåíåðàöèè êëþ÷à. Ôàéë ñ òàêèì èìåíåì äîëæåí ñóùåñòâîâàòü â òåêóùåé äèðåêòîðèè */
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
	sbuf.sem_op = -4;
	sbuf.sem_flg = 0;
	sbuf.sem_num = 0;
	if( semop( semid, &sbuf, 1) < 0){
		printf( "semaphore wait error\n");
		exit( ESEM);
	}

	sbuf.sem_op = -1;
	sbuf.sem_flg = 0;
	sbuf.sem_num = 0;
	if( semop( semid, &sbuf, 1) < 0){
		printf( "semaphore wait error\n");
		exit( ESEM);
	}
	printf( "Right condition\n");
	return 0;
}
