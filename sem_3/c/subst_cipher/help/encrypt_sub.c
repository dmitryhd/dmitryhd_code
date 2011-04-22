#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define SSIZE 2048
#define MAXNUM 26
#define MAXCRLEN 2048

//Old version of simple substitution encrypt program.

#define openf(f,name,mode)\
if((f=fopen(name,mode))==NULL){\
	fprintf(stderr,"file error\n");\
	exit(1);\
}

char sub[MAXNUM];
//encr - open

void get_sub(char *);
void print_help(char *);
void print_sub();
void encr_str(char *str, char *en);
void decr_str(char *enstr,char *str);

int main(int argc, char *argv[]){
	char opt;
	char *opts="hs:e:d:", *sub_file,*encrypt_file=NULL,*decrypt_file=NULL;
	while((opt=getopt(argc,argv,opts))!=-1){
		switch(opt){
			case 'h':
				print_help(argv[0]);
				return 0;
				break;
			case 'e':
				encrypt_file=optarg;
				break;
			case 'd':
				decrypt_file=optarg;
				break;
			case 's':
				sub_file=optarg;
				get_sub(sub_file);		
				break;
		}
	}
	//print_sub();
	/*char enc[SSIZE],dec[SSIZE];
	encr_str("abc",enc);
	printf("%s\n",enc);
	decr_str(enc,dec);
	printf("%s\n",dec);
	*/
	FILE *f;
	char open[SSIZE],c,encr[SSIZE];
	int i=0;
	if(encrypt_file!=NULL){
		openf(f,encrypt_file,"r");
		while((fscanf(f,"%c",&c))==1 && c<='z' &&c>='a'){
			open[i]=c;
			i++;	
		}
		fclose(f);
		encr_str(open,encr);
		printf("%s",encr);
	}
	if(decrypt_file!=NULL){
		openf(f,decrypt_file,"r");
		while((fscanf(f,"%c",&c))==1 && c<='z' &&c>='a'){
			encr[i]=c;
			i++;	
		}
		fclose(f);
		decr_str(encr,open);
		printf("%s",open);
	}
	return 0;
}
void print_help(char *prog_name){
	printf("usage: %s -s sfile -e efile\n \
\t-s file_name - dictionary\n\
\t-e file_name - encrypt file\n",prog_name);
}
void get_sub(char *file){
	FILE *f;
	int n;
	char c;
	openf(f,file,"r");
	while((fscanf(f,"%d - %c ",&n,&c))==2){
		if(c>='A' && c<='Z')
			c=c-'A'+'a';
		sub[n]=c-'a'+1;
	}
	fclose(f);
}
void print_sub(){
	int i;
	for(i=1;i<=MAXNUM;i++)
		printf("%d = %d\n",i,sub[i]);
}
void encr_str(char *str,char *enstr){
	int i,len=strlen(str),tmp,j;
	for(i=0;i<len;i++){
		tmp=str[i]-'a'+1;
	//	printf("tmp=%d\n",tmp);
		for(j=1;j<=MAXNUM;j++)
			if(sub[j]==tmp)
				break;
	//	printf("j=%d\n",j);
		enstr[i]=j+'a'-1;	
	}
	enstr[i]=0;
}
void decr_str(char *enstr,char *str){
	int i,len=strlen(enstr),tmp;
	for(i=0;i<len;i++){
		tmp=enstr[i]-'a'+1;
	//	printf("tmp=%d\n",tmp);
		
	//	printf("j=%d\n",j);
		str[i]=sub[tmp]+'a'-1;	
	}
	str[i]=0;
}
