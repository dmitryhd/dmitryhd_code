#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#define WSIZE 40
#define MAXNUM 26
#define MAXCRLEN 2048

//Old version of simple substitution decrypt program.

#define openf(f,name,mode)\
if((f=fopen(name,mode))==NULL){\
	fprintf(stderr,"file error\n");\
	exit(1);\
}

typedef struct _wtree wtree;
struct _wtree{
	int is_word;
	wtree *a[26+1];
	char *word;
};
wtree head[26+1];// tree;
wtree *tmp;

wtree * mknod();
void freq_an(char *fname);
void addword(char *w);
void print_w(char c);
void init_wtree(char *dict_file);
int isword(char *w);
int num[MAXNUM+1]; 
int crypt[MAXCRLEN], crlen;

int main(int argc, char *argv[]){
	char opt,freq_print=0;
	char *opts="hfd:e:", *dict_file,*encrypt_file;
	while((opt=getopt(argc,argv,opts))!=-1){
		switch(opt){
			case 'h':
printf("-d file_name - dictionary\n\
-e file_name - encrypt file\n\
-f - print char freq\n");
				return 0;
				break;
			case 'e':
				encrypt_file=optarg;
				break;
			case 'd':
				dict_file=optarg;		
				break;
			case 'f':
				freq_print=1;		
				break;
		}
	}
	if(freq_print){
		int i;
		freq_an(encrypt_file);
		for(i=1;i<=MAXNUM;i++)
			printf("%d - %d\n",i,num[i]);
		return 0;
	}
	//init_wtree(dict_file);
	addword("ab");
	print_w('a');
	printf("%d\n",isword("ab"));
	//wtree *tmp=&head[c-'a'+1];
	return 0;
}
wtree * mknod(){
	wtree *new=(wtree *)malloc(sizeof(wtree));
	if(new==NULL)
		exit(2);
	return new;
}
void freq_an(char *fname){
	int i,a;
	FILE *f;
	openf(f,fname,"r");	
	for(i=0;i<=MAXNUM;i++)
		num[i]=0;
	for (i=1;(fscanf(f,"%d",&a))==1;i++)
		num[a]++;	
	//for(i=1;i<=MAXNUM;i++)
	//	printf("%d - %d\n",i,num[i]);
}

void addword(char *w){
	//w=(char *)tolower(w);
	int len=strlen(w),i;
	printf("add word %s\n",w);
	//head[w[0]-'a'].a[w[1]];	
	if(w[0]>='A' && w[0]<='Z')
		w[0]=w[0]-'A'+'a';
	
	printf("head[%d]\n",w[0]-'a'+1);
	tmp=&head[w[0]-'a'+1];
	printf("w[%d]=%c\n",0,w[0]);
	//tmp=tmp->a[]
	//printf("word =%s\n",w);
	for(i=1;i<len;i++){
		if(w[i]>='A' && w[i]<='Z')
			w[i]=w[i]-'A'+'a';
		tmp=tmp->a[w[i]-'a'+1];
		if(tmp==NULL){
			printf("mknod\n");
			tmp=mknod();
		}
	//	tmp->is_word=1;	
		printf("w[%d]=%c\n",i,w[i]);
		//if(i==len-1){
		//	tmp->word=w;
		//	break;
		//}
	}
	tmp=&head[w[0]-'a'+1];
	for(i=0;i<len;i++){
		if(tmp==NULL)
			printf("none\n");
		else{
			printf("est \n");
			tmp=tmp->a[w[i+1]-'a'+1];
		}
	}
	//if(tmp==NULL)
	//	tmp=mknod();
	//a=tmp->a[w[len-1]-'a'-1];
}
void print_w(char c){
	int i;
	wtree *tmp=&head[c-'a'+1];	
	for(i=0;i<=26;i++){
		if(tmp->a[i]!=NULL){
			printf("%c ",i+'a');	
		}
	}
}
void init_wtree(char *dict_file){
	FILE *f;
	char word[WSIZE+1];
	openf(f,dict_file,"r");
	printf("init tree\n");
	//printf("open usa\n");	
	while((fscanf(f,"%s",word))==1){
		addword(word);
	}
	fclose(f);
}

int isword(char *w){
	int len=strlen(w),i;
	tmp=&head[w[0]-'a'+1];

	printf("is word %s\n",w);
	for(i=1;i<len;i++){
		//if(tmp->is_word==0)
		//	return 0;
		
		tmp=tmp->a[w[i]-'a'+1];
		if(tmp==NULL)
			return 0;
	}
	return 1;	
}
