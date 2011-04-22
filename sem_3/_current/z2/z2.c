/* Title: task #2 */
/* Short description: --- */
/* Last modification: DmitryKh - 12.09.09*/
/* Written by Dmitry Khodakov gr. 817*/

/*************************************************
 * Программа должна брать из командной строки два параметра: 
 * исходную директорию и директорию назначения. Она должна 
 * рекурсивно сканировать исходную директорию, делать копии 
 * всех файлов, для которых ранее не делались копии или которые 
 * были изменены с момента последнего backup’а, размещая их в 
 * соответствующих местах директории назначения.  После 
 * копирования каждого файла должна вызываться команда сжатия 
 * gzip. Это уменьшит требуемый размер дисковой памяти; а файл 
 * будет переименован с добавлением расширения .gz. Все 
 * возникающие ошибки (нет исходной директории, файл закрыт 
 * для чтения и т.д.) должны корректно обрабатываться с 
 * выдачей соответствующего сообщения.
 *************************************************/

//can asyncronous clean up children
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define EOPEN 10
#define EARG 1
#define EDIR 11
#define ECOPY 12
#define EGZIP 13
#define BUFSIZE 255

void print_help(char *);
int spawn(char *prog,char **arg);
void backup_dir(char *sdir, char *ddir);
void clean_up_child(int signum);
int backup_file(char *spath, char *dpath);

int main(int argc, char *argv[])
{
	struct sigaction sigchild_act;
	memset(&sigchild_act,0,sizeof(struct sigaction));
	sigchild_act.sa_handler = &clean_up_child;
	sigaction(SIGCLD,&sigchild_act,NULL);
	if(argc != 3){
		print_help(argv[0]);
		return EARG;
	}
	backup_dir(argv[1],argv[2]);
	return 0;
}
void backup_dir(char *s, char *d)
{
	DIR *sd, *dd;
	struct dirent * sentry;
	if( (sd = opendir(s)) == NULL){
		perror("can't open directory\n");
		return;
	}
	if( (dd = opendir(d)) == NULL){
		char * arg_list[] = {"mkdir","-p",d,NULL };
		spawn("mkdir",arg_list);
	}
	char sfull[BUFSIZE]; 	//source directory
	char dfull[BUFSIZE];	//destination directory

	while((sentry = readdir(sd))){
		if(strcmp(sentry->d_name,".") == 0
			|| strcmp(sentry->d_name,"..") == 0)
			continue;
		strcpy(sfull,s);
		strcat(sfull,sentry->d_name);
		strcpy(dfull,d);
		strcat(dfull,sentry->d_name);
		if(opendir(sfull)){
			strcat(sfull,"/");
			strcat(dfull,"/");
			backup_dir(sfull,dfull);
			continue;
		}
		backup_file(sfull,dfull);
	}
}

int spawn(char *prog,char **arg_list)
{
	pid_t child_pid;
	child_pid = fork();
	if(child_pid != 0)
		return child_pid;
	else{
	    /* Now execute PROGRAM, searching for it in the path. */
	    execvp (prog, arg_list);
	    /* The execvp function returns only if an error occurs. */
	    perror ("an error occurred in execvp\n");
	    abort ();
	}
}

void print_help(char *pname)
{
	printf("Usage: %s [source directory] [destintaion directory]\n",pname);
}

int backup_file(char *spath, char *dpath)
{
	struct stat sstat;
	stat(spath,&sstat);

	char tmp[BUFSIZE];
	strcpy(tmp,dpath);
	strcat(tmp,".gz");

	struct stat dstat;
	stat(tmp,&dstat);
	if(sstat.st_mtime <= dstat.st_mtime)	//don't change
		return 1;
	char * arg_list[] = {"cp","-f",spath,dpath,NULL};
	spawn("cp",arg_list);
	char * arg_list2[] = {"gzip","-f",dpath,NULL};
	spawn("gzip",arg_list2);
	//----------------------------
	//it's piece of code which i decided not to use (yet)
	//-------
	//wait(&child_exit_status);
	//if(!WIFEXITED(child_exit_status))	//can't zip
	//	exit(EGZIP);
	//----------------------------
	return 0;
}
void clean_up_child(int signum)
{
	int stat;
	wait(&stat);
}
