// Here we try to send integer from one process to another 
// using signals SIGUSR1 and SIGUSR2. (u1 and u2 below)
// Example: receiving u1, u2, u1 equals 010. = "2" =)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#define INT_SIZE 8

int number;
int cnt;
int is_receive_all;

void handle_bit ( int signal);
void send_int (int num, pid_t pid)
{
	int i, tmp;
	for ( i = INT_SIZE - 1; i >= 0; i-- ){
		tmp = num >> i;
		tmp = tmp & 01; //get bit;
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
void set_handlers()
{
	struct sigaction act;
	memset ( &act, 0, sizeof(struct sigaction));
	act.sa_handler = &handle_bit;
	sigaction ( SIGUSR1 ,&act, NULL);
	sigaction ( SIGUSR2 ,&act, NULL);

}

void handle_bit ( int signal)
{
	printf( "receive # %d\n", cnt);
	int tmp;
	if ( signal == SIGUSR1){
		printf("get sigusr1\n");
		tmp = 0;	
	}
	else{
		printf("get sigusr2\n");
		tmp = 1;
	}
	tmp = tmp << cnt;
	number = number | tmp;
	printf( "received %0d, num = %0d\n", tmp, number);
	if ( cnt == 0){
		is_receive_all = 1;
		return;
	}
	cnt--;
	set_handlers();
}


void receive_int ()
{
	number = 0;
	cnt = INT_SIZE - 1;
	is_receive_all = 0;


	set_handlers();
	while ( !is_receive_all); //wait for int to receive;
	//sleep( 10);
}

int main (int argc, char *argv[] )
{
	if ( argc != 1){
		printf( "usage: %s\n", argv[0]);
		return 1;
	}
	printf("i am %d\n", getpid());
	receive_int();
	int n = number;
	printf("received %d\n", n);
	return 0;
}
