#include <stdio.h>
int i=1;

int main(){
	int i,j;
	int fsize=20;	
	for(i=-fsize/2;i<fsize/2;i++){
		for(j=-fsize/2;j<fsize/2;j++){
			printf("i=%d, j=%d\n",i,j);
		}
	}
	return 0;
}
