#include <stdio.h>
#define MAXN 12

void print_comb(short *way,int k);
void comb(int n, int k);

int main(int argc, char *argv[]){
	int n=5,k=4;
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
	for(i=k-1;i>=0;){
		//move last num. to the end or
	//	printf("way[%d]=%d\n",i,way[i]);
		//прогоним последний эелемент до конца.
		while(1){
			if(way[i]<n && (way[i+1]!=way[i]+1)){
				way[i]++;	
				//print new comb;
				print_comb(way,k);
			}
			else 
				break;		
		}
		for(j=0;way[j]==n-k+1+j && j<k;j++);
		if(j==k)
			break;
		
		
		if(i==0)
			break;
		while (1){
			if(i>0 && way[i-1]<way[i] ){
				way[--i]++;
				//выровняем
				printf("rovn ot %i, i=%d, k-i=%d\n",way[i],i,k-i);
				for(j=1;j<k-i && way[i]+k-i-1<=n;j++){
					//set next elements in right order
					//such as (125)->(13_4_), change i=1,
					//and we move back way[2]=(way[i]+1)+1=3+1
					//printf("move\n");
				//	printf("\tj=%d\n",j);
					way[i+j]=way[i]+j;
					//if(way[i]<n && way[i+1]!=way[i]+1){
					//	way[i+j]=way[i]+j;
					//}
				//	i=k-1;
				}
				print_comb(way,k);
				if(way[k-1]<n)
					i=k-1;
				else 
					i=k-2;
				break;
			}
			i--;
			if(i==0)
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

