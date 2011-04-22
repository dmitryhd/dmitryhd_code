#include <stdio.h>
#include <stdlib.h>
#define BUFSZ 0x100
#define _openf(desc,filename,mode) (desc)=fopen((filename),(mode));\
if (desc==NULL){\
	printf("io err\n");\
	exit(1);}
	

int main(int argc, char **argv){
	char buf[BUFSZ];
	int len;
	FILE *p=popen(argv[1],"r");
	if((len=fread(buf,1,BUFSZ,p))==0){
		printf("io err\n");
		exit(1);
	}
	int i;
	FILE *des[100];
	for(i=2;i<argc;i++){
		_openf(des[i-2],argv[i],"w");
		fwrite(buf,1,len,des[i-2]);
	}
	fwrite(buf,1,len,stdout); 
	return 0;
}
