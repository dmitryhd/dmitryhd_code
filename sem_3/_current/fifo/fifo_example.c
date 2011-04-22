#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <ctype.h>
#include <fcntl.h>

#define ECRPIPE 10 //pipe creation error
#define EFORK 11
#define ARGLEN 50
#define NAMELEN 50

int main( int argc, char ** argv)
{
	pid_t cpid;
	if( argc < 2){
		printf("usage: %s program\n",argv[0]);
		exit(1);
	}
	char * fifo_path = argv[1];
	char data[] = "abacaba";
	int data_length = strlen( data);
	int fd = open ( fifo_path, O_WRONLY );
	write (fd, data, data_length);
	close (fd);

	return 0;
}
