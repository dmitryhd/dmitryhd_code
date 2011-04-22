#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	printf("run pid %d\n",getppid());
	sleep(5);
	return 0;
}
