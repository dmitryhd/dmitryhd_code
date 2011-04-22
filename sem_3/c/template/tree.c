#include "template.h"


/***********************************
 * copy and pastle this code for use
 * Template.
 * Tree.
 * ********************************/
/**********************************
 * modify this to contain data you need
 * ********************************/
#define N 2
typedef struct _node node;
struct _node{		
	node * branch[N];
	int x;
};
/**********************************
 * simple add, add in sorted tree
 * print atr tree (just debug)
 * and search for key
 * ********************************/
void tradd(node ** head,node *);
void traddsrt(node ** head, node * new);
void trprint(node *head);
int trfind(node * head,int key);
void trfree(node * head);
int trdel(node ** head, int key);
int trfind_sort(node * head,int key);

int main(int argc, char *argv[]){
	node *a, *head=NULL;
	int i,j;

	for(i=0,j=5;i<5;i++,j--){
		a=NULL;
		a=(node*)malloc(sizeof(node));
		a->x=j;
		//printf("before - head=%d , a[%d]=%d\n",head,i,a[i]);
		traddsrt(&head,a);
		//printf("after - head=%d \n",head);
	}
	for(i=6;i<11;i++){
		a=NULL;
		a=(node*)malloc(sizeof(node));
		a->x=i;
		//printf("before - head=%d , a[%d]=%d\n",head,i,a[i]);
		traddsrt(&head,a);
		//printf("after - head=%d \n",head);
	}
	trprint(head);
	printf("%d\n",trfind_sort(head,1));
	printf("%d\n",trfind_sort(head,6));
	printf("%d\n",trfind_sort(head,0));
	return SUCC;
}
void trprint(node * head){
	int i;
	printf("x=%d\n",head->x);
	for(i=0;i<N;i++)
		if(head->branch[i]!=NULL)
			trprint(head->branch[i]);
}
//free memory in tree
//????????????
void trfree(node * head){
	int i;
	for(i=0;i<N;i++)
		if(head->branch[i]!=NULL)
			trfree(head->branch[i]);
}
//delete any/or first nodeect with key
//return true - if del.
//???????????????
int trdel(node ** head, int key){
 	node * tmp=*head, * pr=NULL;
//	printf("del %d\n",key);
	while(tmp!=NULL){
		if(tmp->x==key){
			if(pr==NULL){ 	//from head	
			//	printf("del from head\n");
//				pr=tmp->next;
				free(*head);
				*head=pr;
			}
			else{		//from tail or between
			//	printf("del from tail or between\n");
//				pr->next=tmp->next;
				free(tmp);
			}	
			return true;
		}
		pr=tmp;
//		tmp=tmp->next;
		
	}
	return false;
}

int trfind(node * head,int key){
	int i;
	if(head->x==key){
		printf("find!\n");
		return true;
	}
	for(i=0;i<N;i++)
		if(head->branch[i]!=NULL)
			trfind(head->branch[i], key);
	return false;
}
//binary sort tree
int trfind_sort(node * head,int key){
	int i;
	if(head == NULL){
	//	printf("can't find\n");
		return false;
	}
	//printf("find: cur - %d for %d\n",head->x,key);
	if(head->x == key){
	//	printf("find!\n");
		return true;
	}
	if(head->x < key){
	//	printf("lesser\n");
		return trfind_sort(head->branch[0],key);
	}
	else{
	//	printf("bigger\n");
		return trfind_sort(head->branch[1],key);
	}
}
void tradd(node ** head, node * new){
	printf("add node %d\n",new->x);
	//printf("in tail\n");
	//trprint(*head);
}
//binary sort tree
//0-left (lesser)
void traddsrt(node ** head, node * new){
	//printf("add node %d\n",new->x);
	if(*head==NULL){
		//*head=(node*)malloc(sizeof(node));
		*head=new;
		int i;
		for(i=0;i<N;i++)
			(*head)->branch[i]=NULL;
		return;
	}

	if((*head)->x == new->x)
		return;
	if((*head)->x < new->x)
		traddsrt(&((*head)->branch[0]),new);	
	else
		traddsrt(&((*head)->branch[1]),new);	
}
