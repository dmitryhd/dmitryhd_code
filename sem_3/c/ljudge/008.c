//brackets
#include <stdio.h>
#include <string.h>
#define LEN 4002
char buf[LEN];
int mark;
int is_right(char *);
void push(char c);
char pop();
int main(int argc, char *argv[]){
	char str[LEN];
	scanf("%s",str);
	if(is_right(str))	
		printf("YES");
	else
		printf("NO");
	return 0;
}
int is_right(char *str){
	int i,len=strlen(str);
	char c;
	for(i=0;i<len;i++){
		if(str[i]=='(')
			push('(');
		else if((c=pop())!='(')
			return 0;	
	}
	if(mark==0)
		return 1;
	return 0;
}
void push(char c){
	buf[mark]=c;
	mark++;
}
char pop(){
	if(mark==0)
		return 0;
	mark--;
	return buf[mark];
}
