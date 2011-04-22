/* Title: myecho */
/* Short description: try to write full analog of unix's echo*/
/* Last modification: DmitryKh - 12.09.09*/
/* Written by Dmitry Khodakov gr. 817*/

/* I hope, it's clear code =) */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

int is_octal_num(char * str, int pos, int len)
{
	int i = pos, cnt = 0, flag = 0;
	int oct[3] = {0, 0, 0};
	int num= 0;
	for( i = pos; i < pos + 3; i++){
		if(i < len && isdigit(str[i])){
			if(str[i] == '8' || str[i] == '9')
				break;
			if(str[i] == '0' && !flag)
				continue;
			oct[cnt++] = str[i] - '0';
			flag = 1;
		}
		else
			break;
	}
	for( i = cnt-1; i>=0; i--)
		num += (oct[i]*pow(8,cnt-1-i));	
	if(cnt)
		putchar(num);
	return cnt;
}

void my_print( char *str, int is_esc){
	int i, len;
	if( !is_esc){
		printf("%s",str);
		return;
	}
	len = strlen(str);
	for( i=0;i<len;i++){
		if(str[i] == '\\'){
			i++;
			switch( str[i]){
				case 'a':
					putchar('\a');
					break;
				case 'b':
					putchar('\b');
					break;
				case 'f':
					putchar('\f');
					break;
				case 'r':
					putchar('\r');
					break;
				case 't':
					putchar('\t');
					break;
				case 'n':
					putchar('\n');
					break;
				case 'v':
					putchar('\v');
					break;
				case '0':
					i+=is_octal_num(str,i+1,len);
					break;


			}
		}
		else
			putchar(str[i]);
	}
}

int main(int argc, char *argv[])
{
	int i;
	int do_newline = 1;
	int is_esc = 0;
	int  opt_c = 0;
	if(argc > 1 && argv[1][0] == '-'){
		for( i=1;i<strlen(argv[1]);i++){
			switch(argv[1][i]){
				case 'n':
					do_newline = 0;
					break;
				case 'e':
					is_esc = 1;
					break;
				case 'E':
					is_esc = 0;
					break;
				default:
					is_esc = 0;
					do_newline = 1;
					goto out;
			}
		}
		opt_c++;
	}
	out:
	for( i = 1 + opt_c;i<argc;i++){
		if(i>1+opt_c && i<=argc-1)
			printf(" ");
		my_print(argv[i],is_esc);
	}
	if(do_newline)
		printf("\n");
	return 0;
}
