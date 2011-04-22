#include <stdio.h>

#define LEN 2000
typedef struct _lnum{
	char num[LEN];
	char sign;
	int cnt;
}lnum;
int lev,max_lev;
lnum ladd(lnum a, lnum b);
void fibon(lnum an_1, lnum an_2);
void printlnum(lnum a);
int main(int argc, char *argv[]){
	lnum a0,a1;
	int i;
	a0.sign=a1.sign=0;
	for(i=0;i<LEN;i++)
		a0.num[i]=a1.num[i]=0;
	a0.num[0]=0;
	a1.num[0]=1;
	a1.cnt=a0.cnt=1;
	lev=1;
	scanf("%d",&max_lev);
	if(max_lev==0){
		printf("0\n");
		return 0;
	}
	fibon(a1,a0);	
	return 0;
}
void fibon(lnum an_1, lnum an_2){
	lnum res;
	res=ladd(an_1,an_2);
	if(lev==max_lev){
		printlnum(res);
		printf("\n");
		return;
	}
	lev++;
	fibon(res,an_1);
}
lnum ladd(lnum a, lnum b){
	int i,flag=1;
	lnum res;
	res.sign=res.cnt=0;
	//printf("add a + b. cnt a=%d, cnt b=%d\n",a.cnt,b.cnt);
	for(i=0;i<LEN;i++)
		res.num[i]=0;
	for(i=0;i<LEN-1;i++){
		res.num[i]+=a.num[i]+b.num[i];
		if(i<a.cnt || i<b.cnt || res.num[i]!=0){
			res.cnt++;
			flag=0;
		}
		res.num[i+1]+=res.num[i]/10;
		res.num[i]%=10;
	}
	if(flag)
		res.cnt+=a.cnt+b.cnt;
	//printf("res.cnt=%d\n",res.cnt);
	return res;
}
void printlnum(lnum a){
	int c=a.cnt;
	for(c--;c>=0;c--)
		printf("%d",a.num[c]);
}
