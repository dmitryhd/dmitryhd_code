#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	pid_t pid = getpid();
	pid_t ppid = getppid();
	printf("pid - %d, ppid - %d\n",pid,ppid);
	return 0;
}
