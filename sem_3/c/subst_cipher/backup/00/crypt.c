#include "my_template.h"
#include "crypt.h"
#include "word.h"
int debug_level;

/****************************************
 *Crypt functions
 *BUG:
 ***************************************/
void decr_str(char *enstr,char *str,int *sub){
	int i,len=strlen(enstr),tmp;
	trace(5,"\tdecrypt string: %s\n",enstr);
	for(i=0;i<len;i++){
		trace(5,"i=%d, c=%c\n",i,enstr[i]);
		if(isspace(enstr[i])){
			str[i]=enstr[i];
		}
		else{
			tmp=c2n(enstr[i]);
			//printf("tmp=%d\n",tmp);
			str[i]=n2c(sub[tmp]);	
		}
	}
	trace(5,"\tto: %s\n",str);
	str[i]=0;
}
void encr_str(char *str,char *enstr, int * sub){
	int i,len=strlen(str),tmp,j;
	for(i=0;i<len;i++){
		if(isspace(str[i])){
			enstr[i]=str[i];
		}
		else{
			tmp=c2n(str[i]);
			for(j=1;j<=N;j++)
				if(sub[j]==tmp)
					break;
			enstr[i]=n2c(j);	
		}
	}
	enstr[i]=0;
}
