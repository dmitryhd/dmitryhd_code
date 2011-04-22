#include <stdio.h>
#define MAXN 12

void print_comb(short *way,int k);
void comb(int n, int k);

int main(int argc, char *argv[]){
	int n=4,k=3;
	comb(n,k);
	return 0;
}

void comb(int n, int k){
	int i,j;
	short way[MAXN];
	//set start combination
	//for example 1 2 3 (in case (5,3))
	for(i=1;i<=k;i++)
		way[i-1]=i;
	//print first comb.
	print_comb(way,k);
	for(i=k-1;i>=0;i--){
		//move last num. to the end or
		printf("way[%d]=%d\n",i,way[i]);
		for(j=1;j<k-i;j++){
			//set next elements in right order
			//such as (125)->(13_4_), change i=1,
			//and we move back way[2]=(way[i]+1)+1=3+1
			printf("move\n");
			if(way[i]<n && way[i+1]!=way[i]+1){
				way[i+j]=way[i]+j;
			}
		//	i=k-1;
		}
		if(way[i]<n&& (i==0 || way[i+1]>way[i])){
			way[i]=way[i]+1;	
			//print new comb;
			print_comb(way,k);
			i=k-1;
		}
		while(1){
			if(way[i]<n && (i==0 || way[i+1]>way[i]) ){
				way[i]=way[i]+1;	
				//print new comb;
				print_comb(way,k);
			}
			else 
				break;		
		}
	}
}
void print_comb(short *way,int k){
	int i;
	printf("\t");
	for(i=0;i<k;i++)
		printf("%d ",way[i]);
	printf("\n");
}

