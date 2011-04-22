#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int spawn(char * program, char **arglist){
	pid_t cpid;
	cpid=fork();
	if(cpid !=0){
		printf("parent process\n");
		return cpid;
	}
	else
		execvp(program,arglist);
}

int main(int argc, char *argv[]){
	char *arglist[]={	
		"ps",
		"-Af",
		NULL
	};
	spawn("ps",arglist);
	return 0;
}
