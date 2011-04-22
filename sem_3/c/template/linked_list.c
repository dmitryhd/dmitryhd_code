#include "template.h"

/***********************************
 * copy and pastle this code for use
 * Template.
 * Linked list of objects
 * ********************************/
/**********************************
 * modify this to contain data you need
 * ********************************/
typedef struct _obj obj;
struct _obj{		
	obj * next;
	int x;
};
/**********************************
 * simple add, add in sorted linked list
 * print all list (just debug)
 * and search for key
 * ********************************/
void lladd(obj ** head,obj *);
void lladdsrt(obj ** head, obj * new);
void llprint(obj *head);
int llfind(obj * head,int key);
void llfree(obj * head);
int lldel(obj ** head, int key);
int llfind_sort(obj * head,int key);

int main(int argc, char *argv[]){
	obj *a, *head=NULL;
	int i,j;

	for(i=0,j=5;i<5;i++,j--){
//		alloc_mem(a,obj,1);
		a=NULL;
		a=(obj*)malloc(sizeof(obj));
		a->x=j;
		//printf("before - head=%d , a[%d]=%d\n",head,i,a[i]);
		lladdsrt(&head,a);
		//printf("after - head=%d \n",head);
	}
	llprint(head);
	lldel(&head,4);
	llprint(head);
	llfree(head);
	return SUCC;
}
void llprint(obj * head){
 	obj * tmp=head;
	printf("+++++++print jobs++++++++\n");
	while(tmp!=NULL){
		printf("%d\n",tmp->x);
		tmp=tmp->next;
	}
	printf("+++++++++++++++++++++++++\n");
}
//free memory in list
void llfree(obj * head){
 	obj * tmp=head, *next=NULL;
	while(tmp!=NULL){
		next=tmp->next;
		free(tmp);
		tmp=next;
	}
}
//delete any/or first object with key
//return true - if del.
int lldel(obj ** head, int key){
 	obj * tmp=*head, * pr=NULL;
//	printf("del %d\n",key);
	while(tmp!=NULL){
		if(tmp->x==key){
			if(pr==NULL){ 	//from head	
			//	printf("del from head\n");
				pr=tmp->next;
				free(*head);
				*head=pr;
			}
			else{		//from tail or between
			//	printf("del from tail or between\n");
				pr->next=tmp->next;
				free(tmp);
			}	
			return true;
		}
		pr=tmp;
		tmp=tmp->next;
		
	}
	return false;
}

int llfind(obj * head,int key){
	obj *tmp=head;
	while(tmp!=NULL){
		if(tmp->x==key)
			return true;
		tmp=tmp->next;
	}
	return false;
}
int llfind_sort(obj * head,int key){
	obj *tmp=head;
	while(tmp!=NULL && tmp->x <= key){ //comparison
		if(tmp->x==key)
			return true;
		tmp=tmp->next;
	}
	return false;
}
void lladd(obj ** head, obj * new){
	printf("add obj %d\n",new->x);
	obj * tmp=*head, * pr=*head;
	if(tmp==NULL){
		*head=new;
		(*head)->next=NULL;
		//printf("in head\n");
		//llprint(*head);
		//printf("change - head=%d , new=%d\n",*head,new);
		return;
	}
	while(tmp!=NULL){
		pr=tmp;
		tmp=tmp->next;
	}
	new->next=NULL;
	pr->next=new;
	//printf("in tail\n");
	//llprint(*head);
}
void lladdsrt(obj ** head, obj * new){
	printf("add obj %d\n",new->x);
	obj * tmp=*head, * pr=NULL;
	if(tmp==NULL){
		*head=new;
		(*head)->next=NULL;
		//printf("in head\n");
		//llprint(*head);
		//printf("change - head=%d , new=%d\n",*head,new);
		return;
	}
	while(tmp!=NULL){
		if(tmp->x >= new->x){	//sort
			if(pr == NULL){	//add in head	
				new->next=*head;
				*head=new;
				return;
			}
			pr->next=new;	//add between
			new->next=tmp;
			return;
		} 
		pr=tmp;
		tmp=tmp->next;
	}
	new->next=NULL;			//add to tail
	pr->next=new;
	//printf("in tail\n");
	//llprint(*head);
}
