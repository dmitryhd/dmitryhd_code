#include "template.h"

/* Write TEXT to the socket given by file descriptor SOCKET_FD. */
void write_text (int socket_fd, const char* text){
  	/* Write the number of bytes in the string, including
     	NUL-termination. */
  	int length = strlen (text) + 1;
  	write (socket_fd, &length, sizeof (length));
  	/* Write the string. */
  	write (socket_fd, text, length);
	printf("len = %d, mess = %s\n",length,text);
	//fflush(&socket_fd);
}
int main (int argc, char* const argv[])
{
  	const char* const socket_name = argv[1];
  	const char* const message = argv[2];
  	int socket_fd;
  	struct sockaddr name;
  	/* Create the socket. */
  	socket_fd = socket (PF_LOCAL, SOCK_STREAM, 0);
	if(socket_fd < 0){
		printf("socket() failed\n");
		exit(ESOCK);
	}
  	/* Store the server’s name in the socket address. */
  	name.sa_family = AF_LOCAL;
  	strcpy (name.sa_data, socket_name);
  	/* Connect the socket. */
  	if(connect (socket_fd, &name, strlen(name.sa_data)) < 0){
		printf("connect failed\n");
		exit(ECONN);
	}
  	/* Write the text on the command line to the socket. */
  	write_text (socket_fd, message);
  	close (socket_fd);
  	return 0;
}

