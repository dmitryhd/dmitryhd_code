#include "tree.h"

void *xmalloc (size_t sz)
{
	void * p = malloc (sz);
	if ( p == NULL){
		perror("malloc fail");
		exit ( EMALLOC);
	}
	return p;
}

node * create_node ( int s, node * parent)
{
	node * n;
	n = xmalloc ( sizeof( node));
	n->key = s;
	n->left = NULL;
	n->right = NULL;
	n->parent = parent;
	return n;
}


node * add_node ( node * root, int s)
{
	printf ( "add node: %d\n", s);
	node * x = root; // current node
	node * y = NULL; // parent of current
	while ( x!=NULL){
		y = x;
		if ( x->key < s ) 
			x = x->left;
		else if ( x->key > s ) 
			x = x->right;
		else //( x->key == s ){
		{
			printf ( "node with %d already exist\n", s);
			return root;
		}
	}
	node * new = create_node ( s, y);
	if ( !y) // new tree
	{
		//printf("new tree\n");
		return new;
	}
	else{ 
		if ( y->key > s){
		//	printf("add right\n");
			y->right = new;
		}
		else{
		//	printf("add left\n");
			y->left = new;
		}
	}
	return root;
}

node * tree_minimum ( node * x)
{
	node * a = x;
	while ( a != NULL)
		a = a->right;
	return a;

}

node * tree_successor ( node * root, node * x)
{
	if ( x->right != NULL)
		return tree_minimum ( x->right);
	node * y = x->parent;
	while ( y != NULL && x == y->right){
		x = y;
		y = x-> parent;
	}
	return y;
}

void print_tree ( node * root)
{
	if ( root == NULL)
		return;
	printf( "|->%d\n", root->key);
	print_tree( root->left);
	print_tree( root->right);
}

void del_node ( node * root, node * x)
{
	if ( x == NULL)
		return;
	//case 1
	if ( x->left == NULL && x->right == NULL){
		node * y = x->parent;
		if ( x->key > y->key)
			y->left = NULL;
		else 
			y->right = NULL;
		return;
	}
	//case 2
	if ( x->left == NULL){
		node * y = x->parent;
		x->right->parent = y;
		if ( x->key > y->key)
			y->left = x->right;
		else 
			y->right = x->right;
		return;
	}
	if ( x->right == NULL){
		node * y = x->parent;
		x->left->parent = y;
		if ( x->key > y->key)
			y->left = x->left;
		else 
			y->right = x->left;
		return;
	}
	//case 3
	//need to write!
	/*
	node * y = tree_successor ( x);
	if ( y->left == NULL && y->right == NULL){
		node * z = y->parent;
		if ( y->key > z->key)
			z->left = NULL;
		else 
			z->right = NULL;
		return;
	}
	if ( y->left == NULL){
		node * z = y->parent;
		y->right->parent = y;
		if ( y->key > z->key)
			y->left = z->right;
		else 
			y->right = z->right;
		return;
	}
	if ( y->right == NULL)z
		node * z = y->parent;
		y->left->parent = y;
		zf ( y->key > z->key)
			z->left = y->left;
		else 
			z->right = y->left;
		return;
	}
	*/
	return;	
}

node * find_node ( node * root, int s)
{
	node * t = root;
	while ( t != NULL){
		if ( t->key == s)
			return t;
		if ( t->key > s)
			t = t->right;
		else
			t = t->left;
	}
	printf("can't find node with %d,\n", s);
	return t;
}


