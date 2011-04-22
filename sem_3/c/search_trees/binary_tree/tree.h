#ifndef  _TREE_H_
#define _TREE_H_

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#define EMALLOC 3

typedef struct _node node;

struct _node 
{
	node * left;
	node * right;
	node * parent;
	int key;
};
void * xmalloc ( size_t size);
node * create_node ( int s, node * parent);
node * add_node ( node * root, int s);
node * tree_minimum ( node * x);
node * tree_successor ( node * root, node * x);
void print_tree ( node * root);
void del_node ( node * root, node * x);
node * find_node ( node * root, int s);

#endif
