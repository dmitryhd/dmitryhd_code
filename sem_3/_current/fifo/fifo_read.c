/*Written by Dmitry Khodakov - 817 gr. 09.2009.*/
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
#define BUFSIZE 1024

int main( int argc, char ** argv)
{
	int i = 0;
	if( argc < 2){
		printf("usage: %s program\n",argv[0]);
		exit(1);
	}
	char * fifo_path = argv[1];
	char buf[BUFSIZE];
	int fd = open ( fifo_path, O_RDONLY);
	read (fd, buf, BUFSIZE);
	while (buf[i] != 0)
		putchar(buf[i++]);
	putchar('\n');
	//printf("%s\n",buf);
	close (fd);
	if(unlink (fifo_path)){
		perror("can't delete fifo\n");
		return 1;
	}

	return 0;
}
