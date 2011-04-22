#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

/*Напишите программу runsim, осуществляющую контроль количества одновременно работающих UNIX приложений. Программа читает UNIX команду со стандартного ввода и запускает ее на выполнение. Количество одновременно работающих команд не должно превышать N, где N – параметр командной строки при запуске runsim. При попытке запустить более чем N приложений выдайте сообщение об ошибке. Программа runsim должна прекращать свою работу при возникновении признака конца файла на стандартном вводе
*/

/*Written by Dmitry Khodakov - 817 gr. 09.2009.*/

#define BUFSIZE 255
#define MAXARG 10
#define EARG 10
#define EMAXCOMM 11
#define SUCC 0
#define ARGLEN 50
#define NAMELEN 50

sig_atomic_t child_cnt;
int max_child;

void clean_up_child(int signum);
void run_comm(char *comm);
void spawn( char * program);

int main(int argc, char *argv[]){
	int c, i;

	if(argc != 2){
		printf("usage: %s N\n",argv[0]);
		exit(EARG);
	}

	max_child = atoi(argv[1]);

	struct sigaction sigchild_act;
	memset(&sigchild_act,0,sizeof(struct sigaction));
	sigchild_act.sa_handler = &clean_up_child;
	sigchild_act.sa_flags = SA_NOCLDSTOP & SA_NOCLDWAIT;
	if(sigaction(SIGCLD,&sigchild_act,NULL))
		perror("sigaction error");
	
	char *comm = NULL;
	comm = (char *)malloc(BUFSIZE*sizeof(char));

	while(1){
		c = getchar();
		if(c == EOF)
			return SUCC;
		ungetc(c,stdin);
		
		comm = fgets(comm,BUFSIZE,stdin);
		if(comm == NULL || comm[0] == '\n')
			break;
		spawn( comm);
		//run_comm(comm);
	}
	return SUCC;
}
char ** split_string( char * str) 
{
	int i, j, cnt;
	char ** out;
	out = (char **)malloc( ARGLEN * sizeof(char *) );
	cnt = i = j = 0;
	while(1){
		//printf("str[%d]=%d\n",cnt,str[cnt]);
		if(str[cnt] == 0)
			break;
		out[i] = (char *)malloc( NAMELEN * sizeof(char) );
		
		while(isspace(str[cnt]))
			cnt++;
		for( j = 0 ; str[cnt]!=0 && !isspace(str[cnt]) && j < NAMELEN; cnt++, j++)
			out[i][j] = str[cnt];
		if(str[cnt-1] == 0)
			break;
		out[i][j] = 0;

		//printf("i=%d: %s\n",i,out[i]);
		i++;
	}
	out[i-1] = NULL;
	return out;
}

void spawn( char * program)
{
	pid_t chpid;
	int i;
	char ** arglist = split_string(program);
	//printf("split:\n");
	i = 0;
	/*
	while(arglist[i]){
		printf("arglist [%d] = %s\n",i,arglist[i]);
		i++;
	}
	*/

	chpid = fork();
	if(chpid)
		return;
	else{		
		if(execvp( arglist[0], arglist)){ // on error
			fprintf (stderr,"can't execute: %s\n",arglist[0]);
			exit(10);
		}
	}
}
void clean_up_child(int signum){
	int stat;
	wait(&stat);
	printf("[exit proc]\n");
	child_cnt--;
}
void run_comm(char *comm){
	int i,j, cnt = 0, flag = 1;
	char ** arglist = NULL;
	arglist = (char **)malloc(MAXARG*sizeof(char *));
	arglist[0] = NULL;
	arglist[0] = (char *)malloc(BUFSIZE*sizeof(char));
	for(i=0,j=0;i<BUFSIZE;i++){
		if(isspace(comm[i])){
			if(flag != 1){
				arglist[cnt][j] = 0;
				cnt++;
				arglist[cnt] = NULL;
				arglist[cnt] = (char *)malloc(BUFSIZE*sizeof(char));
				j = 0;
			}
			flag = 1;
		}
		else{
			flag = 0;
			arglist[cnt][j++] = comm[i];
		}
	}
	arglist[cnt] = NULL;
	child_cnt++;
	if(child_cnt > max_child){
		fprintf(stderr,"reached limit of running commands : %d\n",max_child);
		child_cnt--;
		return;
	}
	//printf("run #%d : ",child_cnt);
	//for(i=0;i<cnt;i++)
	//	printf("%s ",arglist[i]);
	//printf("\n");
	pid_t child_pid = fork();
	if(child_pid == 0){
		execvp(arglist[0],&(arglist[0]));
		fprintf(stderr,"can't exec %s , #%d\n",arglist[0],child_cnt);
		child_cnt--;
	}
	else
		return;
}
