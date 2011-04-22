//horse =)
#include <stdio.h>
#define LNUMLEN 1000

unsigned long int max_lev, lev;
typedef unsigned int ui;
typedef struct _lnum{
	char num[LNUMLEN];
	char sign;
	unsigned int cnt;
}lnum;
lnum cnt;
lnum ladd(lnum a, lnum b);
lnum a,b,c,o;
lnum an,bn,cn,on;

void set_knight();
void printlnum(lnum a);
int main(int argc, char *argv[]){
	scanf("%ld",&max_lev);
	if(max_lev==1){
		printf("8\n");
		return 0;
	}
	lev=1;
	a.cnt=b.cnt=c.cnt=o.cnt=1;
	a.num[0]=4;	
	b.num[0]=2;	
	c.num[0]=1;	
	o.num[0]=0;
/*	
	printf("beg:");
	printlnum(a);
	printf(" ");
	printlnum(b);
	printf(" ");
	printlnum(c);
	printf(" ");
	printlnum(o);
	printf(" \n");
*/
	set_knight();
	printlnum(cnt);
	printf("\n");	
	return 0;
}
void set_knight(){
	if(lev==max_lev){
		cnt=ladd(cnt,a);
		cnt=ladd(cnt,b);
		cnt=ladd(cnt,c);
		cnt=ladd(cnt,o);
		return;
	}
/*
	printf("lev=%ld :",lev);
	printlnum(a);
	printf(" ");
	printlnum(b);
	printf(" ");
	printlnum(c);
	printf(" ");
	printlnum(o);
	printf(" \n");
*/
	lev++;

	an=a;
	bn=b;
	cn=c;
	on=o;

	a=bn;
	a=ladd(a,bn);
	a=ladd(a,cn);
	a=ladd(a,cn);

	b=an;
	b=ladd(b,on);
	b=ladd(b,on);

	c=an;

	o=bn;

	set_knight();
}
lnum ladd(lnum a, lnum b){
	int i,flag=1;
	lnum res;
	res.sign=res.cnt=0;
	for(i=0;i<LNUMLEN;i++)
		res.num[i]=0;
	for(i=0;i<LNUMLEN-1;i++){
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
	return res;
}
void printlnum(lnum a){
	int c=a.cnt;
	for(c--;c>=0;c--)
		printf("%d",a.num[c]);
}
