#include "my_template.h"
#include "word.h"
int debug_level;
void trace(short level, char * format, ...){
	if(level<0 || level > debug_level)
		return;
	va_list args;
	va_start(args,format);
	vprintf(format,args);
	va_end(args);
}
void str2num(char *str,int *num, int *size){
	int i,l=strlen(str);
	for(i=0,*size=0;i<l;i++,(*size)++){
		if(isspace(str[i]))
			num[i]=str[i];
		else
			num[i]=str[i]-'a'+1;
	}
	
}
void num2str(int *num,char *str, int *size){
	int i;
	for(i=0;i<*size;i++)
		str[i]=num[i]+'a'-1;
}
void print_int(int *num,int size){
	int i;
	for(i=0;i<size;i++){
		if(isspace(num[i]))
			printf(" | ");
		else
			printf("%d ",num[i]);
	}
	printf("\n");
}
void init_tree(char *file){
	FILE *f;
	char w[WSIZE];
	openf(f,file,"r");
	while(fscanf(f,"%s",w)==1){
		to_lower(w);
		add_word(w);
	//	printf("%s\n",w);
	}
}
void add_word(char *word){
	int i,len=strlen(word);
	tmp=&root;
	for(i=0;i<len;i++){	
		if(tmp==0)
			tmp=mknod();	
		if(tmp->a[c2n(word[i])]==0)
			tmp->a[c2n(word[i])]=mknod();	
		tmp=tmp->a[c2n(word[i])];
		if(i==len-1){
			tmp->is_word=1;
		}
	}
}
wtree * mknod (){
	wtree *a;
	if((a=(wtree *)malloc(sizeof(wtree)))==NULL){
		printf("alloc_mem err\n");
		exit(2);
	}
	int i;
	a->is_word=0;
	for(i=0;i<=N;i++)
		a->a[i]=NULL;
	return a;
}
int c2n(char c){
	return c-'a'+1;
}
char n2c(int n){
	return n+'a'-1;
}
void to_lower(char *s){
	int i,len=strlen(s);
	for(i=0;i<len;i++)
		s[i]=tolower(s[i]);
}
int is_word(char *s, int *cnt){
	int i,l=strlen(s);
	*cnt=0;
	if(l==0 || l==1)
		return 0;
	tmp=&root;
	//printf("is word %s?\n",s);
	for(i=0;i<l;i++){
		tmp=tmp->a[c2n(s[i])];
		if(tmp==0)
			return 0;
		//printf("s[%d]=%c,is_word=%d\n",i,s[i],tmp->is_word);
		if(i==l-1 && tmp->is_word==1){
			*cnt=l;
			//printf("yes!\n");
			return 1;
		}
	}
	return 0;
}
int is_any_word(char *s, int *num){
	*num=0;
	int res=0, i,j, c=0, l=strlen(s);
	char tmp[STRLEN];
	trace(3,"is any word | %s \n",s);
	for(i=0;i<l;i++){
		for(j=0;;j++){
			if(isspace(s[j+i])){
				if(j!=0)
					i+=j-1;
				tmp[j+1]=0;
				break;
			}
			tmp[j]=s[j+i];
		}
		//got word
		if(is_word(tmp,&c)){
			//printf("c=%d\n",c);
			res++;
			*num+=c;
			trace(1,"word %s\n",tmp);
			//trace(3,"num=%d, i=%d\n",*num,i);
		}
	}
	return res;
}
