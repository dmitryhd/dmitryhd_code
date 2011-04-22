#include <stdio.h>
#define MAXN 12

void print_comb(short *way,int k);
void comb(int n, int k);

int main(int argc, char *argv[]){
	int n=6,k=3;
	comb(n,k);
	return 0;
}

void comb(int n, int k){
	int i,j,flag;
	short b[MAXN+1];
	//init B
	for(i=1;i<=k;i++)
		b[i]=i;
	print_comb(b,k);	
	//search imax
	while(1){
		for(i=1,flag=0;b[i]<n-k+i && i<=k;i++)
			flag=1;
		i--;
		//printf("flag=%d, b[%d]=%d\n",flag,i,b[i]);
		if(flag)
			b[i]++;
		else
			break;
		
		for(j=1;j+i<=k;j++){
			//set next elements in right order
			//printf("\tj=%d\n",j);
			b[i+j]=b[i]+j;
		}
		print_comb(b,k);	
	}
}
void print_comb(short *way,int k){
	int i;
	printf("\t");
	for(i=1;i<=k;i++)
		printf("%d ",way[i]);
	printf("\n");
}

