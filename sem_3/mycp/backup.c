/* TITLE: backup */
/* Short description: just my variant of unix's cp util with */
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
#include <errno.h>

/* named vars */
#define BUFFSIZE 255
#define MAX_PARAMS 255
/* exit codes */
#define ENON_CRIT 1
#define SUCC 0
#define ECRIT 2

// last!! -> preserve

// TODO
// Keys
// errors
// deletion
// modification

int is_preserve;
int is_interactive;
int is_verbose;
int is_force;
int is_follow_link = 1;
int is_preserve;
int is_quiet; //deny all output
int is_deny_exit_on_del_err; //deny exit on deletion error
int is_exit_on_err;


int is_copy_link; // copy link instead file on link;
int is_deny_deletion;

int is_dir( char *name);
void copy_file (char *src_name, char *dst_name );
void copy_dir (char * src_name, char * dst_name );
void print_help();
int is_name_like_dir(char *name);
int remove_last_slash(char *name);
int add_last_slash(char *name);
int is_file_exist ( char *fname);
int is_file_link ( char *fname);
int is_modificated ( char *src, char *dst);
void remove_file ( char *fname);
void remove_dir ( char *dname);
char * get_last_dir (char * path);
void backup_dir ( char * src_dir, char * dst_dir  );
void compare_dirs (char *src, char *dst); 

