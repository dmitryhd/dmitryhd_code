#include <stdio.h>

#define MAXN 10+1

unsigned int bc[MAXN][MAXN];

void print_bc(int n){
	int i, j;
	for( i = 0; i < n; i++){
		for( j = 0; j < n; j++)
			printf("%3d ",bc[i][j]);
		printf("\n");
	}
}

int main(void)
{
	int n = 8;
	int i, j;
	for( i = 0; i < n; i++)
		for( j = 0; j < n; j++)
			bc[i][j] = 0;
	for( j = 0; j < n; j++)
		bc[j][j] = 1;
	for( j = 0; j < n; j++)
		bc[j][0] = 1;
	for( i = 1; i < n; i++)
		for( j = 1; j < i; j++)
			bc[i][j] = bc[i-1][j-1] + bc[i-1][j];
	print_bc(n);	
	return 0;
}
