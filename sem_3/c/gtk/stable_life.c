/* 	conway's game of live
 *  	Khodakov Dmitry - 817 gr.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "stable_life.h"
#define FIELD_VIS 24

//just for hash function
#define NUM1 9
#define NUM2 10
#define NUM3 9

//error codes
#define SUCCESS 0
#define ERR_CANNOT_OPEN_FILE 1
#define ERR_WRONG_DATA 2
#define ERR_ZERO_LENGTH 3
#define ERR_MEM 4

typedef struct list_struct cell_list;
typedef long int coord;
typedef struct quenue que;
typedef struct hash_table_struct hash_basket;

//for hash table - pool & count of cell_list cells type;
typedef unsigned int cell_cnt;

void print_que();
struct list_struct{
	cell_list *next;
	coord x;
	coord y;
};
struct quenue{
	coord x;
	coord y;
	unsigned char alive;
};
cell_list * head;//next +
cell_list * p, * tmp;
cell_cnt max_q;
short debug_level=3;
unsigned long int turn, live_cells;
unsigned int birth, death;
int quiet_mode, print_all, print_f, step_mode;

//queue of uncalculated cells ();
que * q; 

struct hash_table_struct{
	cell_list * head;
};
//this is hash_table;
hash_basket ht[NUM2*(NUM1+1)];

void print_field();
void next_turn();
int search_cell(coord in_x, coord in_y);
void add_cell(coord in_x, coord in_y);	
void del_live(coord in_x, coord in_y);
void print_live();
int hash(coord x, coord y);
void trace(short level, char * format, ...){
	if(level<0 || level > debug_level)
		return;
	va_list args;
	va_start(args,format);
	vprintf(format,args);
	va_end(args);
}
void print_live(){
	int i;
	for(i=0;i<NUM2*(NUM1+1);i++){
		tmp=ht[i].head;
		while(tmp!=NULL){
			printf("x=%ld, y=%ld\n",tmp->x,tmp->y);
			tmp=tmp->next;
		}	
	}
}
void del_all_live(){
	int i;
	cell_list *tmp1,*tmp2;
	for(i=0;i<NUM2*(NUM1+1);i++){
		tmp1=ht[i].head;
		while(tmp1!=NULL){
			tmp2=tmp1->next;
			del_live(tmp1->x,tmp1->y);
			tmp1=tmp2;
			printf("del x=%ld, y=%ld\n",tmp->x,tmp->y);
		}	
	}
}
void print_turn(){
	if(print_all)
		printf("Turn %ld, Live %ld Death %d Birth %d\n",turn, live_cells, death, birth);
}

int hash(coord x, coord y){
	return abs(NUM2*(x%NUM1)+(y%NUM3));
}

void add_cell(coord in_x, coord in_y){	
	//if list_struct is empty
	if(search_cell(in_x,in_y))
		return;
	live_cells++;
	birth++;
	cell_list *pr; //save pointer to previous cell;
	int h=hash(in_x,in_y);
	if(ht[h].head==0){
		ht[h].head=(cell_list *)malloc(sizeof(cell_list));
		if(ht[h].head==NULL){
			printf("cell_list err mem\n");
			exit(1);
		}
		ht[h].head->x=in_x;
		ht[h].head->y=in_y;
		ht[h].head->next=NULL;
		return;
	}
	tmp=ht[h].head;
	while(tmp && tmp->x < in_x ){
		pr=tmp;
		tmp=tmp->next;
	}
	if(tmp && tmp->x==in_x){
		while(tmp && tmp->y<in_y){
			pr=tmp;
			tmp=tmp->next;
		}
		p=(cell_list *)malloc(sizeof(cell_list));
		if(p==NULL){
			printf("cell_list err mem\n");
			exit(1);
		}
		if(!tmp){
			p->next=tmp;
			pr->next=p;		
			p->x=in_x;
			p->y=in_y;
			return;
		}
		p->next=tmp->next;
		p->x=in_x;
		p->y=in_y;
		tmp->next=p;
	       	return;	
		//insert
	}
	p=(cell_list *)malloc(sizeof(cell_list));
	if(p==NULL){
		printf("cell_list err mem\n");
		exit(1);
	}
	if(!tmp){
		p->next=tmp;
		pr->next=p;		
		p->x=in_x;
		p->y=in_y;
		return;
	}
	int tmp_x=tmp->x;
	//if x>in_x
	while(tmp && tmp->x==tmp_x && tmp->y<in_y){
			pr=tmp;
			tmp=tmp->next;
	}
	if(!tmp){
		p->next=tmp;
		pr->next=p;		
		p->x=in_x;
		p->y=in_y;
		return;
	}
	p->next=tmp->next;
	p->x=in_x;
	p->y=in_y;
	tmp->next=p; 
}
void del_live(coord in_x, coord in_y){
	int h=hash(in_x,in_y);
	tmp=ht[h].head;
	if(ht[h].head==NULL)
		return;
	while(tmp!=NULL ){
		if(tmp->x == in_x && tmp->y == in_y){
			live_cells--;
			death++;
			if(tmp==ht[h].head){
				ht[h].head=tmp->next;
				free(tmp);
				return;
			}
			p->next=tmp->next;
			free(tmp);			
			return;
		}
		if(tmp->next==NULL)
			return;
		p=tmp;
		tmp=tmp->next;
	}
}
int search_cell(coord in_x, coord in_y){	
	int h=hash(in_x,in_y);
	tmp=ht[h].head;
	while (tmp!=NULL ){
		if(tmp->x==in_x && tmp->y==in_y)
			return 1;
		tmp=tmp->next;
	}
	return 0;
}
void check_field(coord x, coord y){
	int res=0;
	//printf("check field\n");
	res+=search_cell(x,y+1);
	res+=search_cell(x,y-1);
	res+=search_cell(x+1,y);
	res+=search_cell(x+1,y+1);
	res+=search_cell(x+1,y-1);
	res+=search_cell(x-1,y);
	res+=search_cell(x-1,y-1);
	res+=search_cell(x-1,y+1);

	if( search_cell(x,y) && (res<2 || res>3) ){
		max_q++;
		q=(que*)realloc(q,max_q*sizeof(que));
		if(q==NULL){
			fprintf(stderr,"MEM ERR q\n");
			exit(1);	
		}
		q[max_q-1].x=x;
		q[max_q-1].y=y;
		q[max_q-1].alive=0;
		return;
	}
	if( !search_cell(x,y) && res==3 ){
		max_q++;
		q=(que*)realloc(q,max_q*sizeof(que));
		if(q==NULL){
			fprintf(stderr,"MEM ERR q\n");
			exit(1);	
		}
		q[max_q-1].x=x;
		q[max_q-1].y=y;
		q[max_q-1].alive=1;
		return;
	}
}

void next_turn(){
	q=NULL;
	max_q=0;
	int i;
	coord x, y, j;
	cell_list * it;
	death=birth=0;
	//printf("next turn\n");
	for(i=0;i<NUM2*(NUM1+1);i++){
		it=ht[i].head;
	//	printf("\t i=%d\n",i);
		if(it==NULL){
			continue;
		}
		while(it!=NULL ){
			x=it->x;
			y=it->y;
			check_field(x+1,y);
			check_field(x+1,y+1);
			check_field(x+1,y-1);
			check_field(x-1,y);
			check_field(x-1,y+1);
			check_field(x-1,y-1);
			check_field(x,y+1);
			check_field(x,y-1);
			check_field(x,y);
			it=it->next;
		}
	}
	print_que();
	for(j=0;j<max_q;j++)
		if(q[j].alive==1)
			add_cell(q[j].x,q[j].y);
		else
			del_live(q[j].x,q[j].y);
	free(q);
	turn++;
}

void print_field(){
	coord x,y;
	int i;
	for(i=0;i<FIELD_VIS;i++)
		printf("-");
	printf("\n");
	for(y=-FIELD_VIS/2;y<FIELD_VIS/2;y++){
		for(x=-FIELD_VIS/2;x<FIELD_VIS/2;x++)
			if(search_cell(x,y)==1)
				printf("*");
			else 
				printf(" ");
		printf("\n");
	}
	for(i=0;i<FIELD_VIS;i++)
		printf("-");
	printf("\n");
}

int load_file(char *file_name){
	FILE *f=NULL;
	int x, y;
	int cnt, cnt_x;
	int i;
	int a=0, b=0;
	char c;
	trace(4,"load_file %s\n",file_name);
	if((f=fopen(file_name,"r"))==NULL){
		trace(1,"can't open file %s\n", file_name);
		return ERR_CANNOT_OPEN_FILE;
	}
	fscanf(f,"x = %d, y = %d ",&x,&y);
	trace(4,"x=%d, y=%d\n",x,y);
	cnt_x=x;
	do{
		if( fscanf(f,"%d",&cnt)==0){
			cnt=1;
		}
		trace(4,"cnt=%d ,all=%d\n",cnt,cnt_x);
		c=fgetc(f);
		trace(4,"c=%c\n",c);
		if(c=='o'){
			cnt_x-=cnt;
			for(i=0;i<cnt;i++){
				trace(3,"read cell - 1st=%d,2nd=%d\n",i+a,b);
				add_cell(b,i+a);
			}
			a+=cnt;
		}
		if(c=='b'){
			cnt_x-=cnt;
			a+=cnt;
		}
		if(c=='$' || c=='!'){
			a=0;
			b++;
			cnt_x=x;
		}
	}
	while(c!='!');
	fclose(f);
	return SUCCESS;
}
void print_que(){
	int i;
	trace(2,"que:");
	for(i=0;i<max_q;i++)
		trace(2,"[%d,%d]=%d  ",q[i].x,q[i].y,q[i].alive);
	trace(2,"\n");
}

