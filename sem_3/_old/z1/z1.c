/* Title: task #1 */
/* Short description: --- */
/* usage: prog joblibst */
/* Last modification: DmitryKh - 12.09.09*/
/* Written by Dmitry Khodakov gr. 817*/

/* I hope, it's clear code =) */

/*Run jobs with definite num. of seconds since run useless*/
/*joblist format: <latency in seconds> <programm with arguments>*/

//Task:
/********************************************
 * Напишите программу useless (UNIX SYSTEM EXTREMELY LATE EXECUTION 
 * SOFTWARE SYSTEM), которая читает файл и запускает указанные в нем 
 * программы с указанной задержкой от времени старта программы 
 * useless. Формат записи в файле:
 * <время задержки в секундах> <программа для выполнения>
 * Файл не является упорядоченным по временам задержки.
 * ******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define _GNU_SOURCE
#define COMM_LEN 300
#define MAX_JOB 100

#define EARG 1
#define SUCC 0

#define ARGLEN 50
#define NAMELEN 50

#define openf(f,name,mode)\
do {\
	if((f=fopen(name,mode))==NULL){\
		fprintf(stderr,"file error\n");\
		exit(1);\
	}\
}\
while (0);

#define alloc_mem(a,type,num)\
do {\
	if((a=(type *)realloc(a,(num)*sizeof(type)))==NULL){\
		fprintf(stderr,"mem err\n");\
		exit(2);\
	}\
}\
while (0);

typedef struct _joblist joblist;
struct _joblist{
	joblist * next;
	int sec;
	char *command;
};
joblist jobs[MAX_JOB];
int cnt;

void add_job(char *comm, int sec);
void print_job();
void exec_job();
void spawn(char * program);
int cmp(const void *a, const void *b)
{
	return ((joblist *) a)->sec - ((joblist * )b)->sec;
}

int main(int argc, char *argv[])
{
	FILE *f;
	int sec;
 	char * comm = NULL;
	size_t len = 0;
	if(argc!=2){
		printf("usage: prog joblibst\n");	
		printf("Run jobs with definite num. of seconds since run useless\n");
		return EARG;
	}
	openf(f,argv[1],"r");
	while(fscanf(f,"%d ",&sec)==1){
		getline(&comm, &len, f);
		add_job(comm,sec);
	}
	fclose(f);
	qsort(jobs,cnt,sizeof(joblist),cmp); 
	exec_job();
	return SUCC;
}
void print_job()//debug func.
{
	int i;
	printf("print jobs:\n");
	for(i=0;i<cnt;i++)
		printf("%d %s\n",jobs[i].sec, jobs[i].command);
	printf("+++++++++++++++++++++++++\n");
}
void exec_job()	
{
	int i, cur_sec=0,tmp_sec;
	for(i=0;i<cnt;i++){
		tmp_sec=jobs[i].sec-cur_sec;
		while((tmp_sec--)!=0){
			sleep(1);
			cur_sec++;
		}
		system(jobs[i].command);
	}
}
void add_job(char *comm, int sec)
{
	int l=strlen(comm);
	alloc_mem(jobs[cnt].command,char,l+1);
	strncpy(jobs[cnt].command,comm,l+1);
	jobs[cnt++].sec=sec;

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
	out[i] = NULL;
	return out;
}

void spawn( char * program)
{
	pid_t chpid;
//	int i = 0;
//	printf("prog: %s\n",program);

	char ** arglist = split_string(program);

//	while(arglist[i]){
//		printf("arglist [%d] = %s\n",i,arglist[i]);
//		i++;
//	}
	
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
