#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

char fname[]="./fifofile";

int main(int argc, char **argv){
	mkfifo(fname,0600);
	FILE *f=fopen(fname,"w");
	int c=1;
	while(c!='Q'){
		c=getchar();
		fputc(c,f);
		if(c==10)
			fflush(f);
	}
	fclose(f);
	unlink(fname);
	return 0;
}
