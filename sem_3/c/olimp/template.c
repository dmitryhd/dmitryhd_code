#include <stdio.h>

#define MAX_AR 10000

unsigned long long pr[MAX_AR];

int main(void)
{
	int a, b, i;
	scanf("%d %d\n",&a,&b);
	for(i=0;i<a;i++)
		scanf("%lld ",&pr[i]);
	printf("%d %d\n",a,b);
	for(i=0;i<a;i++)
		printf("%lld ", pr[i]);
	printf("\n");
	return 0;
}
