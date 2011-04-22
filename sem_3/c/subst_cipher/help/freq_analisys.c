#include <stdio.h>
#define MAXNUM 26
int main(int argc, char *argv[]){
	int num[MAXNUM+1],i,a;
	for(i=0;i<=MAXNUM;i++)
		num[i]=0;
	for (i=1;(scanf("%d",&a))==1;i++)
		num[a]++;	
	for(i=1;i<=MAXNUM;i++)
		printf("%d - %d\n",i,num[i]);
	return 0;
}
