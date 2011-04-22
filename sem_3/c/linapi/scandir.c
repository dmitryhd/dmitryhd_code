#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int sel(struct dirent * x){
	return 1;
}

int main(int argc,char **argv){
	int n,i;
	struct dirent ** list;
	if(argc!=2)
		exit(1);
	n=scandir(argv[1],&list,sel,alphasort);
	if(n<0)
		exit(1);
	for(i=0;i<n;i++)
		printf("%s inode=%o\n",list[i]->d_name,list[i]->d_ino);	
	return 0;
}
