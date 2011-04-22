#include <stdio.h>
#include <stdlib.h>
#define MAXN 200
typedef struct _pl{
	char wl[MAXN];
	//1-win
	//0-lose
}pl;
pl p[MAXN];
int n;
void print_pl(){
	int i,j;
	for(i=0;i<n;i++){
		printf("pl %d : ",i+1);
		for(j=0;j<n;j++)	
			printf("%d",p[i].wl[j]);
		printf("\n");
	}
}
char obh[MAXN];//1 +обошли, 0 -не обошли 
int obh_num;
void print_way(){
	int j;
	printf("WAY:");
	for(j=0;j<obh_num;j++)
		printf("%d ",obh[j]+1);
	printf("\n");
}
int rec_gr(int x){
	//1 есть путь
	//0 - нет пути =)
	int i,j;
	obh[obh_num]=x;
	obh_num++;
	if(obh_num==n)
		return 1;
	printf("enter %d\n",x+1);
	print_way();
	for(i=0;i<n;i++){
		if(p[x].wl[i]==1 && i!=x){
			printf("i=%d\n",i+1);
			for(j=0;j<obh_num;j++){
				if(obh[j]==i)
					goto X;
				
			}
			if(rec_gr(i)==1)
				return 1;
			obh_num--;
		}
		
		X: continue;
	}
	printf("back\n");
	return 0;
}

int main(int argc, char *argv[]){
	int i,j;
	char ch;
	scanf("%d\n",&n);
	for(i=0;i<n;i++){
		for(j=0;j<i;j++){
			scanf("%c",&ch);
			if(ch=='+'){
				p[i].wl[j]=1;
				p[j].wl[i]=0;
			}
			else{
				p[i].wl[j]=0;
				p[j].wl[i]=1;
			}
		}
		scanf("#\n");
	}
	print_pl();	
	for(i=2;i<n;i++){
		if(rec_gr(i)==1){
			print_way();
			return 0;
		}
		printf("----\n");
	}
	return 0;
}
