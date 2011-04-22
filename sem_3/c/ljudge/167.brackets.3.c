//brackets III
#include <stdio.h>
#include <string.h>
#define LEN 1000001
long mark;
char buf[LEN];
long st[4];
/* 0 - ()
 * 1 - {}
 * 2 - <>
 * 3 - []	
 * */
int is_right(char *, long,long);
void push(char c);
char pop();
int main(int argc, char *argv[]){
	long n,i,beg,end;
	char str[LEN];
	scanf("%s\n",str);
	scanf ("%ld\n",&n);
	for(i=0;i<n;i++){
		scanf("%ld %ld\n",&beg,&end);
		if(is_right(str,beg,end))	
			printf("Y\n");
		else
			printf("N\n");
	}
	return 0;
}
int is_right(char *str,long beg,long end){
	long i;
	//printf("from %ld %c to %ld %c\n",beg-1,str[beg-1],end-1,str[end-1]);
	for(i=0;i<4;i++)
		st[i]=0;
	mark=0;
	for(i=beg-1;i<end;i++){
		//printf("%c :\n",str[i]);
		switch( str[i]){
			case '(':
				st[0]++;
				push(0);
				break;
			case ')':
				if(st[0]<=0 || pop()!=0){
					return 0;	
				}
				st[0]--;
				break;
			case '{':
				st[1]++;
				push(1);
				break;
			case '}':
				if(st[1]<=0 || pop()!=1)
					return 0;	
				st[1]--;
				break;
			case '<':
				st[2]++;
				push(2);
				break;
			case '>':
				if(st[2]<=0 || pop()!=2)
					return 0;	
				st[2]--;
				break;
			case '[':
				st[3]++;
				push(3);
				break;
			case ']':
				if(st[3]<=0 || pop()!=3)
					return 0;	
				st[3]--;
				break;
		}
	}
	for(i=0;i<4;i++)
		if(st[i]!=0)
			return 0;
	return 1;
}
void push(char c){
	//printf("push %d\n",c);
	buf[mark]=c;
	mark++;
}
char pop(){
	if(mark==0)
		return -1;
	mark--;
	//printf("pop %d\n",buf[mark]);
	return buf[mark];
}
