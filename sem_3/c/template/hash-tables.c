#include "template.h"

/***********************************
 * copy and pastle this code for use
 * hash tables
 * ********************************/
/**********************************
 * modify this to contain data you need
 * ********************************/
typedef struct _obj obj;
struct _obj{		
	obj * next;
	int x;
};

typedef struct _hash_table hash_table;
struct _hash_table{
	obj **basket;	//array of pointers to object
			//objects can be simple integer, 
			//linked list or tree ...
	unsigned long hashsize;
};
int hash(hash_table * ht,int x);
void ht_insert(hash_table *ht, obj * x);
void ht_print(hash_table ht);
int ht_search(hash_table *ht, int x);
int ht_del(hash_table *ht, obj *x);
int main(){
	obj *a;
	hash_table ht;
	ht.basket = NULL;
	ht.hashsize = 9;
	int i;

	for(i=0;i<5;i++){
		a=NULL;
		a=(obj*)malloc(sizeof(obj));
		a->x=i;
		ht_insert(&ht,a);
	}
	for(i=10;i<15;i++){
		a=NULL;
		a=(obj*)malloc(sizeof(obj));
		a->x=i;
		ht_insert(&ht,a);
	}
	ht_print(ht);
	printf("0 - %d\n",ht_search(&ht,0));
	a=NULL;
	a=(obj*)malloc(sizeof(obj));
	a->x=0;
	ht_del(&ht,a);
	a->x=10;
	ht_del(&ht,a);
	printf("0 - %d\n",ht_search(&ht,0));
	printf("10 - %d\n",ht_search(&ht,10));
	printf("14 - %d\n",ht_search(&ht,14));
	printf("20 - %d\n",ht_search(&ht,20));
	printf("-1 - %d\n",ht_search(&ht,-1));
	ht_print(ht);
	return SUCC;
}
int ht_del(hash_table *ht, obj *x){
	int key = x->x;
	int h = hash(ht, key);
	if(ht_search(ht,key) == false)
		return false;
 	obj * tmp=ht->basket[h], * pr=NULL;
	printf("del %d\n",key);
	while(tmp!=NULL){
		if(tmp->x==key){
			if(pr==NULL){ 	//from head	
				pr=tmp->next;
				free(ht->basket[h]);
				ht->basket[h]=pr;
			}
			else{		//from tail or between
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
int ht_search(hash_table *ht, int x){
	int h = hash(ht, x);
	if(ht->basket == NULL)
		return false;
	obj *tmp=ht->basket[h];
	while(tmp!=NULL){
		if(tmp->x==x)
			return true;
		tmp=tmp->next;
	}
	return false;
}

void ht_insert(hash_table *ht, obj * x){
	int h = hash(ht, x->x);
	if(ht_search(ht,x->x))	//add unique objects
		return;
	if(ht->basket == NULL){
		printf("create new table\n");
		alloc_mem(ht->basket,obj *,ht->hashsize);
		int i;
		for(i=0;i<ht->hashsize;i++)
			ht->basket[i] = NULL;
	}
	obj * tmp = ht->basket[h];
	obj * pr = tmp;
	if(tmp == NULL){
		printf("in head\n");
		ht->basket[h] = x;
		(ht->basket[h])->next = NULL;
		return;
	}
	printf("not in head\n");
	while(tmp!=NULL){
		pr=tmp;
		tmp=tmp->next;
	}
	x->next=NULL;
	pr->next=x;
}
void ht_print(hash_table ht){
	int sz = ht.hashsize;
	int i;
	for(i=0;i<sz;i++)
		if(ht.basket[i] != NULL){
			obj * tmp=ht.basket[i];
			printf("basket %d: ",i);
			while(tmp!=NULL){
				printf("%d ",tmp->x);
				tmp=tmp->next;
			}
			printf("\n");
		}
}

int hash(hash_table * ht,int x){	//sample hash function
	return abs(x%ht->hashsize);
}
