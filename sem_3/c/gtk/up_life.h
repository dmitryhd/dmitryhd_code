#ifndef _LIFE_H_
#define _LIFE_H_
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

