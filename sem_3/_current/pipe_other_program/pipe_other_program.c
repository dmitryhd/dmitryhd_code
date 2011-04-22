/*Written by Dmitry Khodakov - 817 gr. 09.2009.*/
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
	int pipefd[2], i, size, max = 0;
	pid_t cpid;
	if( argc < 2){
		printf("usage: %s program\n",argv[0]);
		exit(1);
	}
	//here we form. all args to string
	for( i = 1; i<argc; i++)
		max += strlen(argv[i]) + 1;
	char * message = (char *)malloc( max * sizeof(char));
	message[0] = 0;
	char * buf = (char *)malloc( max * sizeof(char));
	for( i = 1; i<argc; i++){
		strcat(message,argv[i]);
		strcat(message," ");
	}
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
	//	printf( "buf: %s\n", buf);
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

		i++;
	}
	out[i-1] = NULL;
	return out;
}

void spawn( char * program)
{
	pid_t chpid;
	int i;
	char ** arglist = split_string(program);
	i = 0;
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
