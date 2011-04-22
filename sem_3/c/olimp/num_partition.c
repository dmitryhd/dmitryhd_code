#include <stdio.h>
/*finds number of all num. sum of which nums is N;
 * 5: 11111 1112 1121 ....
 * */

/* n - num. k - max part*/
int num_partition(int n, int k)
{
	printf("num partition: n=%d, k=%d\n",n,k);
	if(n == k)
		return 1;
	if(k > n)
		return 0;
	return num_partition( n - k, k) + num_partition( n, k + 1); 
}

int main(void)
{
	int n = 5, k = 3;
	scanf("%d %d\n",&n, &k);
	printf("%d %d\n",n,k);
	printf("%d\n",num_partition(n,1));
	return 0;
}
