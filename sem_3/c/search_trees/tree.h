#ifndef  _TREE_H_
#define _TREE_H_

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>

#define EMALLOC 3
#define RED 0
#define BLACK 1
#define INVALID_NODE -1

typedef struct _node node;

struct _node 
{
	node * left;
	node * right;
	node * parent;
	int color;
	int key;
	int ind;
};

extern node ** nar;
extern int nar_size;

//---------base--------------------
extern int add_node ( int root, int s);
extern int mass_insert ( int root, int num, ... );
extern void print_tree ( int root);
extern int find_node ( int root, int s);

extern int tree_minimum ( int x);
extern int tree_successor (  int x);
extern int del_node ( int root, int x);
extern int del_node ( int root, int x);

//service--------------
extern void * xmalloc ( size_t size);
extern void * xrealloc ( void * ptr, size_t sz);
extern node * create_node ( int s, node * parent);
extern node * extract_node ( int n);

//------RB trees---------------
extern int right_rotate ( int x);
extern int left_rotate ( int x);
extern node * rb_insert_fixup ( node * root, node * z );
extern int mass_rb_insert ( int root, int num, ... );
extern int  rb_insert ( int root, int s);

#endif
