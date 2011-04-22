#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>
/*Напишите программу runsim, осуществляющую контроль количества одновременно работающих UNIX приложений. Программа читает UNIX команду со стандартного ввода и запускает ее на выполнение. Количество одновременно работающих команд не должно превышать N, где N – параметр командной строки при запуске runsim. При попытке запустить более чем N приложений выдайте сообщение об ошибке. Программа runsim должна прекращать свою работу при возникновении признака конца файла на стандартном вводе
*/
#define BUFSIZE 255
#define MAXARG 10
#define EARG 10
#define EMAXCOMM 11
#define SUCC 0

int end_flag;
sig_atomic_t child_cnt;
int max_child;

void clean_up_child(int signum);
void run_comm(char *comm);

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("usage: %s N\n",argv[0]);
		exit(EARG);
	}
	max_child = atoi(argv[1]);
	struct sigaction sigchild_act;
	memset(&sigchild_act,0,sizeof(struct sigaction));
	sigchild_act.sa_handler = &clean_up_child;
	sigaction(SIGCLD,&sigchild_act,NULL);
	char *comm = NULL;
	comm = (char *)malloc(BUFSIZE*sizeof(char));
	int c, i;
	while(1){
	//омг, вот тут какая-та хренотень.
		c = getchar();
	//	if(c == EOF)
	//		break;
		ungetc(c,stdin);
		
		comm = fgets(comm,BUFSIZE,stdin);
		printf("%s\n",comm);
		if(comm == NULL)
			break;
		if(comm[0] == '\n')
			break;
		
		/*
		i = 0;
		while((c = getchar()) != 0)
			else
				comm[i++] = c;
		comm[i] = 0;
		*/
	//конец хренотени.
	//выход при вводе пустой строки
		run_comm(comm);

	}
	return SUCC;
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
		exit(EMAXCOMM);
	}
	printf("run #%d : ",child_cnt);
	for(i=0;i<cnt;i++)
		printf("%s ",arglist[i]);
	printf("\n");
	pid_t child_pid = fork();
	if(child_pid == 0){
		execvp(arglist[0],&(arglist[0]));
		fprintf(stderr,"can't exec %s , #%d\n",arglist[0],child_cnt);
		child_cnt--;
	}
}
void clean_up_child(int signum){
	int stat;
	wait(&stat);
	child_cnt--;
}
