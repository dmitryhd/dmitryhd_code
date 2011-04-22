#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
	FILE *f=fopen("abcaddd","r");
	if(1){
		fprintf(stderr,"can't open file %s\n",strerror(errno));
		exit(1);
	}
	return 0;
}
