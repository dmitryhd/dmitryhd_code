/* 	conway's game of live
 *  	Khodakov Dmitry - 817 gr.
 */
#include "life.h"

unsigned long int turn, live_cells;
unsigned int birth, death;
int quiet_mode, print_all, print_f, step_mode;
key hash_t[HASH_MAX];
cell * head, * p, * tmp;
queue cell_q[QUE_BUFF]; 
pool mem_pool;
unsigned int cell_q_size;
unsigned short debug_level;

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
		tmp=hash_t[i].head;
		while(tmp!=NULL){
			printf("x=%ld, y=%ld\n",tmp->x,tmp->y);
			tmp=tmp->next;
		}	
	}
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

void print_turn(){
	if(print_all)
		printf("Turn %ld, Live %ld Death %d Birth %d\n",turn, live_cells, death, birth);
}

int hash(coord x, coord y){
	return abs(NUM2*(x%NUM1)+(y%NUM3));
}
int alloc_pool(){
	int i, size=mem_pool.size;
	trace(3,"alloc\n");
	//top always =0 here! =)
	mrealloc(mem_pool.free,unsigned int,mem_pool.size+ALLOC_CELLS);
	mrealloc(mem_pool.mem,cell,mem_pool.size+ALLOC_CELLS);
	for(i=0;i<ALLOC_CELLS;i++){
		//reverse
		mem_pool.free[i]=ALLOC_CELLS+size-1-i;
	}
	mem_pool.top=ALLOC_CELLS-1;
	mem_pool.size+=ALLOC_CELLS;
	pool_print();
	hash_print();
	return SUCCESS;
}
cell* alloc_mem(){
	//cell * c=(cell *)malloc(sizeof(cell));
	cell *c;
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
	//free(c);
	mem_pool.free[mem_pool.top]=c->mem_addr;
	c=NULL;
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
		cell_q_size++;
		//mrealloc(cell_q,queue,cell_q_size);
		cell_q[cell_q_size-1].x=x;
		cell_q[cell_q_size-1].y=y;
		cell_q[cell_q_size-1].alive=0;
		return;
	}
	if( !(search_cell(x,y)) && res==3 ){
		cell_q_size++;
		//mrealloc(cell_q,queue,cell_q_size);
		cell_q[cell_q_size-1].x=x;
		cell_q[cell_q_size-1].y=y;
		cell_q[cell_q_size-1].alive=1;
	}
}

void next_turn(){
	//cell_q=NULL;
	cell_q_size=0;
	int i;
	coord x, y, j;
	cell * it;
	death=birth=0;
	//printf("next turn\n");
	for(i=0;i<NUM2*(NUM1+1);i++){
		it=hash_t[i].head;
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
	for(j=0;j<cell_q_size;j++)
		if(cell_q[j].alive==1)
			add_cell(cell_q[j].x,cell_q[j].y);
		else
			del_cell(cell_q[j].x,cell_q[j].y);
	//free(cell_q);
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
	for(i=0;i<cell_q_size;i++)
		trace(2,"[%d,%d]=%d  ",cell_q[i].x,cell_q[i].y,cell_q[i].alive);
	trace(2,"\n");
}

void hash_print(){
	int i;
	cell *tmp;
	for(i=0;i<HASH_MAX;i++){
		if((tmp=hash_t[i].head)!=NULL)
			trace(1,"hash #%05d head:",i);
		else 
			continue;
		while(tmp!=NULL){
			trace(1,"->%d,%d, m(%d) ",tmp->x,tmp->y,tmp->mem_addr);
			tmp=tmp->next;
		}
		trace(1,"\n");
	}	
}
void pool_print(){
	int i;
	trace(1,"pool top=%d, size=%d, cnt=%d\n",
	mem_pool.top,mem_pool.size,mem_pool.cnt);
	for(i=0;i<mem_pool.top;i++){
		trace(1,"%d ",mem_pool.free[i]);
	}
	trace(1,"\n");
}
