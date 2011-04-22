#include <stdio.h>

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("car: need arg\n");
		return 1;
	}
	printf("car %s\n",argv[1]);
	return 0;
}
