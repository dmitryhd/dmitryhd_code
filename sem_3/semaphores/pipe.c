#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <ctype.h>

#define ECRPIPE 10 //pipe creation error
#define EFORK 11
#define ARGLEN 50
#define BUF 1000
#define NAMELEN 50

#define WRITE 1
#define READ 0
int semid[3];

void sem_set(int i);
void sem_wait(int i);
void sem_create();

void pipe_write( char * message, int fd, int max);
void pipe_read(int fd, int max);
int main( int argc, char ** argv)
{
	int pipefd[2], max;
	pid_t cpid;
	max = 0100;
	char * message = (char *)malloc( max * sizeof(char));

	strcpy( message, "abacaba");

	if( pipe( pipefd)){
		perror( "pipe creation error\n");
		exit(ECRPIPE);
	}
	sem_create();
	int f = 0;
	cpid = fork();
	if( cpid == -1){
		perror( "fork error\n");
		exit( EFORK);
	}
	if( !cpid){ //parent, write
		//close( pipefd[0]);
		for( f = 0; f < 5; f++){
			sem_wait(WRITE);
			printf("! parent write\n");
			pipe_write( "parent_write\n", pipefd[1], max);
			sem_set(WRITE);

			sem_wait(READ);
			printf("! parent read\n");
			pipe_read( pipefd[0], max);
			//sem_set(READ);
		}
		wait( NULL);
		close( pipefd[1]);
		return 0;
	}
	else{	//child, read
		//close( pipefd[1]);
		long i;
		for( f = 0; f < 5; f++){
			sem_wait(WRITE);
			printf("! child read\n");
			pipe_read( pipefd[0], max);
			//sem_set(WRITE);

			//sem_wait(WRITE);
			printf("! child write\n");
			pipe_write( "child_write\n", pipefd[1], max);
			sem_set(READ);
			sem_set(WRITE);
			//sem_set(2);
		}
		close( pipefd[0]);
		return 0;
	}
	return 0;
}
void pipe_write( char * message, int fd, int max)
{
	int size = write( fd, message, max);
	if(size < 0){	
		perror("can't write\n");
		exit(-1);
	}		
}

void pipe_read(int fd, int max)
{
	char buf[BUF];
	int size = read( fd, buf, max);
	if(size != max){
		perror("pipe read error\n");
		exit(ECRPIPE);
	}
	printf("read: %s\n",buf);
}
	
void sem_create()
{
	if( (semid[0] = semget( IPC_PRIVATE, 1, 0666 | IPC_CREAT)) < 0){
		printf( "semaphore get error\n");
		exit( -1);
	}
	if( (semid[1] = semget( IPC_PRIVATE, 1, 0666 | IPC_CREAT)) < 0){
		printf( "semaphore get error\n");
		exit( -1);
	}
	if( (semid[2] = semget( IPC_PRIVATE, 1, 0666 | IPC_CREAT)) < 0){
		printf( "semaphore get error\n");
		exit( -1);
	}
	sem_set(WRITE);
}
void sem_set(int i)
{	
	struct sembuf sbuf;
	sbuf.sem_op = 1;
	sbuf.sem_flg = 0;
	sbuf.sem_num = 0;
	if( semop( semid[i], &sbuf, 1) < 0){
		perror( "semaphore set error\n");
		exit( -1);
	}
}

void sem_wait(int i)
{	
	struct sembuf sbuf;
	sbuf.sem_op = -1;
	sbuf.sem_flg = 0;
	sbuf.sem_num = 0;
	if( semop( semid[i], &sbuf, 1) < 0){
		perror( "semaphore wait error\n");
		exit( -1);
	}
}
