#include <stdio.h>
#include <stdlib.h>

int cmp(const void *a, const void *b){
	return *(int *)a- *(int *)b;
}

int main(int argc, char *argv[]){
		
	int *a, n,i;
	scanf("%d ",&n);
	a=(int *)malloc(sizeof(int)*n);
	for(i=0;i<n;i++)
		scanf("%d ",&a[i]);
	printf("res: ");
	qsort(a,n,sizeof(int),cmp);
	for(i=0;i<n;i++)
		printf("%d ",a[i]);
	printf("\n");
	
	return 0;
}
