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
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>

/* named vars */
#define BUFFSIZE 255
// last!! -> preserve

int is_preserve = 0;
int is_interactive = 0;
int is_verbose = 0;
int is_force = 0;
int is_link = 0;
int is_follow_link = 0;
int is_recursive = 0;
int is_preserve = 0;

int is_dir( char *name);
void copy_file ( char *dst_name, char *src_name);
void copy_dir (char * dst_name, char * src_name);
void print_help();
int is_name_like_dir(char *name);
int remove_last_slash(char *name);
int add_last_slash(char *name);
int is_file_exist ( char *fname);
int is_file_link ( char *fname);

int main( int argc, char *argv[])
{
	int par_cnt = 0;
	char *params[10];
	/* Options */
	/* Parsing aguments */
	int option_index = 0, c;
	struct option long_options[] = {
		{"verbose",	0,	0,	'v'},
		{"recursive",	0,	0,	'r'},
		{"help",	0,	0,	'h'},
		{"force",	0,	0,	'f'},
		{"preserve",	0,	0,	'p'},
		{"interactive",	0,	0,	'i'},
		{"link",	0,	0,	'l'},
		{"dereference",	0,	0,	'L'},
		{"no-dereference",	0,	0,	'P'},
		{NULL,		0,	0,	0}
	};
	while(1){
		c = getopt_long ( argc, argv, "vhrfilLP", long_options, &option_index );
		if ( c == -1)
			break;
		switch ( c) {
			case 'v': is_verbose = 1; break;
			case 'f': is_force = 1; break;
			case 'h': print_help(); return 0;
			case 'r': is_recursive = 1; break;
			case 'i': is_force = 0; break;
			case 'l': is_link = 1; break;
			case 'L': is_follow_link = 1; break;
			case 'P': is_follow_link = 0; break;
			case 'p': is_preserve = 1; break;
			default: break;
		}
	}
	while( optind < argc)
		params[ par_cnt++ ] = argv[ optind++ ];
	par_cnt--;

	if ( par_cnt < 1){
		printf("need 2+ args\n");
		return 0;
	}
	char * dst_name = NULL, * src_name = NULL;
	src_name = params[0];
	dst_name = params[par_cnt];
	if( is_name_like_dir ( dst_name))
		mkdir ( dst_name, 0777);
	if( is_dir ( dst_name) ){
		int i;
		char dst_path[BUFFSIZE];
		for ( i = 0; i < par_cnt; i++ ){
			if ( is_dir ( params[i]) ){
				if( is_recursive ){
				    add_last_slash( dst_name);
				    add_last_slash( params[i]);
							copy_dir ( dst_name, params[i]);
				}
				else
					printf("%s is a dir\n", params[i]);
			}
			else{
				strcpy ( dst_path, dst_name);
				strcat ( dst_path, params[i]);
				copy_file ( dst_path, params[i]);
			}
		}
		return 0;
	}
	
	copy_file ( dst_name, src_name );
		
	return 0;
}

int is_name_like_dir(char *name){
	int i;
	for(i=0;name[i]!=0;i++);
	return (name[--i]=='/')?1:0;
}

int add_last_slash(char *name)
{
	int i;
    	//printf( ":add slash to %s\n", name);
	for( i = 0; name[i]!=0 ;i++);
	i--;
	if ( name[i] != '/'){
        //printf( "\tslash not found, add. last = %c, %d\n", name[i], i);
		name[++i] = '/';
		name[++i] = 0;
	}
    	//printf( "\tslash found, add. last = %c, %d\n", name[i], i);
	return 0;
}

char * get_last_dir (char * path)
{
	char *ret = (char *)calloc ( BUFFSIZE, sizeof(char));
	int i, j,  last_slash = 0;
	for ( i = 0; path[i] != 0; i++);
	i--;
	for (j = i; j>=0; j--){
		if(!last_slash && path[j]=='/'){
			last_slash = j;
			continue;
		}
		if(last_slash && path[j]=='/'){
			break;
		}
	}
    	//is here j on the second slash? - yeah!!1
	j++; //- i.e  ../^dir/
	for(i = j; i <= last_slash; i++)
		ret[i-j] = path[i];
	ret[i-j] = 0;
    	//printf("\t:Get last dir %s\n", ret);
	return ret;
} // for ex: "dir/"


