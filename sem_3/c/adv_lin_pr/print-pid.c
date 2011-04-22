#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	printf("this process id is %d\n",(int)getpid());	
	printf("this parent process id is %d\n",(int)getppid());	
	return 0;
}
