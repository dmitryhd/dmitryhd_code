#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define ECRPIPE 10 //pipe creation error
#define EFORK 11


int main( int argc, char ** argv)
{
	int pipefd[2], i, size, max;
	pid_t cpid;
	max = atoi(argv[1]);
	int inc = atoi(argv[2]);
	printf( "max=%d, inc=%d\n",max,inc);

	char * message = (char *)malloc( max * sizeof(char));
	char * buf = (char *)malloc( max * sizeof(char));


	for( i = 0; i < max; i++)
		message[i] = '0';
	message[i] = 0;

	if( pipe( pipefd)){
		perror( "pipe creation error\n");
		exit(ECRPIPE);
	}

	cpid = fork();
	if( cpid == -1){
		perror( "fork error\n");
		exit( EFORK);
	}
	if( !cpid){ //parent, write
		FILE * stream;
		close( pipefd[0]);
		while(1){
			size = write( pipefd[1], message, max);
			if( size != max){
				return 1;
			}
			max += inc;
		}
		wait( NULL);
		close( pipefd[1]);
		return 0;
	}
	else{	//child, read
		FILE * stream;
		close( pipefd[1]);
		do{
			size = read( pipefd[0], buf, max);
			if( size != max){
				printf("not enough!\n");
				return 1;
			}
			printf("%d\n",max);
			max += inc;
		}
		while( size != max);
		printf("STOP IT!\n");
		close( pipefd[0]);
		return 0;
	}
	return 0;
}
