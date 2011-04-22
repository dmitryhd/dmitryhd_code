#include <stdio.h>
#include <stdlib.h>
#define BUF 0x100
int main(int argc,char *argv[]){
	int i, length;
	char buf[BUF];
	FILE *f=fopen("/proc/self/environ","r");
	if(f==NULL){
		fprintf(stderr,"err open file\n");
		exit(1);
	}
	while((length=fread(buf,1,BUF-1,f))>0){
		for(i=0;i<length;i++)
			if(!buf[i])
				buf[i]=10;
		printf("%s",buf);
	}
	return 0;
}
 
