#include <stdio.h>

#define SEC 0
#define MAIN 1

void spawn_car( char * program, int n);

int main()
{
	spawn_car("car",SEC);
	return 0;	
}

void spawn_car( char * program, int n)
{
	pid_t chpid;
	char * arg[2];
	if( n == SEC)
		strcat( arg, " S");	
	if( n == MAIN)
		strcat( arg, " M");	
	char * arglist[] = { program, arg, NULL};

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
