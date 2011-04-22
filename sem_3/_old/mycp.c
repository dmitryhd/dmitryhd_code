/* TITLE: mycp */
/* Short description: just my variant of unix's cp util with
 * -v,i,p,f options */
/* Last modification: DmitryKh - 19.09.09*/
/* Written by Dmitry Khodakov gr. 817*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
		else{
			param[par_cnt++] = argv[i];
		}
	}
	char * dst_name = NULL, * src_name = NULL;
	src_name = params[0];
	dst_name = params[1];
	fprintf(stderr,"%s to %s\n",src_name,dst_name);
	FILE * src_fd = NULL, *	dst_fd = NULL;
	if((src_fd=fopen(src_name,"r")) == NULL){
		perror("can't open source file");
		exit(EFILE);
	}
	/* Test if destination file already exist */ 
	if((dst_fd=fopen(dst_name,"r")) == NULL){
		fclose(dst_fd);
		printf("destination %s - already exist\n",dst_name);
	}

	if((dst_fd=fopen(dst_name,"w")) == NULL){
		perror("can't open source file");
		exit(EFILE);
	}
	/* Reading pieces from source */
	char buff[BUFFSIZE];
	int rd_cnt = 0, wr_cnt = 0;//number of read or write bytes
	do
	{
		rd_cnt = read(src_fd,buff,BUFFSIZE);
		wr_cnt = write(dst_fd,buff,rd_cnt);
	}
	while(rd_cnt == BUFFSIZE);

	/* Close all descritpors */
	fclose(dst_fd);
	fclose(dst_fd);
		
	return 0;
}
