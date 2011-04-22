#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define LEN 201

typedef struct _lnum{
	char num[LEN];
	char sign;
	char cnt;
}lnum;
lnum ladd(lnum a, lnum b);
lnum lsub(lnum a, lnum b);
lnum int2lnum(unsigned long int a);
lnum lmul(lnum a, lnum b);
lnum _ldiv(lnum a, lnum b, lnum *c);
void printlnum(lnum a);
lnum getlnum(char *a);
lnum a, b, c;

int main(int argc, char *argv[]){
	lnum x, y, z;
	x=getlnum(argv[1]);	
	printlnum(x);
	printf("\n");
	y=getlnum(argv[2]);	
	printlnum(y);
	printf("\n");
	z=ladd(x,y);
	printlnum(z);
	printf("\n");
	z=lsub(x,y);
	printlnum(z);
	printf("\n");
	z=lmul(x,y);
	printlnum(z);
	printf("\n");
	return 0;
}
lnum getlnum(char *a){
	lnum res;
	int i;
	for(i=0;i<LEN;i++)
		res.num[i]=0;
	res.sign=0;
	int len=strlen(a);
	len--;
	for(res.cnt=0;len>=0;len--,res.cnt++)
		res.num[res.cnt]=a[len]-'0';
	return res;
}
void printlnum(lnum a){
	int c=a.cnt;
	for(c--;c>=0;c--)
		printf("%d",a.num[c]);
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
lnum lsub(lnum a, lnum b){
	int i,flag=1;
	lnum res;
	res.sign=res.cnt=0;
	for(i=0;i<LEN;i++)
		res.num[i]=0;
	for(i=0;i<LEN-1;i++){
		res.num[i]+=a.num[i]-b.num[i];
		if(res.num[i]!=0){
			//printf("a-b =%d\n",a.num[i]-b.num[i]);
			res.cnt++;
			flag=0;
		}
		if(res.num[i]<0){
			res.num[i+1]--;
			res.num[i]+=10;
		}
		res.num[i]%=10;
	}
	if(flag)
		res.cnt++;
	return res;
}
lnum lmul_(lnum a, int b){
	int i,flag=1;
	lnum res;
	res.sign=res.cnt=0;
	for(i=0;i<LEN;i++)
		res.num[i]=0;
	for(i=0;i<LEN-1;i++){
		res.num[i]+=a.num[i]*b;
		if(i<a.cnt || res.num[i]!=0){
			res.cnt++;
			flag=0;
		}
		res.num[i+1]+=res.num[i]/10;
		res.num[i]%=10;
	}
	if(flag)
		res.cnt+=a.cnt+1;
	return res;
}
lnum lmul(lnum a, lnum b){
	int i,j,curr_b;
	lnum res,tmp;
	res.sign=res.cnt=0;
	for(i=0;i<LEN;i++)
		res.num[i]=0;
	//printf("mul\n");
	for(i=0;i<b.cnt;i++){
		curr_b=b.num[i];
		tmp=lmul_(a,curr_b);
	//	printf("mul a * %d =",curr_b);
	//	printlnum(tmp);
	//	printf("\n");
		for(j=tmp.cnt-1;;j--){
			tmp.num[j+i]=tmp.num[j];
			if(j==0)
				break;
		}
		for(j=j+i-1;j>=0;j--)
			tmp.num[j]=0;
		tmp.cnt+=i;
	//	printf("shift tmp =>");
	//	printlnum(tmp);
	//	printf("\n");
		res=ladd(tmp,res);
	}
	return res;
		
}
lnum int2lnum(unsigned long int a){
	lnum res;
	int i=0;
	do{
		res.num[i]=a%10;
		i++;			
	}
	while((a/=10)!=0){

}
