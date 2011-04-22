// Here we try to send integer from one process to another 
// using signals SIGUSR1 and SIGUSR2. (u1 and u2 below)
// Example: receiving u1, u2, u1 equals 010. = "2" =)

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#define INT_SIZE 8

int number;
int cnt;
int is_receive_all;

void send_int (int num, pid_t pid)
{
	int i, tmp;
	for ( i = INT_SIZE - 1; i >= 0; i-- ){
		tmp = num >> i;
		tmp = tmp & 01; //get bit;
		printf("tmp =%d\n", tmp);
		if ( tmp == 0){
			if( kill ( pid, SIGUSR1) < 0){
				perror ( "send error");
				exit(1);
			}
		}
		else{
			if( kill ( pid, SIGUSR2) < 0){
				perror ( "send error");
				exit(1);
			}
		}
	}
}

void handle_bit ( int signal)
{
	int tmp;
	if ( signal == SIGUSR1){
		tmp = 0;	
	}
	else
		tmp = 1;
	tmp = tmp << cnt;
	number = number | tmp;
	if ( cnt == 0){
		is_receive_all = 1;
		return;
	}
	cnt--;
}

void receive_int ()
{
	number = 0;
	cnt = INT_SIZE - 1;
	is_receive_all = 0;
	signal ( SIGUSR1, handle_bit );
	signal ( SIGUSR2, handle_bit );
	while ( !is_receive_all); //wait for int to receive;
	
}

int main (int argc, char *argv[] )
{
	if ( argc != 3){
		printf( "usage: %s pid int\n", argv[0]);
		return 1;
	}
	pid_t pid = (pid_t) atoi( argv[1]);
	int n = atoi ( argv[2]);
	printf("i %d, send %d to %d\n", getpid(), n, pid);
	send_int ( n, pid);
	return 0;
}
