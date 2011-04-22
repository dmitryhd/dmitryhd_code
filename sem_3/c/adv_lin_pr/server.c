#include "template.h"
// just a simple server (use with client.c)
// illustration of sockets usage
int read_data(int client_sock);
int main(int argc, char *argv[]){
	const char* const socket_name = argv[1];
	int socket_fd;
	struct sockaddr name;
	int client_quit;


	//create socket
	socket_fd = socket(PF_LOCAL,SOCK_STREAM,0);
	if(socket_fd < 0){
		perror("socket() failed");
		exit(ESOCK);
	}
	//kill "already in use"
	int tr = 1;
	if(setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) < 0){
		perror("setsockopt");
	}
	name.sa_family = AF_LOCAL;
	strcpy (name.sa_data,socket_name);
	if(bind(socket_fd,&name,strlen(name.sa_data)) < 0){
		perror("bind failed");
		exit(EBIND);
	}
	if(listen(socket_fd,5) < 0){
		perror("listen failed");
		exit(EBIND);
	}

	do{
		struct sockaddr client_name;
		socklen_t client_name_len;
		int client_sock_fd;

		//accept connection
		client_sock_fd = accept(socket_fd,&client_name,&client_name_len);
		if(client_sock_fd < 0){
			perror("accept failed");
			exit(EBIND);
		}
		//handle connection
		client_quit = read_data(client_sock_fd);
		//printf("qq = %d\n",client_quit);
		close(client_sock_fd);
	}
	while(!client_quit);
	// Remove the socket file.
	close (socket_fd);
	if(unlink ("/tmp/sock") < 0){
		perror("unlink failed");
		exit(1);
	}
	return 0;
}
/* Read text from the socket and print it out. Continue until the
   socket closes. Return nonzero if the client sent a “quit”
   message, zero otherwise. */
int read_data(int client_sock){
	while(1){
		int len;	
		char *buf;
		if(read(client_sock,&len,sizeof(len)) == 0){
			//printf("still no valid data\n");
			return 0;
		}
		printf("read: len =%d \n",len);
		//alloc_mem(buf,char,len);
		buf = (char *)malloc(sizeof(char)*len);
		read(client_sock,buf,len);
		printf("read: %s\n",buf);
		if(!strcmp(buf,"exit"))
			return 1;
		free(buf);
	}
}
