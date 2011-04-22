//horse =)
#include <stdio.h>
#define LEN 56790
#define MAXN 10

char arr[LEN];
int cnt, max_lev;

void assoc(char *a, char ch, int *mark);
void set_horse(int lev, char num);
int main(int argc, char *argv[]){
	int i;
	scanf("%d",&max_lev);
	max_lev--;
	for(i=0;i<10;i++){
		if(i==0 || i==8)
			continue;
		set_horse(0,i);	
	}
	printf("%d\n",cnt);
	return 0;
}
void set_horse(int lev, char num){
	int i, mark;
	char a[MAXN];
	arr[lev]=num;
	assoc(a,num,&mark);
	if(lev==max_lev){
		for(i=0;i<=lev;i++)
			printf("%d",arr[i]);
		printf("\n");
		cnt++;
		return;
	}
	for(i=0;i<mark;i++){
		set_horse(lev+1,a[i]);
	}	
}
void assoc(char *a, char ch, int *mark){
	switch(ch){
		case 0:
			a[0]=4;
			a[1]=6;
			*mark=2;
			break;
		case 1:
			a[0]=8;
			a[1]=6;
			*mark=2;
			break;
		case 2:
			a[0]=7;
			a[1]=9;
			*mark=2;
			break;
		case 3:
			a[0]=4;
			a[1]=8;
			*mark=2;
			break;
		case 4:
			a[0]=9;
			a[1]=3;
			a[2]=0;
			*mark=3;
			break;
		case 5:
			*mark=0;
			break;
		case 6:
			a[0]=7;
			a[1]=1;
			a[2]=0;
			*mark=3;
			break;
		case 7:
			a[0]=2;
			a[1]=6;
			*mark=2;
			break;
		case 8:
			a[0]=1;
			a[1]=3;
			*mark=2;
			break;
		case 9:
			a[0]=2;
			a[1]=4;
			*mark=2;
			break;
	}
}
 
