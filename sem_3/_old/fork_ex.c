#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char *argv[])
{
	pid_t chpid;
	char *arglist[]={	
		"ps",
		"-f",
		NULL
	};
	chpid = fork();
	if(chpid == 0){
		printf("i am child\n");
		execvp("ps",arglist);
	}
	else
		printf("i am parent\n");
	return 0;
}
