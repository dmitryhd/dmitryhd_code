/*Run jobs with definite num. of seconds since run useless*/
/*usage: prog joblibst*/
/*joblist format: <latency in seconds> <programm with arguments>*/
/*! jobs aren't sort by time*/
/********************************************
 * Напишите программу useless (UNIX SYSTEM EXTREMELY LATE EXECUTION 
 * SOFTWARE SYSTEM), которая читает файл и запускает указанные в нем 
 * программы с указанной задержкой от времени старта программы 
 * useless. Формат записи в файле:
 * <время задержки в секундах> <программа для выполнения>
 * Файл не является упорядоченным по временам задержки.
 * ******************************************/
#include "template.h"
#include <stdio.h>
#define COMM_LEN 300
#define MAX_JOB 100

typedef struct _joblist joblist;
struct _joblist{
	joblist * next;
	int sec;
	char *command;
};
joblist jobs[MAX_JOB];
int cnt;
//joblist * head, * tmp, *cur, *pr;
void add_job(char *comm, int sec);
void print_job();
void exec_job();
int cmp(const void *a, const void *b){
	return ((joblist *) a)->sec - ((joblist * )b)->sec;
}

int main(int argc, char *argv[]){
	if(argc!=2){
		printf("usage: prog joblibst\n");	
		printf("Run jobs with definite num. of seconds since run useless\n");
		return EARG;
	}
	FILE *f;
	openf(f,argv[1],"r");
	int sec;
 	char * comm = NULL;
	size_t len = 0;
	while(fscanf(f,"%d ",&sec)==1){
		getline(&comm, &len, f);
		add_job(comm,sec);
	}
	fclose(f);
	qsort(jobs,cnt,sizeof(joblist),cmp); 
	print_job();
	exec_job();
	return SUCC;
}
void print_job(){
/*
 	joblist * cur=head;
	while(cur!=NULL){
		printf("%d %s\n",cur->sec, cur->command);
		cur=cur->next;
	}
*/
	int i;
	printf("+++++++print jobs++++++++\n");
	for(i=0;i<cnt;i++){
		printf("%d %s\n",jobs[i].sec, jobs[i].command);
	}
	printf("+++++++++++++++++++++++++\n");
}
void exec_job(){
	int i, cur_sec=0,tmp_sec;
	for(i=0;i<cnt;i++){
		tmp_sec=jobs[i].sec-cur_sec;
		while((tmp_sec--)!=0){
			sleep(1);
			cur_sec++;
		}
		printf("sec %d\n",cur_sec);
		system(jobs[i].command);
	}
	
}
void add_job(char *comm, int sec){
	printf("add job %d %s\n",sec,comm);
/*	
 	alloc_mem(cur,joblist,1);
	cur->sec=sec;
	cur->command=comm;	

	tmp=head;
	pr=head;
	if(tmp==NULL){
		head=cur;
		head->next=NULL;
		print_job();
		return;
	}
	while(tmp!=NULL && tmp->sec <= sec){
		pr=tmp;
		tmp=tmp->next;
	}
	if(tmp!=head)
		cur->next=tmp;		
	else
		cur->next=NULL;
	pr->next=cur;
	print_job();
*/
	int l=strlen(comm);
	
	alloc_mem(jobs[cnt].command,char,l+1);
	strcpy(jobs[cnt].command,comm);
	jobs[cnt++].sec=sec;

}
