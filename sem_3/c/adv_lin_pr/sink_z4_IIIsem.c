#include "template.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
/*На мойке посуды в ресторане работают два человека. Один из них моет посуду, второй вытирает уже вымытую. Времена выполнения операций мытья и вытирания посуды меняются в зависимости от того, что моется. Стол для вымытой, но не вытертой посуды имеет ограниченные размеры.
Смоделируйте процесс работы персонала следующим образом: каждому работнику соответствует свой процесс. Времена выполнения операций содержатся в двух файлах. Каждый файл имеет формат записей:
	<тип посуды> : <время операции>
Стол вмещает N предметов независимо от их наименования. Значение N задается как параметр среды TABLE_LIMIT перед стартом процессов. Грязная посуда, поступающая на мойку, описывается файлом с форматом записи:
	<тип посуды> : <количество предметов>
Записи с одинаковым типом посуды могут встречаться неоднократно.
Организуйте передачу посуды от процесса к процессу:
А) через файл, используя семафоры для синхронизации;
Б) через pipe;
В) через сообщения;
Г) через разделяемую память;
Д) через sockets.
*/
//------------Core-----------
#define BUFSIZE 255
typedef struct _operation{	//храним посуду
	int time;
	char name[BUFSIZE];
} op;
op * wop, * cop;		//массивы посуды, которую могут мыть мойщик и вытиратель
int wcnt, ccnt;			//со временем работы
int tcnt;
FILE *tf;			//файл, содержащий грязную посуду
int get_time(op * o, int cnt, char *n);	//узнать, время проведения операции для мойщика 
					//или вытирателя
//------------pipes----------
void spawn_pipe();
//---------semaphores--------
void spawn_sem();
char sh_file[BUFSIZE] = "buf.tst";
//------------socket--------- 
void spawn_sock();
char socket_name[BUFSIZE];
void write_text (int socket_fd, const char* text);
int read_data(int client_sock, char *buf);

//------------Core-----------
int main(int argc, char *argv[]){
	if(argc != 4 && argc != 5){
		fprintf(stderr,"usage: %s washer_file cleaner_file tableware_file\n",argv[0]);
		exit(EARG);
	}	
	//считаем параметры работников из файлов.
	FILE *wf, *cf;
	openf(wf,argv[1],"r");
	wcnt = 1;
	wop = (op *)realloc(wop,wcnt*sizeof(op));
	while(fscanf(wf,"%s %d",wop[wcnt-1].name,&wop[wcnt-1].time) == 2){
		wcnt++;
		wop = (op *)realloc(wop,wcnt*sizeof(op));
	}
	wcnt--;
	fclose(wf);	
	openf(cf,argv[2],"r");
	ccnt = 1;
	cop = (op *)realloc(cop,ccnt*sizeof(op));
	int a;
	while((a=fscanf(cf,"%s %d\n",cop[ccnt-1].name,&cop[ccnt-1].time)) == 2){
		ccnt++;
		cop = (op *)realloc(cop,ccnt*sizeof(op));
	}
	ccnt--;
	fclose(cf);
	openf(tf,argv[3],"r");
	//начнем мойку
	strcpy(socket_name,argv[4]);
	spawn_sock();
	return 0;
}
int get_time(op * o, int cnt, char *n){
	int i;	
	for(i=0;i<cnt;i++)
		if(strcmp(o[i].name,n) == 0)
			return o[i].time;
	return -1;
}
//------------pipes----------
void spawn_pipe(){
	char name[BUFSIZE];
	int num, time, i;
	FILE *str;
	//open pipe
	int fds[2];
	pipe(fds);
	//split to workers
	pid_t child_p = fork();
	if(child_p == 0){	//washer - child
		//prepare pipe
		close(fds[0]);
		str = fdopen(fds[1],"w");
		//get tableware
		while(fscanf(tf,"%s %d",name,&num) == 2){
			time = get_time(wop,wcnt,name);
			printf("w: time for - %s, %d\n",name,time);
			if(time == -1){
				fprintf(stderr,"wrong name %s\n",name);
				continue;
			}
			printf("w: read: name = %s time = %d\n",name,time);
			for(i=0;i<num;i++){
				sleep(time);
				fprintf (str, "%s ", name);
				fflush (str);
			}
		}
		printf("washer exit\n");
		fprintf (str, "%s", "exit");
		fflush (str);
		close(fds[1]);
	}
	else{			//cleaner
		close(fds[1]);
		str = fdopen(fds[0],"r");
		while(1){
			fscanf(str,"%s",name);
			if(strcmp(name,"exit") == 0)
				break;
			time = get_time(cop,ccnt,name);
			if(time == -1){
				fprintf(stderr,"wrong name %s\n",name);
				continue;
			}
			printf("c: read: name = %s time = %d\n",name,time);
			sleep(time);
		}
		printf("cleaner exit\n");
		close(fds[0]);
	}
}
//---------semaphores--------
//semaphore union
union semun {
   	int val;
   	struct semid_ds *buf;
   	unsigned short int *array;
   	struct seminfo *__buf;
};
//allocate semaphore
int semaphore_allocation (key_t key, int sem_flags){
  	return semget (key, 1, sem_flags);
}
//Deallocate a binary semaphore. All users must have finished their
// use. Returns -1 on failure.
int semaphore_deallocate (int semid){
  	union semun ignored_argument;
  	return semctl (semid, 1, IPC_RMID, ignored_argument);
}
//Initialize a binary semaphore with a value of 0.
int semaphore_initialize (int semid){
  	union semun argument;
  	unsigned short values[1];
  	values[0] = 0;
  	argument.array = values;
  	return semctl (semid, 0, SETALL, argument);
}
// Wait on a semaphore.  Block until the semaphore value is positive, then
//   decrement it by 1.
int semaphore_wait (int semid){
  	struct sembuf operations[1];
  	/* Use the first (and only) semaphore.  */
  	operations[0].sem_num = 0;
  	/* Decrement by 1. */
  	operations[0].sem_op = -1;
  	/* Permit undo’ing. */
  	operations[0].sem_flg = SEM_UNDO;
  	return semop (semid, operations, 1);
}
/* Post to a binary semaphore: increment its value by 1.
   This returns immediately. */
