#include <stdio.h>
#include <stdlib.h>
#define SIZE 1001
int cmp(const void *a, const void *b){
	return *(int *)a- *(int *)b;
}
int main(int argc, char *argv[]){
	long xa[SIZE],xb[SIZE],a,acurr=0,bcurr=0;
	int i,j;
	while(1){
L:		scanf("%ld ",&a);
		if(a==-1)
			break;
		for(i=0;i<acurr;i++)
			if(xa[i]==a)
				goto L;
		xa[acurr++]=a;	
	}
	while(1){
		scanf("%ld ",&a);
		if(a==-1)
			break;
		for(i=0;i<acurr;i++){
			if(xa[i]==a)
				xb[bcurr++]=a;	
			
		}
	}
	if(bcurr==0){
		printf("empty");
		return 0;
	}
	acurr=0;
	for(i=0;i<bcurr;i++){
		for(j=0;j<acurr;j++)
			if(xa[j]==xb[i])
				goto L2;
		
		xa[acurr++]=xb[i];
		L2: continue;
	}
	qsort(xa,acurr,sizeof(long),cmp);
	
	for(i=0;i<acurr;i++)
		printf("%ld ",xa[i]);
	printf("\n");
	return 0;
}
