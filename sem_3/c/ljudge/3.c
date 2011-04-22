#include <stdio.h>
#include <stdlib.h>

typedef struct _res res;
struct _res{
	res *next;
	long val;
};
res *head, *tmp;

void print_res(){
	while(tmp!=NULL){
		printf("%ld ",tmp->val);
		tmp=tmp->next;
	}
}

void add(long x){
	res *pr=NULL;
	tmp=head;
	int flag=0;
//	printf("add %ld\n",x);
	if(tmp==NULL){
		head=(res *)malloc(sizeof(res));
		head->next=NULL;
		head->val=x;
		return;
	}
	while(tmp!=NULL){
		if(tmp->val==x){
//			printf("reject\n");
			return;
		}
		pr=tmp;
		tmp=tmp->next;
	}
	tmp=(res *)malloc(sizeof(res));
	pr->next=tmp;
	tmp->next=NULL;
	tmp->val=x;
	return;
}
void del(long x){
	res *pr;
	int flag=0;
	tmp=head;
	//printf("del %ld\n",x);
	while(tmp!=NULL){
		if(tmp->val==x){
			if(flag){
				pr->next=tmp->next;
			}
			else{
				tmp=head->next;
				free(head);
				head=tmp;
				//printf("find in head\n");
				//print_res();
				//printf("\n");
			}
			return;
		}
		pr=tmp;
		tmp=tmp->next;
		flag=1;
	}
}

int main(int argc, char *argv[]){
	long a;
	while(1){
		scanf("%ld ",&a);
		if(a==-1)
			break;
		add(a);
	}
	while(1){
		scanf("%ld ",&a);
		if(a==-1)
			break;
		del(a);
	}
	print_res();
	return 0;
}
