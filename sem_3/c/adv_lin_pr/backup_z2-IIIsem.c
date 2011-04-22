#include "template.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

/*Программа должна брать из командной строки два параметра: исходную директорию и директорию назначения. Она должна рекурсивно сканировать исходную директорию, делать копии всех файлов, для которых ранее не делались копии или которые были изменены с момента последнего backup’а, размещая их в соответствующих местах директории назначения.
После копирования каждого файла должна вызываться команда сжатия gzip. Это уменьшит требуемый размер дисковой памяти; а файл будет переименован с добавлением расширения .gz. Все возникающие ошибки (нет исходной директории, файл закрыт для чтения и т.д.) должны корректно обрабатываться с выдачей соответствующего сообщения.
*/
//can asyncronous clean up children
#define EOPEN 10
#define EDIR 11
#define ECOPY 12
#define EGZIP 13
#define BUFSIZE 255

sig_atomic_t child_exit_status;

void print_help(char *);
int spawn(char *prog,char **arg);
void backup_dir(char *sdir, char *ddir);
void clean_up_child(int signum);
int backup_file(char *spath, char *dpath);

int main(int argc, char *argv[]){
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
void backup_dir(char *s, char *d){
	DIR *sd, *dd;
	//int child_exit_status;
	struct dirent * sentry, * dentry;
	//printf("backup: %s %s\n",s,d);
	if( (sd = opendir(s)) == NULL){
		printf("error: source can't open directory %s\n",s);
		return;
	}
	if( (dd = opendir(d)) == NULL){
		char * arg_list[] = {
		"mkdir",    		
		"-p",
		d,
		NULL };
		spawn("mkdir",arg_list);
		//wait(&child_exit_status);
		//if(!WIFEXITED(child_exit_status))	//can't create dir
		//	exit(EDIR);
		//printf("error: dest can't open directory %s\n",d);
		//return;
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
			//printf("opendir %s\n",sfull);
			strcat(sfull,"/");
			strcat(dfull,"/");
			backup_dir(sfull,dfull);
			continue;
		}
		//if(denrtry = readdir(dd) &&
		//	(strcmp(sentry->d_name,dentry->d_name) == 0))
		//	continue;
		//printf("backup %s %s\n",sfull,dfull);
		backup_file(sfull,dfull);
	}
}
int spawn(char *prog,char **arg_list){
	pid_t child_pid;
	child_pid = fork();
	if(child_pid != 0)
		return child_pid;
	else{
	    /* Now execute PROGRAM, searching for it in the path. */
	    execvp (prog, arg_list);
	    /* The execvp function returns only if an error occurs. */
	    fprintf (stderr, "an error occurred in execvp\n");
	    abort ();
	}
}

void print_help(char *pname){
	printf("Usage: %s [source directory] [destintaion directory]\n",pname);
}
int backup_file(char *spath, char *dpath){
	struct stat sstat;
	//int child_exit_status;
	stat(spath,&sstat);
	//printf("%d - %s\n",sstat.st_mtime,spath);

	char tmp[BUFSIZE];
	strcpy(tmp,dpath);
	strcat(tmp,".gz");

	struct stat dstat;
	stat(tmp,&dstat);
	//printf("%d - %s\n",dstat.st_mtime,dpath);
	if(sstat.st_mtime <= dstat.st_mtime){
		printf("%s - don't change\n",spath);
		return 1;
	}
	char * arg_list[] = {
  	"cp",    /* argv[0], the name of the program. */
	"-f",
	spath,
	dpath,
   	NULL     /* The argument list must end with a NULL.*/
	};
	spawn("cp",arg_list);
	//wait(&child_exit_status);
	//if(!WIFEXITED(child_exit_status))	//can't cp
	//	exit(ECOPY);

	char * arg_list2[] = {
  	"gzip",    	
	"-f",
	dpath,
   	NULL     	
	};
	spawn("gzip",arg_list2);
	//wait(&child_exit_status);
	//if(!WIFEXITED(child_exit_status))	//can't zip
	//	exit(EGZIP);
	return 0;
}
void clean_up_child(int signum){
	int stat;
	wait(&stat);
	child_exit_status = stat;
}
