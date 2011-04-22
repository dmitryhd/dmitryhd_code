#include <stdio.h>
/*finds number of all num. sum of which nums is N;
 * 5: 11111 1112 1121 ....
 * */

/* n - num. k - max part*/
int num_partition(int n, int k)
{
	if(n == 1 && k == 1)	//trivia
		return 1;
	if(k > n)
		return 0;	//trivia
	return num_partition( n - k, k) + num_partition( n, k - 1); 
}

int main(void)
{
	int n, k;
	scanf("%d %d\n",&n, &k);
	printf("%d\n",num_partition(n,k));
	return 0;
}
