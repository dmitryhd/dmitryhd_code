#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define ECRPIPE 10 //pipe creation error
pid_t ppid, cpid;
int sum = 0;

void sig_hand( int num)
{
	printf("signal #%d!\n",num);
	printf("pid =%d,sum = %d\n",getpid(),sum);
	exit(0);
}

int main( int argc, char ** argv)
{
	int pipefd[2], i, size, max;
	max = atoi(argv[1]);
	printf( "max=%d\n",max);

	struct sigaction sigchild_act;
	memset(&sigchild_act,0,sizeof(struct sigaction));
	sigchild_act.sa_handler = &sig_hand;
	sigaction(SIGPIPE,&sigchild_act,NULL);

	char * message = (char *)malloc( max * sizeof(char));

	for( i = 0; i < max; i++)
		message[i] = '0';
	message[i] = 0;

	if( pipe( pipefd)){
		perror( "pipe creation error\n");
		exit(ECRPIPE);
	}
	ppid = getpid();
	cpid = fork();
	if(!cpid){
		close( pipefd[0]);
		cpid = getpid();
		printf("i'm a child, pid = %d\n",cpid);
		fcntl( pipefd[1],F_SETFD,O_NONBLOCK);
		while(1){
			size = write( pipefd[1], message, max);
			//printf("errno = %d\n",errno);
			printf("%d, size = %d\n",sum,size);
			if( size != max){
				//printf("full, %d\n",sum);
				break;
				//return 1;
			}
			sum += max;
		}

		printf("full, %d\n",sum);
		close( pipefd[1]);
		return 0;
	}
	else{
		close( pipefd[1]);
		printf("i'm a parent, pid = %d\n",ppid);
	//	sleep(4);
		//printf("killed %d\n",wait(NULL));
	}
	return 0;
}