int semaphore_post (int semid){
  	struct sembuf operations[1];
  	/* Use the first (and only) semaphore.  */
  	operations[0].sem_num = 0;
  	/* Increment by 1. */
  	operations[0].sem_op = 1;
  	/* Permit undo’ing. */
  	operations[0].sem_flg = SEM_UNDO;
  	return semop (semid, operations, 1);
}
void spawn_sem(){
	char name[BUFSIZE];
	int num, time, i;

	FILE *f;	//open\create file for communication
	openf(f,sh_file,"w");
	fclose(f);
	//create semaphore set
	int key = IPC_PRIVATE;
	int sem_id = semaphore_allocation (key, 0666);
	if(sem_id == -1){
		fprintf(stderr,"can't allocate semaphore with key %d\n",key);
		exit(ESOCK);
	}
	semaphore_initialize (sem_id);

	pid_t child_p = fork();
	if(child_p == 0){	//cleaner
		openf(f,sh_file,"r");
		while(1){
			//printf("cleaner:\n");
			if(semaphore_wait (sem_id) == -1){
				fprintf(stderr,"can't wait semaphore with id %d\n",sem_id);
				exit(ESOCK);
			}
			//printf("cleaner get sem:\n");

			fscanf(f,"%s ",name);
			if(strcmp(name,"exit") == 0)
				break;
			time = get_time(cop,ccnt,name);
			if(time == -1){
				fprintf(stderr,"wrong name %s\n",name);
				continue;
			}
			printf("c: read: name = %s time = %d\n",name,time);
			sleep(time);
		}
		printf("cleaner exit\n");
		semaphore_deallocate (sem_id);
	}
	else{			//washer
		printf("w: sem id = %d\n",sem_id);
		openf(f,sh_file,"w");
		while(fscanf(tf,"%s %d",name,&num) == 2){
			time = get_time(wop,wcnt,name);
			printf("w: read: name = %s cnt = %d time = %d\n",name,num,time);
			if(time == -1){
				fprintf(stderr,"wrong name %s\n",name);
				continue;
			}
			for(i=0;i<num;i++){
				sleep(time);
				printf("w: write to file %s\n",name);
				fprintf(f,"%s ",name);
				fflush(f);
				semaphore_post (sem_id);
			}
		}
		fprintf(f,"exit ");
		fflush(f);
		semaphore_post (sem_id);
		printf("washer exit\n");
		//wait for child. (important)
		int child_stat;
		wait(&child_stat);
	}
}

