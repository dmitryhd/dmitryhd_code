#include <stdio.h>
#include <stdlib.h>
#define KEYLEN 10
#define MESLEN 10240000

// Old version of cicle substitution encrypt program
//
int mess[MESLEN], crypt[MESLEN], key[KEYLEN]={1,1,2,5,4,6,2,2,7,4};
int main(int argc, char *argv[]){
	int c,i=0,j,k,flag=1, file=0;
	FILE *f;
	char * filename;
	if(argv[1][1]=='e')
		flag=1;
	else if(argv[1][1]=='d')
		flag=-1;
	if(argv[1][2]=='f'){
		filename=argv[2];
		file=1;
	}
	if(file){
		f =fopen(filename,"r");
		if(f==NULL){
			printf("file err\n");
			exit(1);
		}
		while((c=getc(f))!=EOF)
			crypt[i++]=c;
		i--;
		fclose(f);
		
	}
	else{
		while((c=getchar())!=EOF)
			crypt[i++]=c;
		i--;
	}
	//printf("encrypt %d\n",flag);
	//printf("Я=%d я=%d, А=%d, а=%d\n",'Я','я','А','а');
	if(file){
		f =fopen(filename,"w");
		if(f==NULL){
			printf("file err\n");
			exit(1);
		}
	}
	for(j=0,k=0;j<i;j++,k++){
		if(k==9)
			k=0;
		//if((crypt[j]<='z' && crypt[j]>='A') || (crypt[j]<='я' && crypt[j]>='А')){
			mess[j]=crypt[j]+(flag)*key[k];
			if(file)
				fprintf(f,"%c",mess[j]);
			else
				printf("%c",mess[j]);
		//}
		//else 
		//	printf("%c = %d \n",crypt[j],crypt[j]);
	}
	if(file){
		fprintf(f,"\n");	
		fclose(f);
	}
	else 
		printf("\n");	
	return 0;
}
