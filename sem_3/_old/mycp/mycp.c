/* TITLE: mycp */
/* Short description: just my variant of unix's cp util with
 * -v,i,p,f options */
/* Last modification: DmitryKh - 19.09.09*/
/* Written by Dmitry Khodakov gr. 817*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

/* ERRORS */
#define EFILE 10

/* named vars */
#define BUFFSIZE 255

int main( int argc, char *argv[])
{
	int opt, i;
	int par_cnt = 0;
	char *params[10];
	/* Options */
	int is_preserve = 0;
	int is_interactive = 0;
	int is_verbose = 0;
	int is_force = 0;
	/* Copy vars */
	char buff[BUFFSIZE];
	int rd_cnt = 0, wr_cnt = 0;//number of read or write bytes
	/* Parsing aguments */
	for( i=1;i<argc;i++){
		/* If current argument is option */
		if(argv[i][0] == '-'){
			optind = i ;
			opt=getopt(argc,argv,"ipfv");
			switch(opt){
				case 'p':
					is_preserve = 1;
					break;
				case 'i':
					is_interactive = 1;
					break;
				case 'v':
					is_verbose = 1;
					break;
				case 'f':
					is_force = 1;
					break;
			}
		}
//!!!
		/* If current argument is parameter */
		else
			params[par_cnt++] = argv[i];
	}
	par_cnt--;
	char * dst_name = NULL, * src_name = NULL;
	src_name = params[0];
	dst_name = params[par_cnt];
	int src_fd, dst_fd;
	if((src_fd = open(src_name,O_RDONLY)) < 0){
		perror("can't open source file");
		exit(EFILE);
	}
	DIR *dst_dir;
	struct dirent * sentry;

	if( par_cnt > 1 && (dst_dir = opendir(dst_name)) == NULL){
		printf("dirname = %s\n",dst_name);
		perror("can't open directory\n");
		exit(EFILE);
	}

	/* copy files to folder */ 
	char dst_path[BUFFSIZE];
	if(par_cnt > 1) {
		strcpy(dst_path,dst_name);
		strcat(dst_path,src_name);
		//printf("%s\n",dst_path);
		return 0;
	}
	/* copy file to file */ 
	/* Test if destination file already exist */ 
	if((dst_fd = open(dst_name,O_RDONLY)) >= 0){
		close(dst_fd);
		printf("destination %s - already exist\n",dst_name);
	}

	if((dst_fd = open(dst_name,O_WRONLY)) < 0){
		perror("can't open source file");
		exit(EFILE);
	}
	/* Reading pieces from source */
	do
	{
		rd_cnt = read((int)src_fd,buff,BUFFSIZE);
		printf("reading %s from src\n",buff);
		wr_cnt = write(dst_fd,buff,rd_cnt);
	}
	while(rd_cnt == BUFFSIZE);

	/* Close all descritpors */
	close(dst_fd);
	close(dst_fd);
		
	return 0;
}
