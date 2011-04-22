#include <stdio.h>
#include <unistd.h>

int main( int argc, char *argv[], char *env[])
{
	int i;
	for( i=1;i<argc;i++)
		printf("%s ",argv[i]);	
	printf("\nenvironment:\n");
	for( i=0; env[i] ;i++)
		printf("%s\n",env[i]);	
	return 0;
}
