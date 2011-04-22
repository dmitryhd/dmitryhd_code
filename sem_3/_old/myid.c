/* Title: myid */
/* Short description: try to write full analog of unix's id*/
/* Last modification: DmitryKh - 12.09.09*/
/* Written by Dmitry Khodakov gr. 817*/

/* tam-tam-tam =) I hope it works properly */
/* small bug: myid -Gn don't work; */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>

#define BUFSIZE 0x100
#define GRSIZE 0x100
#define EFIND 12

#define EARG 11

char * gid_to_name (gid_t gid)
{
	struct group *sgid;
	if((sgid = getgrgid(gid)) == NULL){
		perror("can't find id");
		exit(EFIND);
	}
	return sgid->gr_name;
}

uid_t name_to_uid (char * name)
{
	struct passwd *spass;
	if((spass = getpwnam(name)) == NULL){
		fprintf(stderr,"%s: no such user\n",name);
		exit(EFIND);
	}
	return spass->pw_uid;
}
gid_t name_to_gid (char * name)
{
	struct passwd *spass;
	if((spass = getpwnam(name)) == NULL){
		perror("can't find id");
		exit(EFIND);
	}
	return spass->pw_gid;
}

char * uid_to_name (uid_t uid)
{
	struct passwd *spass;
	if((spass = getpwuid(uid)) == NULL){
		perror("can't find id");
		exit(EFIND);
	}
	return spass->pw_name;
}

gid_t * get_groups (char * name, int *num)
{
	gid_t * list = NULL;
	struct group  * grtmp = NULL;
	list = (gid_t *)malloc(GRSIZE*sizeof(gid_t));
	int i, cnt = 0;
	while((grtmp = getgrent()))
		for ( i=0;grtmp->gr_mem[i] != NULL;i++)
			if(strcmp(grtmp->gr_mem[i],name) == 0){
				list[cnt++] = grtmp->gr_gid;
				break;
			}
	*num = cnt;
	return list;	
}

int main(int argc, char *argv[])
{
	int i;
	gid_t user_gid;
	uid_t user_uid;


	int is_only_group = 0;
	int is_only_group_all = 0;
	int is_name = 0;
	int is_only_user = 0;
	int is_real = 0;

	//bug!: myid -Gn - err
	char *opts="rgGnu";
	int opt, opt_c = 0;
	while((opt=getopt(argc,argv,opts))!=-1){
		switch(opt){
			case 'a':
				break;
			case 'g':
				is_only_group = 1;
				break;
			case 'G':
				is_only_group_all = 1;
				break;
			case 'n':
				is_name = 1;
				break;
			case 'r':
				is_real = 1;
				break;
			case 'u':
				is_only_user = 1;
				break;
		}
		opt_c++;
	}
	//without username
	if(argc == 1 || (opt_c + 1) == argc){
		if( is_real){
			user_gid = getgid();
			user_uid = getuid();
		}
		else{
			user_gid = getegid();
			user_uid = geteuid();

		}
	}
	//with username in command line
	else{
		user_gid = name_to_uid(argv[1+opt_c]);
		user_uid = name_to_gid(argv[1+opt_c]); 
	}
	// -g flag.
	if( is_only_group){
		if( is_name)
			printf("%s\n",gid_to_name(user_gid));
		else
			printf("%d\n",user_gid);
		return 0;
	}
	// -G flag.
	if( is_only_group_all){
		int list_size = 0;
		gid_t * user_groups = get_groups(gid_to_name(user_gid),&list_size);
		for( i=0;i<list_size;i++)
			if( is_name)
				printf("%s ",gid_to_name(user_groups[i]));
			else
				printf("%d ",user_groups[i]);
		printf("\n");
		return 0;
	}
	// -u flag.
	if( is_only_user){
		if( is_name)
			printf("%s\n",uid_to_name(user_uid));
		else
			printf("%d\n",user_uid);
		return 0;
	}
	//wrong flags
	if( is_name && is_real){
		fprintf(stderr,"id: cannot print only names or real IDs in default format\n");
		return EARG;
	}
	//common case:
	printf("uid=%d(%s) ",user_uid,uid_to_name(user_uid));
	printf("gid=%d(%s) ",user_gid,gid_to_name(user_gid));

	int list_size = 0;
	gid_t * user_groups = get_groups(gid_to_name(user_gid),&list_size);
	printf("groups=");
	printf("%s(%d)",gid_to_name(user_gid),user_gid);
	for( i=0;i<list_size;i++)
		printf("%d(%s) ",	user_groups[i],
					gid_to_name(user_groups[i]));
	printf("\n");
	return 0;
}
