/*See Cormen's mit book. DP. (test)*/
#include <stdio.h>

#define N 10000

int in[2], out[2];
int line[2][N];
int throw[2][N];
int l1[N], l2[N];
int n;

int fastest_way(char * best){
	int f1 = in[0] + line[0][0], f2 = in[1] + line[1][0];	
	int i;
	for( i = 1; i < n; i++){
		if( f1 + throw[0][i] + line[1][n] >= f1 + line[0][i]){
			f1 += line[0][i]; 
			l1[i] = 1;
		}
		else{
			f1 += throw[0][i] + line[1][i]; 
			l1[i] = 2;
		}
		if( f2 + throw[1][i] + line[0][n] >= f1 + line[1][i]){
			f2 += line[1][i]; 
			l2[i] = 2;
		}
		else{
			f2 += throw[1][i] + line[0][i];
			l2[i] = 1;
		}
	}
	if (f1 > f2){
		*best = 2;
		return f2; 
	}
	else {
		*best = 1;
		return f1;
	}
}

void print_way(char best){
	int i;
	if(best == 1)
		for( i = 1; i < n; i++)
			printf("#%d - %d\n",i,l1[i]);
	if(best == 2)
		for( i = 1; i < n; i++)
			printf("#%d - %d\n",i,l2[i]);
}


int main(void)
{
	int i;
	scanf("%d %d %d %d %d ",&n, &in[0],&in[1],&out[0],&out[1]);
	for(i=0;i<n;i++){
		scanf("%d %d ",&line[0][i], &line[1][i]);
		scanf("%d %d ",&throw[0][i], &throw[1][i]);
	}
	char best;
	printf("%d\n",fastest_way(&best));
	print_way(best);
	return 0;
}
