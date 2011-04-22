#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>


#define BUF 256
#define D_TYPE_LINK 10 // experimentally :)

char * gid_to_name(gid_t gid)
{
	struct group *sgid;
	if((sgid=getgrgid(gid)) == NULL){
		perror("get gid error\n");
		exit(1);
	}
	return sgid->gr_name;
}
char * uid_to_name(uid_t uid)
{
	struct passwd *suid;
	if((suid=getpwuid(uid)) == NULL){
		perror("get gid error\n");
		exit(1);
	}
	return suid->pw_name;
}

int main (int argc, char *argv[]) 
{
	if( argc != 2 && argc != 1){
		printf("usage: %s dir\n", argv[0]);
		return 1;
	}

	DIR * dir;
	if(argc == 1){
		if (! (dir = opendir( ".")) ){
			perror("can't open dir\n");
			return 2;
		}
	}
	else{
		if (! (dir = opendir( argv[1])) ){
			perror("can't open dir\n");
			return 2;
		}
	}

	struct dirent *ds;
	struct stat ss;
	char path[BUF];
	char cur_path[BUF];
	char stime[BUF];
	char link_path[BUF];

	if( argc == 1)
		strcpy ( path, "");
	if( argc == 2)
		strcpy ( path, argv[1]);
	//printf("path = %s\n", path);

	while(1) {
		ds = readdir( dir);
		if( !ds )
			break;

		int is_link = 0;
		if( ds->d_type == D_TYPE_LINK )
			is_link = 1;
		strcpy ( cur_path, path);
		strcat ( cur_path, ds->d_name );
		//printf("cur_path = %s\n", cur_path);
		if (stat(cur_path, &ss) == -1) {
               		perror("stat");
               		return 1;
           	}
		if( !is_link ){
			switch (ss.st_mode & S_IFMT) {
				case S_IFBLK:  printf("b");            break;
				case S_IFCHR:  printf("c");        break;
				case S_IFDIR:  printf("d");               break;
				case S_IFIFO:  printf("p");               break;
				case S_IFLNK:  printf("l"); is_link = 1;              break;
				case S_IFREG:  printf("-");            break;
				case S_IFSOCK: printf("s");                  break;
				default:       printf("?");                break;
			}
		}
		else{
			printf("l");
		}
		//printf ( "mode = %o \n", ss.st_mode );
		int i, m, s;
		//here we print rights =)
		for( m = 0700, s = 06; s >= 0; m >>= 3, s -= 3 ){
			i = ss.st_mode & m;
			i >>= s;
			if(i & 04) printf("r");
			else printf("-");
			if(i & 02) printf("w");
			else printf("-");
			if(i & 01) printf("x");
			else printf("-");
		}		

		printf(" %3d", (int)ss.st_nlink);
		printf(" %10s %10s", uid_to_name(ss.st_uid), gid_to_name(ss.st_gid));
		printf(" %8d", (int)ss.st_size);
		strcpy ( stime, ctime(&ss.st_atime));  
		stime[strlen(stime)-2] = 0; // delete \n
		printf(" %s",stime);
		printf (" %s", ds->d_name);
		if( is_link ) {
			int readln = readlink ( cur_path, link_path, BUF);
			link_path[readln] = 0;
			printf(" -> %s", link_path);
		}
		printf("\n");
	}
	
	return 0;
}
