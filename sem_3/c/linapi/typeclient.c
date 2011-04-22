#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

char fname[]="./fifofile";

int main(int argc, char **argv){
	FILE *f=fopen(fname,"r");
	int c=1;
	while(c!='Q'){
		//c=getchar();
		c=fgetc(f);
		putchar(c);
	}
	fclose(f);
	unlink(fname);
	return 0;
}