void copy_dir (char * dst_dir, char * src_dir) //with "/"
//for ex: copy_dir tmp/ <- dimert/ =>> tmp/dimert/*
{
	struct dirent *ds;
	char dst_new[BUFFSIZE]; //destination dir;
    	char dst_path[BUFFSIZE]; //current path to copy;
    	char src_path[BUFFSIZE]; //current file from src

	
	//if ( is_verbose) printf("call copy dir %s -> %s\n", src_dir, dst_dir);

	strcpy ( dst_new, dst_dir);
	strcat ( dst_new, get_last_dir( src_dir));
	mkdir ( dst_new, 0777);
    	// "dir/temp/"
	if ( is_verbose) printf("\'%s\' -> \'%s\'\n", src_dir, dst_new);

	DIR * dir;
	if (! (dir = opendir( src_dir)) ){
		perror("can't open dir\n");
		return;
	}

	while(1) {
		ds = readdir( dir);
		if( !ds )
			break;
		if( strcmp(ds->d_name, ".")==0)
			continue;
		if( strcmp(ds->d_name, "..")==0)
			continue;
		strcpy ( src_path, src_dir);
		strcat ( src_path, ds->d_name );
		//printf("current = %s\n", cur_path);
		if ( is_dir ( src_path)){
			strcat ( src_path, "/");
			copy_dir ( dst_new, src_path);
		}
		else {
            		strcpy ( dst_path, dst_new);
		    	strcat ( dst_path, ds->d_name);
			copy_file ( dst_path, src_path);
		}
	}
}

void copy_file ( char *dst_name, char *src_name)
{
	/* copy file to file */ 
    // for ex: copy file a2 a1, a1->a2

	/* Test if destination file already exist */ 
	int dst_fd, src_fd;
	int rd_cnt, wr_cnt;
	struct stat ss;
	char buff[BUFFSIZE];
	//printf( "\n%s\n", dst_name);
	if ( is_force_link){
		if( is_file_link ( src_name)){
			char link_path[BUFFSIZE];
			int readln = readlink ( src_name, link_path, BUF);
			link_path[readln] = 0;
			strcmp ( src_name, link_path); 
		}
	}
	
	if ( is_verbose) printf("\'%s\' -> \'%s\'\n", src_name, dst_name);
	if ( is_file_exist ( dst_name )){
		if ( is_force){
			if( is_link){
				if( unlink ( dst_name) < 0){
					perror("can't remove file");
					close(dst_fd);
					exit(1);
				}
				if( symlink (src_name, dst_name) < 0 ){
					perror("can't create symlink");
					close(dst_fd);
					exit(1);
				}
				return;
			}
			if((dst_fd = open(dst_name, O_WRONLY | O_CREAT)) < 0){
				perror("can't open destination for write\n");
				close(dst_fd);
				exit(1);
			}

		}
		else{
			char c;
			do{
				printf( "do you want to rewrite file \'%s\'? (y/n)", dst_name);
				scanf( "%c", &c);
				if (c == 'y'){

					printf( "rewriting \'%s\'\n", dst_name);
					break;
				}
				if (c == 'n'){
					printf( "skip \'%s\'\n", dst_name);
					return;
				}
			}
			while ( c != 'y' && c != 'n' );
			if( is_link){
				if( unlink ( dst_name) < 0){
					perror("can't remove file");
					close(dst_fd);
					exit(1);
				}
				if( symlink (src_name, dst_name) < 0 ){
					perror("can't create symlink\n");
					close(dst_fd);
					exit(1);
				}
				return;
			}
			if((dst_fd = open(dst_name, O_WRONLY | O_CREAT)) < 0){
				perror("can't open destination for write\n");
				close(dst_fd);
				exit(1);
			}
		}
	}
	if( is_link){
		if( symlink ( src_name, dst_name) < 0 ){
			perror("can't create symlink");
			close(dst_fd);
			exit(1);
		}
		return;
	}
	if((src_fd = open(src_name,O_RDONLY | O_CREAT)) < 0){
		perror("can't open source file");
		exit(-1);
	}
	/* Reading pieces from source */
	do
	{
		rd_cnt = read ( (int)src_fd, buff, BUFFSIZE);
		//printf("read %d from src\n",rd_cnt);
		wr_cnt = write ( dst_fd, buff, rd_cnt);
		//printf("write %d to dst\n",wr_cnt);
	}
	while(rd_cnt == BUFFSIZE);
	if (stat(src_name, &ss) == -1) {
		perror("stat");
		exit(1);
	}
	if ( chmod(dst_name, ss.st_mode) ){
		perror("chmod\n");
		exit(1);
	}

	if ( chown(dst_name, ss.st_uid, ss.st_gid) ){
		perror("chown\n");
		exit(1);
	}

	/* Close all descritpors */
	close(dst_fd);
	close(dst_fd);

}

void print_help()
{
	printf ( "nobody here help you =)\n");
}

int is_dir( char *name){
	struct stat ss;
	if( open ( name, O_RDONLY) == -1)
		return 0;
	if (stat(name, &ss) == -1) {
		perror("stat");
		exit(1);
	}
	return S_ISDIR(ss.st_mode)?1:0;
}

int is_file_exist ( char *fname) 
{
	int tmpfd;
	if( (tmpfd = open ( fname, O_RDONLY)) < 0 ){
		close(tmpfd);
		return 0;
	}
	close(tmpfd);
	return 1;
}

int is_file_link ( char *fname) 
{
	struct stat buf;
	if( lstat( fname, &buf) < 0 ){
		return 0;
	}
	return 1;
}
