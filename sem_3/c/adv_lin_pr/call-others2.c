#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	pid_t child_pid;
	int i;
	printf("I am first process! my pid is - %d\n",(int)getpid());
	for(i=0;i<100;i++){
		child_pid=fork();		
		if(child_pid == 0)
			//printf("I am %d child. My father is %d, my pid is - %d\n",i,(int)getppid(),(int)getpid());
		if(child_pid == -1){
			//printf("Can't create process\n");
			//exit(1);
		}
	}
	system("ps -Af");
	return 0;
}
