#include "template.h"

/***********************************
 * copy and pastle this code for use
 * binary tree.
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
void bt_add(node ** head, node * new);
void bt_print(node *head);
int bt_find(node * head,int key);

int main(int argc, char *argv[]){
	node *a, *head=NULL;
	int i,j;

	for(i=0,j=5;i<5;i++,j--){
		a=NULL;
		a=(node*)malloc(sizeof(node));
		a->x=j;
		//printf("before - head=%d , a[%d]=%d\n",head,i,a[i]);
		bt_add(&head,a);
		//printf("after - head=%d \n",head);
	}
	for(i=6;i<11;i++){
		a=NULL;
		a=(node*)malloc(sizeof(node));
		a->x=i;
		//printf("before - head=%d , a[%d]=%d\n",head,i,a[i]);
		bt_add(&head,a);
		//printf("after - head=%d \n",head);
	}
	bt_print(head);
	printf("%d\n",bt_find(head,1));
	printf("%d\n",bt_find(head,6));
	printf("%d\n",bt_find(head,0));
	return SUCC;
}
void bt_print(node * head){
	int i;
	printf("x=%d\n",head->x);
	for(i=0;i<N;i++)
		if(head->branch[i]!=NULL)
			bt_print(head->branch[i]);
}
//binary sort tree
int bt_find(node * head,int key){
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
		return bt_find(head->branch[0],key);
	}
	else{
	//	printf("bigger\n");
		return bt_find(head->branch[1],key);
	}
}
//binary sort tree
//0-left (lesser)
void bt_add(node ** head, node * new){
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
		bt_add(&((*head)->branch[0]),new);	
	else
		bt_add(&((*head)->branch[1]),new);	
}
