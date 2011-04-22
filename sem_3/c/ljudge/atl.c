#include <stdio.h>
#define MAXN 100
long n;
int weight[MAXN], force[MAXN];


int main(int argc, char *argv[]){
	long n, i;
	scanf("%ld\n",&n);	
	for(i=0;i<n;i++)
		scanf("%d %d\n",&weight[i],&force[i]);
	return 0;
}