int main( int argc, char *argv[])
{
	int par_cnt = 0;
	char *params[MAX_PARAMS];
	int option_index = 0, c;
	//key x !!!!!
	struct option long_options[] = {
		{"quiet",	0,	0,	'q'},
		{"deny_exit_on_del_err",	0,	0,	'C'},
		{"no-dereference",	0,	0,	's'},
		{"dereference",	0,	0,	'L'},
		{"denydeletion",	0,	0,	'D'},
		{"force",	0,	0,	'f'},
		{"help",	0,	0,	'h'},
		{"preserve",	0,	0,	'p'},
		{"verbose",	0,	0,	'v'},
		{"exit_on_error",	0,	0,	'x'},
		{NULL,		0,	0,	0}
	};
	while(1){
		c = getopt_long ( argc, argv, "qCsLDfhpx", long_options, &option_index );
		if ( c == -1)
			break;
		switch ( c) {
			case 'q': is_verbose = 1; break;
			case 'C': is_deny_exit_on_del_err = 1; break;
			case 's': is_follow_link = 0; break;
			case 'L': is_follow_link = 1; break;
			case 'D': is_deny_deletion = 1; break;
			case 'f': is_force = 1; break;
			case 'h': print_help(); return 0;
			case 'p': is_preserve = 1; break;
			case 'x': is_exit_on_err = 1; break;
			default: break;
		}
	}
	while( optind < argc)
		params[ par_cnt++ ] = argv[ optind++ ];
	par_cnt--;

	if ( par_cnt != 1){
		fprintf(stderr, "wrong format\n");
		print_help();
		return ENON_CRIT;
	}
	char * dst_name = NULL, * src_name = NULL;
	src_name = params[0];
	dst_name = params[par_cnt];
	if ( mkdir ( dst_name, 0777) < 0 && errno != EEXIST){
		perror ( "directory creation:");
		exit(ECRIT);
	}
	if ( !is_dir( dst_name)){
		fprintf (stderr, "%s isn't dir\n", dst_name);
		exit(ECRIT);
	}
	add_last_slash( dst_name);
	add_last_slash( src_name);
	backup_dir(src_name, dst_name);
	return SUCC;
}
/*              File related functions                   */
//---------------------------------------------------------
/* copy file to file */ 
// for ex: copy file (a1,a2) a1->a2
void copy_file ( char *src_file, char *dst_file )
{
	/* Test if destination file already exist or non modified*/ 
	int dst_fd, src_fd;
	int rd_cnt, wr_cnt;
	struct stat ss;
	char buff[BUFFSIZE];
	char src_name[BUFFSIZE];
	char dst_name[BUFFSIZE];
	strcpy ( dst_name, dst_file);
	//printf( "\n%s\n", dst_name);
	if ( is_follow_link && is_file_link ( src_name)){
		char link_path[BUFFSIZE];
		int readln = readlink ( src_file, link_path, BUFFSIZE);
		link_path[readln] = 0;
		strcpy ( src_name, link_path); 
	}
	else
		strcpy ( src_name, src_file); 

	//printf("is modificated? - %d\n", is_modificated(src_name, dst_name));
	if (!is_modificated ( src_name, dst_name))
		return;
	if ( !is_quiet)
		printf ("Copying %s into %s\n", src_name, dst_name);

	if ( is_file_exist ( dst_name ))
		remove_file ( dst_name);
	
	if((dst_fd = open(dst_name, O_WRONLY | O_CREAT)) < 0){
		perror("can't open destination for write\n");
		close(dst_fd);
		if ( !is_exit_on_err)
			return;
		exit(ECRIT);
	}
		
	if((src_fd = open(src_name,O_RDONLY | O_CREAT)) < 0){
		perror("can't open source file");
		if ( !is_exit_on_err)
			return;
		exit(ECRIT);
	}
	/* Reading pieces from source */
	do
	{
		//printf("piece\n");
		rd_cnt = read ( (int)src_fd, buff, BUFFSIZE);
		//printf("read %d from src\n",rd_cnt);
		wr_cnt = write ( dst_fd, buff, rd_cnt);
		//printf("write %d to dst\n",wr_cnt);
	}
	while(rd_cnt == BUFFSIZE);
//timestamps todo
	if ( is_preserve){
		if (stat(src_name, &ss) == -1) {
			perror("stat, src");
			if ( !is_exit_on_err)
				return;
			exit(ENON_CRIT);
		}
		if ( chmod(dst_name, ss.st_mode) ){
			perror("chmod, dst\n");
			if ( !is_exit_on_err)
				return;
			exit(ENON_CRIT);
		}
		if ( chown(dst_name, ss.st_uid, ss.st_gid) ){
			perror("chown, dst\n");
			if ( !is_exit_on_err)
				return;
			exit(ENON_CRIT);
		}
	}
	/* Close all descritpors */
	close(dst_fd);
	close(dst_fd);
}
//---------------------------------------------------------
//
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
//---------------------------------------------------------
//
void remove_file ( char *fname)
{
	if(!is_quiet)
		printf ("Deleting destination object %s\n", fname);
	if ( unlink ( fname) < 0 ){
		perror ("deletion");
		if ( !is_exit_on_err)
			return;
		if ( is_deny_exit_on_del_err)
			return;
		exit(ECRIT);
	}
}
//---------------------------------------------------------
//
int is_file_link ( char *fname) 
{
	struct stat buf;
	if( !is_file_exist( fname))
		return 0;
	if( lstat( fname, &buf) < 0 ){
		perror ( "stat");
		if(!is_exit_on_err)
			return 0;
		exit ( ENON_CRIT);
	}
	if ( S_ISLNK(buf.st_mode))
		return 1;
	else
		return 0;
}
//---------------------------------------------------------
//todo - check sums
int is_modificated ( char *src, char *dst)
{
	struct stat ss, ds;
	if (!is_file_exist ( src)){
		//printf("file %s don't exist\n", src);
		return 1;
	}
	if (!is_file_exist ( dst)){
		//printf("file %s don't exist\n", dst);
		return 1;
	}
	if (stat(src, &ss) == -1) {
		perror("stat, modif src");
		if ( !is_exit_on_err)
			return 0;
		exit(ENON_CRIT);
	}
	if (!S_ISREG( ss.st_mode)){
		if(!is_quiet)
			printf ("File %s is not copied into %s because %s isn't regular file\n", src, dst, src);
		return 0;
	}
	if (stat(dst, &ds) == -1) {
		perror("stat, modif dst");
		if ( !is_exit_on_err)
			return 0;
		exit(ENON_CRIT);
	}
	if ( !S_ISREG( ds.st_mode)){
		if(!is_quiet)
			printf ("File %s is not copied into %s because %s isn't regular file\n", src, dst, dst);
		return 0;
	}

	if (ss.st_mtime > ds.st_mtime){
		return 1;
	}
	if (ss.st_size != ds.st_size){
		return 1;
	}
	if ((ss.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) != (ds.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) ){
		return 1;
	}
		// get file acess permissions
		// Read Write eXecute for Usr, Group and Others
	if ( ss.st_uid != ds.st_uid  && ss.st_gid != ds.st_gid)
		return 1;
	if(!is_quiet)
		printf ("File %s is not copied into %s because file don't modified\n", src, dst);
	return 0;
}

