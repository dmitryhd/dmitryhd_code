#include <stdio.h>
#include <string.h>
#include <math.h>
#define BUF 255
#define MAXN 9
int main(int argc, char *argv[]){
	int i,j,len,cnt=0,a;
	long x=0;
	long double res=0.0;
	char str[BUF], num[MAXN+1];	
	//scanf("%s",str);
	len=0;
	while(scanf("%c",&str[len++])==1);
	len--;
	//printf("%s\n",str);
	for(i=0;i<len;i++){
		//printf("a[%d]=%c\n",i,str[i]);
		char sign=1;
		char flag=0;
		if(str[i]=='('){
			cnt++;
			continue;
		}
		if(str[i]==')'){
			cnt--;
			continue;
		}
		if(str[i]=='-'){
		}
		else if(str[i]>'9' || str[i]<'0')
			continue;
		
	//	printf("got, i=%d\n",i);
		for(x=0,j=0;(str[i]<='9' && str[i]>='0')||str[i]=='-';i++,j++){
			if(str[i]=='-'){
				sign=-1;
				j--;
				continue;
			}
		//	printf("a[%d]=%c\n",i,str[i]);
			num[MAXN-j]=str[i]-'0';
	//		printf("num[%d]=%d\n",MAXN-j,num[MAXN-j]);
			flag=1;
		}
		a=(--j);
		x=0;
	//	printf("a=%d\n",a);
		for(j=0;j<=a;j++)
			x+=num[MAXN-a+j]*pow(10,j);
	//	printf("sign=%d\n",sign);
		x*=sign;
	//	printf("get x=%ld, cnt=%d, x*pow(1/2,cnt)=%f\n",x,cnt,pow(0.5,cnt));
		res+= (x * pow(0.5,cnt));
	//	printf("res=%Lf\n",res);
		if(flag)
			i--;
	}
	printf("%.2Lf\n",res);
	return 0;
}
