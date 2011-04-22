#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define ARGLEN 50
#define NAMELEN 50

void spawn( char * program);

int main( int argc, char **argv )
{
	spawn ( "ls -l -r -a");
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
//	int i = 0;
//	printf("prog: %s\n",program);

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
