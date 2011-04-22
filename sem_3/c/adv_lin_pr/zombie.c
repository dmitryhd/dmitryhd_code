#include <stdio.h>
#include <unistd.h>

void mkzombie();
int main(int argc, char *argv[]){
	int i=0;
	while(1){
		i++;
		printf("int %d\n",i);
		mkzombie();
	}
	return 0;
}
void mkzombie(){
	pid_t child_pid;
	child_pid = fork();
	if(child_pid == 0)
		return;
	else
		sleep(60);
}
