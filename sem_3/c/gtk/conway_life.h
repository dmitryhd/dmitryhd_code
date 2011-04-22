#ifndef _LIFE_H_
#define _LIFE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
//error codes
#define SUCCESS 0
#define FAILURE 1
#define ERR_CANNOT_OPEN_FILE 1
#define ERR_WRONG_DATA 2
#define ERR_ZERO_LENGTH 3
#define ERR_MEM 4

/*	hash numbers	*/
#define NUM1 9
#define NUM2 10
#define NUM3 9
#define HASH_MAX NUM2*(NUM1+1)
/*	num of allocated cells		*/
#define ALLOC_CELLS 10
/*	visible field width		*/
#define FIELD_VIS 30
#define mrealloc(p,type,size) \
p=(type *)realloc(p,(size)*sizeof(type));\
if (p==NULL){\
	printf("%s,%d - memory error\n",__FILE__,__LINE__);\
	exit(ERR_MEM);\
}	

typedef long int coord;

/*	linked list of cells		*/
typedef struct cell_struct{
	struct cell_struct *next;
	coord x;
	coord y;
	unsigned int mem_addr;
} cell;
extern cell * head, * p, * tmp;

/* 		cell queue		*/
/*I placed here cells which next_turn 
 * should del or add			*/
#define QUE_BUFF 5000
typedef struct queue_struct{
	coord x;
	coord y;
	char alive;
}queue; 
extern queue cell_q[QUE_BUFF]; 
extern unsigned int cell_q_size;

/* 	cell hash table			*/
//just static array hash table
typedef struct hash_table{
	cell * head;
}key;
extern key hash_t[HASH_MAX];

/*	memory pool			*/
typedef struct pool_struct{
	cell * mem;
	unsigned int * free;
	unsigned int top;//top=last+1

	unsigned int cnt, size;
} pool;
extern pool mem_pool;

/*	debug =)	*/
extern unsigned short debug_level;
void trace(short level, char * format, ...){
	if(level<0 || level > debug_level)
		return;
	va_list args;
	va_start(args,format);
	vprintf(format,args);
	va_end(args);
}
extern unsigned long int turn, live_cells;
extern unsigned int birth, death;
extern int quiet_mode, print_all, print_f, step_mode;

int search_cell(coord in_x, coord in_y);
int hash(coord x, coord y);
int load_file(char *file_name);
void add_cell(coord in_x, coord in_y);	
void del_cell(coord in_x, coord in_y);
void print_cell();
void print_field();
void print_turn();
void next_turn();
cell * alloc_mem();
int alloc_pool();
void free_mem(cell * c);
#endif
