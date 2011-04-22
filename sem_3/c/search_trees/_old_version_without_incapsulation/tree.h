#ifndef  _TREE_H_
#define _TREE_H_

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>

#define EMALLOC 3
#define RED 0
#define BLACK 1

typedef struct _node node;

struct _node 
{
	node * left;
	node * right;
	node * parent;
	int color;
	int key;
};
void * xmalloc ( size_t size);

//extern 

extern node * create_node ( int s, node * parent);
extern node * add_node ( node * root, int s);
// example : tree = add_node ( tree, 12);
extern node * tree_minimum ( node * x);
extern node * tree_successor ( node * root, node * x);
extern void print_tree ( node * root);
extern node * del_node ( node * root, node * x);
extern node * find_node ( node * root, int s);
extern void mass_insert ( node ** root, int num, ... );
extern void test_insert ( node ** root);
extern void test_delete ( node ** root);
extern void test_rotate ( node ** root);
extern void test ( node ** root);
extern node * right_rotate ( node * x);
extern node * left_rotate ( node * x);
extern node * rb_insert_fixup ( node * root, node * z );
extern void mass_rb_insert ( node ** root, int num, ... );
extern node * rb_insert ( node *root, int s);
extern void test_rb_insert ( node ** root);

extern node nil;
extern node * node_array;

#endif
