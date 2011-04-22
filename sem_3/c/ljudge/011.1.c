//brackets new
#include <stdio.h>
#include <string.h>
#define LEN 100001
int mark;
char buf[LEN];
long st[4];
/* 0 - ()
 * 1 - {}
 * 2 - <>
 * 3 - []	
 * */
int is_right(char *);
void push(char c);
char pop();
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
				st[0]++;
				push(0);
				break;
			case ')':
				if(st[0]<=0 || pop()!=0)
					return 0;	
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