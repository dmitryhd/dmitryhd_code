#include "life.h"

//life.h variables from life.h
unsigned long int turn, live_cells;
unsigned int birth, death;
int quiet_mode, print_all, print_f, step_mode;
key hash_t[HASH_MAX];
cell * head, * p, * tmp;
queue cell_q[QUE_BUFF]; 
pool mem_pool;
unsigned int cell_q_size;
unsigned short debug_level;

void print_field();
void next_turn();
int search_cell(coord in_x, coord in_y);
void add_cell(coord in_x, coord in_y);	
void del_cell(coord in_x, coord in_y);
void print_cell();
int hash(coord x, coord y);
void print_turn();
cell * alloc_mem();
void free_mem(cell * c);
int load_file(char *file_name);

void print_cell(){
	int i;
	for(i=0;i<HASH_MAX;i++){
		tmp=hash_t[i].head;
		while(tmp!=NULL){
			printf("x=%ld, y=%ld\n",tmp->x,tmp->y);
			tmp=tmp->next;
		}	
	}
}
void print_turn(){
	if(print_all&&(!quiet_mode))
		printf("Turn %ld, Live %ld Death %d Birth %d\n",turn, live_cells, death, birth);
}

int hash(coord x, coord y){
	return abs(NUM2*(x%NUM1)+(y%NUM3));
}
void clrscr(){
	int i;
	char clrterm[]={33,'[','H',33,'[','2','J',0};
	for(i=0;i<7;i++)
		putchar(clrterm[i]);
}
int alloc_pool(){
	int i,top=mem_pool.top, size=mem_pool.size;
	trace(3,"alloc\n");
	mrealloc(mem_pool.free,unsigned int,mem_pool.size+ALLOC_CELLS);
	mrealloc(mem_pool.mem,cell,mem_pool.size+ALLOC_CELLS);
	//potential bug
	//mrealloc(cell_q,queue,mem_pool.size+ALLOC_CELLS);
	for(i=0;i<ALLOC_CELLS;i++){
		//reverse
		mem_pool.free[i+top]=ALLOC_CELLS+size-i-1;
		//free[0]=0 always
	}
	mem_pool.top+=ALLOC_CELLS;
	mem_pool.size+=ALLOC_CELLS;
	return SUCCESS;
}
cell* alloc_mem(){
	cell * c;
	trace(3,"mem \n");
	if(mem_pool.top==0)
		alloc_pool();
	c=&(mem_pool.mem[mem_pool.free[mem_pool.top-1]]);
	c->mem_addr=mem_pool.free[mem_pool.top-1];
	mem_pool.top--;
	mem_pool.cnt++;
	return c;
}
void free_mem(cell * c){
	mem_pool.free[mem_pool.top]=c->mem_addr;
	mem_pool.top++;
	mem_pool.cnt--;
}

void add_cell(coord in_x, coord in_y){	
	if(search_cell(in_x,in_y))
		return;
	trace(4,"add_cell x=%d, y=%d\n",in_x,in_y);
	cell *pr=head;
	live_cells++;
	birth++;
	int h=hash(in_x,in_y);
	if(hash_t[h].head==0){
		hash_t[h].head=alloc_mem();
		hash_t[h].head->x=in_x;
		hash_t[h].head->y=in_y;
		hash_t[h].head->next=NULL;
		return;
	}
	tmp=hash_t[h].head;
	while(tmp && tmp->x < in_x ){
		pr=tmp;
		tmp=tmp->next;
	}
	if(tmp && tmp->x==in_x){
		while(tmp && tmp->y<in_y){
			pr=tmp;
			tmp=tmp->next;
		}
		p=alloc_mem();
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
	}
	p=alloc_mem();
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

void del_cell(coord in_x, coord in_y){
	int h=hash(in_x,in_y);
	if((tmp=hash_t[h].head)==NULL)
		return;
	while(tmp!=NULL){
		if(tmp->x == in_x && tmp->y == in_y){
			live_cells--;
			death++;
			if(tmp==hash_t[h].head){
				hash_t[h].head=tmp->next;
				free_mem(tmp);
				return;
			}
			p->next=tmp->next;
			free_mem(tmp);			
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
	tmp=hash_t[h].head;
	while (tmp!=NULL ){
		if(tmp->x==in_x && tmp->y==in_y)
			return FAILURE;
		tmp=tmp->next;
	}
	return SUCCESS;
}
void check_field(coord x, coord y){
	int res=0;
	trace(3,"check field\n");
	res+=search_cell(x,y+1);
	res+=search_cell(x,y-1);
	res+=search_cell(x+1,y);
	res+=search_cell(x+1,y+1);
	res+=search_cell(x+1,y-1);
	res+=search_cell(x-1,y);
	res+=search_cell(x-1,y-1);
	res+=search_cell(x-1,y+1);

	if( search_cell(x,y) && (res<2 || res>3) ){
		cell_q_size++;
//--------------------!!!!!!!!!!!!!
		//mrealloc(cell_q,queue,cell_q_size)
		cell_q[cell_q_size-1].x=x;
		cell_q[cell_q_size-1].y=y;
		cell_q[cell_q_size-1].alive=0;
		return;
	}
	if( !search_cell(x,y) && res==3 ){
		cell_q_size++;
		//mrealloc(cell_q,queue,cell_q_size)
		cell_q[cell_q_size-1].x=x;
		cell_q[cell_q_size-1].y=y;
		cell_q[cell_q_size-1].alive=1;
		return;
	}
}
void next_turn(){
	//cell_q=NULL;
	cell_q_size=0;
	int i;
	coord x, y;
	cell * tmp;
	death=birth=0;
	trace(3,"next turn\n");
	for(i=0;i<HASH_MAX;i++){
		tmp=hash_t[i].head;
		if(tmp==NULL){
			continue;
		}
		while(tmp!=NULL){
			x=tmp->x;
			y=tmp->y;
			check_field(x+1,y);
			check_field(x+1,y+1);
			check_field(x+1,y-1);
			check_field(x-1,y);
			check_field(x-1,y+1);
			check_field(x-1,y-1);
			check_field(x,y+1);
			check_field(x,y-1);
			check_field(x,y);	
			tmp=tmp->next;
		}
	}
	for(i=0;i<cell_q_size;i++)
		if(cell_q[i].alive==1)
			add_cell(cell_q[i].x,cell_q[i].y);
		else
			del_cell(cell_q[i].x,cell_q[i].y);
//----------------------
//!!!!!!!!!!!!!!!!!!!!!
	//free(cell_q);
}

void print_field(){
	if(!print_f||quiet_mode)
		return;
	coord x,y;
	int i;
	//clrscr();
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
				trace(2,"read cell - 1st=%d,2nd=%d\n",i+a,b);
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
void del_all_live(){
	int i;
	cell *tmp1,*tmp2;
	for(i=0;i<NUM2*(NUM1+1);i++){
		tmp1=hash_t[i].head;
		while(tmp1!=NULL){
			tmp2=tmp1->next;
			del_cell(tmp1->x,tmp1->y);
			tmp1=tmp2;
			printf("del x=%ld, y=%ld\n",tmp->x,tmp->y);
		}	
	}
}
