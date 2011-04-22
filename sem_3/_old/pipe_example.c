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
#define NAMELEN 50
char ** split_string( char * str);
void spawn( char * program);


int main( int argc, char ** argv)
{
	int pipefd[2], i, size, max;
	pid_t cpid;
	if( argc != 2){
		printf("usage: %s program\n",argv[0]);
		exit(1);
	}
	max = strlen(argv[1]);
	char * message = (char *)malloc( max * sizeof(char));
	char * buf = (char *)malloc( max * sizeof(char));

	strcpy( message, argv[1]);

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
		close( pipefd[0]);
		size = write( pipefd[1], message, max);
		wait( NULL);
		close( pipefd[1]);
		return 0;
	}
	else{	//child, read
		close( pipefd[1]);
		size = read( pipefd[0], buf, max);
		if(size != max){
			printf("pipe read error\n");
			exit(ECRPIPE);
		}
		spawn( buf);
		close( pipefd[0]);
		return 0;
	}
	return 0;
}

char ** split_string( char * str) 
{
	int i, j, cnt;
	char ** out;
	out = (char **)malloc( ARGLEN * sizeof(char *) );
	cnt = i = j = 0;
	while(1){
		//printf("str[%d]=%d\n",cnt,str[cnt]);
		if(str[cnt] == 0)
			break;
		out[i] = (char *)malloc( NAMELEN * sizeof(char) );
		
		while(isspace(str[cnt]))
			cnt++;
		for( j = 0 ; str[cnt]!=0 && !isspace(str[cnt]) && j < NAMELEN; cnt++, j++)
			out[i][j] = str[cnt];
		if(str[cnt-1] == 0)
			break;
		out[i][j] = 0;

		//printf("i=%d: %s\n",i,out[i]);
		i++;
	}
	return out;
}

void spawn( char * program)
{
	pid_t chpid;

	char ** arglist = split_string(program);

//	while(arglist[i]){
//		printf("arglist [%d] = %s\n",i,arglist[i]);
//		i++;
//	}
	
	chpid = fork();
	if(chpid)
		return;
	else{		
		if(execvp( arglist[0], arglist)){ // on error
			fprintf (stderr,"can't execute: %s\n",arglist[0]);
			exit(10);
		}
	}
}
