//brackets new
#include <stdio.h>
#include <string.h>
#define LEN 100001
int mark[4];
char buf[5][LEN];
unsigned long st[4];
/* 0 - ()
 * 1 - {}
 * 2 - <>
 * 3 - []	
 * */
int is_right(char *);
void push(char c,int type);
char pop(int type);
int main(int argc, char *argv[]){
	char str[LEN];
	scanf("%s",str);
	if(is_right(str))	
		printf("YES\n");
	else
		printf("NO\n");
	return 0;
}
int is_right(char *str){
	int i,len=strlen(str);
	for(i=0;i<len;i++){
		//printf("%c :\n",str[i]);
		switch( str[i]){
			case '(':
				push(0,0);
				push(0,4);
				break;
			case ')':
				if((pop(0))!=0 || pop(4)!=0)
					return 0;	
				break;
			case '{':
				push(1,1);
				last=1;
				break;
			case '}':
				if((pop(1))!=1 || last!=1)
					return 0;	
				break;
			case '<':
				push(2,2);
				last=2;
				break;
			case '>':
				if((pop(2))!=2 || last!=2)
					return 0;	
				break;
			case '[':
				push(3,3);
				last=3;
				break;
			case ']':
				if((pop(3))!=3 || last!=3)
					return 0;	
				break;
		}
	}
	for(i=0;i<4;i++)
		if(mark[i]!=0)
			return 0;
	return 1;
}
void push(char c, int type){
	//printf("push %d, %c\n",type,c);
	buf[type][mark[type]]=c;
	mark[type]++;
}
char pop(int type){
	if(mark[type]==0)
		return 0;
	mark[type]--;
	//printf("pop %d\n",type);
	return buf[type][mark[type]];
}