/*        Directory related functions			*/
//---------------------------------------------------------
void backup_dir ( char * src_dir, char * dst_dir  ) //with "/"
//slightly differ from copy_dir (sm ex)
//for ex: copy_dir dimert/ -> tmp/ -> =>> tmp/*
{
	char dst_new[BUFFSIZE]; //destination dir;
    	char dst_path[BUFFSIZE]; //current path to copy;
    	char src_path[BUFFSIZE]; //current file from src
	
	strcpy ( dst_new, dst_dir);
	//if ( is_verbose) printf("call copy dir %s -> %s\n", src_dir, dst_dir);
	if(!is_quiet)
		printf ("Begin: Entering directory %s\n", dst_new );

	compare_dirs ( src_dir, dst_new);
	errno = 0;
	if( mkdir ( dst_new, 0777) < 0 && errno != EEXIST){
		perror ("creation directory");
		if ( !is_exit_on_err)
			return;
		exit ( ECRIT);
	}
	if(!is_quiet && errno != EEXIST)
		printf ("Making directory %s\n", dst_new);

	DIR * dir;
	struct dirent *ds;
	if (! (dir = opendir( src_dir)) ){
		perror("can't open dir\n");
		if ( !is_exit_on_err)
			return;
		exit ( ECRIT);
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
		if ( is_dir ( src_path)){
			strcat ( src_path, "/");
			copy_dir ( src_path, dst_new );
		}
		else {
			//printf("current = %s\n", src_path);
            		strcpy ( dst_path, dst_new);
		    	strcat ( dst_path, ds->d_name);
			copy_file ( src_path, dst_path);
		}
	}
}
//---------------------------------------------------------
void copy_dir ( char * src_dir, char * dst_dir  ) //with "/"
//for ex: copy_dir dimert/ -> tmp/ -> =>> tmp/dimert/*
{
	char dst_new[BUFFSIZE]; //destination dir;
    	char dst_path[BUFFSIZE]; //current path to copy;
    	char src_path[BUFFSIZE]; //current file from src
	
	strcpy ( dst_new, dst_dir);
	strcat ( dst_new, get_last_dir( src_dir));
	//if ( is_verbose) printf("call copy dir %s -> %s\n", src_dir, dst_dir);
	if(!is_quiet)
		printf ("Entering directory %s\n", dst_new );
	compare_dirs ( src_dir, dst_new);
	errno = 0;
	if( mkdir ( dst_new, 0777) < 0 && errno != EEXIST){
		perror ("creation directory");
		if ( !is_exit_on_err)
			return;
		exit ( ECRIT);
	}
	if(!is_quiet && errno != EEXIST)
		printf ("Making directory %s\n", dst_new);

	DIR * dir;
	struct dirent *ds;
	if (! (dir = opendir( src_dir)) ){
		perror("can't open dir\n");
		if ( !is_exit_on_err)
			return;
		exit ( ECRIT);
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
		if ( is_dir ( src_path)){
			strcat ( src_path, "/");
			copy_dir ( src_path, dst_new );
		}
		else {
			//printf("current = %s\n", src_path);
            		strcpy ( dst_path, dst_new);
		    	strcat ( dst_path, ds->d_name);
			copy_file ( src_path, dst_path);
		}
	}
}
//---------------------------------------------------------

int is_dir( char *name){
	struct stat ss;
	if( open ( name, O_RDONLY) == -1)
		return 0;
	if (stat(name, &ss) == -1) {
		perror("stat");
		if ( !is_exit_on_err)
			return 0;
		exit(ENON_CRIT);
	}
	return S_ISDIR(ss.st_mode) ? 1 : 0;
}

