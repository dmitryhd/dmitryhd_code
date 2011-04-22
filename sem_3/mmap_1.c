/*Written by Dmitry Khodakov - 817 gr. 09.2009.*/
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#define N 2
#define PATH "mmap_1.c"

int * p;
size_t length;
int first;
int second; 

void wait ( int semid)
{
	struct sembuf sbuf;
	sbuf.sem_op = -1;
	sbuf.sem_flg = 0;
	sbuf.sem_num = 0;
	if ( semop (semid, &sbuf, 1) < 0 ){
		perror("semop error\n");
		exit(1);
	}
}
void post ( int semid)
{
	struct sembuf sbuf;
	sbuf.sem_op = 1;
	sbuf.sem_flg = 0;
	sbuf.sem_num = 0;
	if ( semop (semid, &sbuf, 1) < 0 ){
		perror("semop error\n");
		exit(1);
	}
}
int get_sem (int i)
{
	key_t key = ftok ( PATH, i);
	if ( key < 0 ){
		perror("key generation error\n");
		exit(1);
	}
	int semid = semget ( key, 1, 0666 | IPC_CREAT );
	if ( semid < 0 ){
		perror("semget error\n");
		exit(1);
	}
	printf ( "%d\n", semid);
	return semid;
}
//here we use signal handling to avoid unfreed mmaped memory
void sigint_handler(int signum)
{
	printf("get sigint =)\n");
	munmap ( (void *)p, length );
	semctl ( first, 0, IPC_RMID, NULL);
	semctl ( second, 0, IPC_RMID, NULL);
	exit( 0);

}

int main (int argc, char *argv[])
{
	//here we use signal handling to avoid unfreed mmaped memory
	signal ( SIGINT, sigint_handler);

	if ( argc != 2){
		printf( "usage: %s mappedfile\n", argv[1]);
		exit(1);
	}
	//get semaphore;
	int first = get_sem( 1);
	int second = get_sem( 2); 
	printf ( "first = %d sec = %d\n", first, second);

	//get mmapped file; 
	int fd;
	fd = open ( argv[1], O_RDWR | O_CREAT, 0666);
	if(fd < 0){
		perror("open error\n");
		exit(1);
	}
	//for one int
	length = N*sizeof(int);
	//extend file size
	ftruncate ( fd, length );
	p = (int *) mmap ( NULL, length, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	if ( p == MAP_FAILED ){
		perror("mmap error\n");
		exit(1);
	}
	//first
	p[0] = 1;
	while(1){
		//increment shared variable
		p[0]++;
		printf("p = %d\n", p[0]);
		//suncronize
		if ( msync ( p, length, MS_SYNC) ){
			perror("sync error\n");
			exit(1);
		}
		//just for debug
		sleep(1);
		//printf ( "post 1st\n");
		post ( first);
		//printf ( "wait 2nd\n");
		wait ( second);
	}
	//free 
	munmap ( (void *)p, length );
	semctl ( first, 1, IPC_RMID);
	semctl ( second, 1, IPC_RMID);
	
	return 0;
}
