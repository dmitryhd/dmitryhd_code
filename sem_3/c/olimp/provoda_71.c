#include <stdio.h>

#define MAX_AR 10000

unsigned long long pr[MAX_AR];
unsigned long long sum;
int a, b;

int try_cL(int cL)
{
	int i, cnt = 0;	
	for(i=0;i<a;i++)
		cnt += pr[i]/cL;
	return (cnt < b)? 0 : 1; 	
}

int main(void)
{
	int i, cL;
	scanf("%d %d\n",&a,&b);
	for(i=0;i<a;i++){
		scanf("%lld ",&pr[i]);
		sum += pr[i];
	}
	cL = sum / b;
	while(!try_cL(cL)){
		cL--;
	}
	//printf("%d %d\n",a,b);
	//for(i=0;i<a;i++)
	//	printf("%lld ", pr[i]);
	printf("%d\n",cL);
	return 0;
}