//---------------------------------------------------------
//todo
void compare_dirs (char *src, char *dst) 
{
	if ( is_deny_deletion)
		return;
	DIR * dir;
	struct dirent *ds;
	char **src_entries = NULL;
	char **dst_entries = NULL;
	//get src folder content
	if (! (dir = opendir( src)) ){
		perror("can't open dir\n");
		if ( !is_exit_on_err)
			return;
		exit(ECRIT);
	}
	int src_cnt = 1;
	while(1) {
		src_entries = (char **) realloc( src_entries, src_cnt*sizeof(char*));
		ds = readdir( dir);
		if( !ds )
			break;
		if( strcmp(ds->d_name, ".")==0)
			continue;
		if( strcmp(ds->d_name, "..")==0)
			continue;

		src_entries[ src_cnt - 1 ] = (char *) malloc ( (strlen(ds->d_name)+1) * sizeof(char) );
		strcpy ( src_entries[ src_cnt - 1], ds->d_name);
		//printf("current = %s\n", cur_path);
		src_cnt++;
	}
	src_cnt--;

	//get dst folder content
	if (! (dir = opendir( dst)) ){
		perror("can't open dir\n");
		if ( !is_exit_on_err)
			return;
		exit(ECRIT);
		return;
	}
	int dst_cnt = 1;
	while(1) {
		dst_entries = (char **) realloc( dst_entries, dst_cnt*sizeof(char*));
		ds = readdir( dir);
		if( !ds )
			break;
		if( strcmp(ds->d_name, ".")==0)
			continue;
		if( strcmp(ds->d_name, "..")==0)
			continue;

		dst_entries[ dst_cnt - 1 ] = (char *) malloc ( (strlen(ds->d_name)+1) * sizeof(char) );
		strcpy ( dst_entries[ dst_cnt - 1], ds->d_name);
		//printf("current = %s\n", cur_path);
		dst_cnt++;
	}
	dst_cnt--;
	//compare;
	int i, j;
	char dst_path[BUFFSIZE]; //destination dir;
	for( i = 0; i < dst_cnt; i++)
	{
		int is_exist = 0; //exist in src directory
		for ( j = 0; j < src_cnt; j++)	
			if( strcmp ( dst_entries[i], src_entries[j] ) == 0){
				is_exist = 1;
				break;
			}
		if ( !is_exist){
			strcpy ( dst_path, dst);
			strcat ( dst_path, dst_entries[i]);

			if ( is_dir(dst_path)){
				strcat( dst_path, "/");
				remove_dir (dst_path);
			}
			else
				remove_file (dst_path);
		}
	}
	//free_mem;
	for ( i = 0; i < src_cnt; i++)
		free(src_entries[i]);
	free(src_entries);
	for ( i = 0; i < dst_cnt; i++)
		free(dst_entries[i]);
	free(dst_entries);
}
//---------------------------------------------------------

void remove_dir ( char *dname)
//dir with ../"
{
	char src_path[BUFFSIZE];
	DIR * dir;
	struct dirent *ds;
	if (! (dir = opendir( dname)) ){
		perror("can't open dir\n");
		if ( !is_exit_on_err)
			return;
		if ( is_deny_exit_on_del_err)
			return;
		exit(ECRIT);
	}
	//first, we clean directory
	while(1) {
		ds = readdir( dir);
		if( !ds )
			break;
		if( strcmp(ds->d_name, ".")==0)
			continue;
		if( strcmp(ds->d_name, "..")==0)
			continue;
		strcpy ( src_path, dname);
		strcat ( src_path, ds->d_name );
		//printf("current = %s\n", cur_path);
		if ( is_dir ( src_path)){
			remove_dir ( src_path);
		}
		else 
            		remove_file ( src_path);
	}
	//finally, we can remove directory itself
	if(!is_quiet)
		printf ("Deleting destination object %s\n", dname);
	if ( rmdir ( dname) < 0){
		perror ("remove dir");
		if ( !is_exit_on_err)
			return;
		if ( is_deny_exit_on_del_err)
			return;
		exit(ECRIT);
	}
}
/*               String related functions             */
//---------------------------------------------------------
void print_help()
{
	printf ( "nobody here help you =)\n");
}
//---------------------------------------------------------
int is_name_like_dir(char *name){
	int i;
	for(i=0;name[i]!=0;i++);
	return (name[--i]=='/')?1:0;
}
//---------------------------------------------------------
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
//---------------------------------------------------------

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