//---------sockets--------
int read_data(int client_sock, char *buf){
	while(1){
		int len;	
		if(read(client_sock,&len,sizeof(len)) == 0){
			//printf("still no valid data\n");
			return 0;
		}
		printf("read: len =%d \n",len);
		//alloc_mem(buf,char,len);
		//buf = (char *)malloc(sizeof(char)*len);
		read(client_sock,buf,len);
		printf("read: %s\n",buf);
		if(!strcmp(buf,"exit"))
			return 1;
		//free(buf);
	}
}
/* Write TEXT to the socket given by file descriptor SOCKET_FD. */
void write_text (int socket_fd, const char* text){
  	/* Write the number of bytes in the string, including
     	NUL-termination. */
  	int length = strlen (text) + 1;
  	write (socket_fd, &length, sizeof (length));
  	/* Write the string. */
  	write (socket_fd, text, length);
}
//don't work!!! access error from clener - server
void spawn_sock(){
	char name[BUFSIZE];
	int num, time, i;
	pid_t child_p = fork();
	if(child_p == 0){	//cleaner
		//server
		int rsv_socket, client_quit;
		struct sockaddr rsv_name;
		//create socket
		rsv_socket = socket(PF_LOCAL,SOCK_STREAM,0);
		if(rsv_socket < 0){
			perror("socket() failed");
			close (rsv_socket);
			unlink (socket_name);
			exit(ESOCK);
		}
		rsv_name.sa_family = AF_LOCAL;
		strcpy (rsv_name.sa_data,socket_name);

		if(bind(rsv_socket,&rsv_name,strlen(rsv_name.sa_data)) < 0){
			perror("bind failed");
			close (rsv_socket);
			unlink (socket_name);
			exit(EBIND);
		}
		if(listen(rsv_socket,5) < 0){
			perror("listen failed");
			close (rsv_socket);
			unlink (socket_name);
			exit(EBIND);
		}
		printf("c: sock create %d\n",rsv_socket);
		do{
			struct sockaddr client_name;
			socklen_t client_name_len;
			int client_sock_fd;
			//accept connection
			client_sock_fd = accept(rsv_socket,&client_name,&client_name_len);
			if(client_sock_fd < 0){
				perror("accept failed");
				close (rsv_socket);
				unlink (socket_name);
				exit(EBIND);
			}
			//handle connection
			client_quit = read_data(client_sock_fd,name);
			close(client_sock_fd);
			time = get_time(cop,ccnt,name);
			if(time == -1){
				fprintf(stderr,"wrong name %s\n",name);
				continue;
			}
			printf("c: read: name = %s time = %d\n",name,time);
			sleep(time);
		}
		while(!client_quit);
		close (rsv_socket);
		if(unlink (socket_name) < 0){
			perror("unlink failed");
			close (rsv_socket);
			unlink (socket_name);
			exit(1);
		}
		printf("cleaner exit\n");
	}
	else{			//washer
		/* Connect the socket. */
		int send_sock;
		struct sockaddr send_name;
		/* Create the socket. */
		send_sock = socket (PF_LOCAL, SOCK_STREAM, 0);
		if(send_sock < 0){
			printf("socket() failed\n");
			exit(ESOCK);
		}
		/* Store the server’s name in the socket address. */
		send_name.sa_family = AF_LOCAL;
		strcpy (send_name.sa_data, socket_name);
		/* Connect the socket. */
		sleep(1);
		printf("w: try connect %d\n",send_sock);
		if(connect (send_sock, &send_name, strlen(send_name.sa_data)) < 0){
			perror("connect");
			close (send_sock);
			unlink (socket_name);
			exit(ECONN);
		}
		while(fscanf(tf,"%s %d",name,&num) == 2){
			time = get_time(wop,wcnt,name);
			if(time == -1){
				fprintf(stderr,"wrong name %s\n",name);
				continue;
			}
			printf("w: read: name = %s cnt = %d time = %d\n",name,num,time);
			for(i=0;i<num;i++){
				sleep(time);
				printf("w: write to socket %s\n",name);
  				write_text (send_sock, name);
			}
		}
  		write_text (send_sock, "exit ");
		printf("washer exit\n");
  		close (send_sock);
		int child_stat;
		wait(&child_stat);
	}
}

