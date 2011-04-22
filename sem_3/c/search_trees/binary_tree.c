#include "tree.h"

//This variables contain all nodes;
node ** nar;
int nar_size;


/*****************************************
 * Function add_node 
 * insert node to the rb-tree like in binary 
 * tree 
 * or if such a node exist -> do nothing
 * Parameters:
 * - node * root - root of the tree
 * - int s - key of new node
 * Return:
 * 	pointer to root
 * Usage: 
 * 	tree = add_node ( tree, 15)
 ******************************************/
int add_node ( int root_, int s)
{
	printf ( "add node: %d\n", s);
	node * root = extract_node ( root_);
	node * x = root; // current node
	node * y = NULL; // parent of current
	while ( x != NULL){
		y = x;
		if ( s > x->key ) 
			x = x->right;
		else if ( s < x->key ) 
			x = x->left;
		else //( x->key == s ){
		{
			printf ( "node with %d already exist\n", s);
			return root->ind;
		}
	}
	node * new = create_node ( s, y);
	
	if ( !y) // new tree
		return new->ind;
	else
		if ( y->key > s)
			y->left = new;
		else
			y->right = new;
		
	return root->ind;
}

/*****************************************
 * Function tree_minimum
 * seek for a minimum node
 * Parameters:
 * - node * x - root of tree or subtree
 * Return:
 * 	minimal node of NULL, if root == NULL
 * Usage: 
 ******************************************/
int tree_minimum ( int x_)
{
	node *x = extract_node ( x_);
	if ( x == NULL)
		return INVALID_NODE;
	while ( x->left != NULL)
		x = x->left;
	return x->ind;
}

/*****************************************
 * Function tree_successor
 * find successor of current node
 * Parameters:
 * - node * root - root of the tree
 * - node * x - node which is need of successor
 * Return:
 * 	pointer to successor or nil there is no
 * 	such node
 * Usage: 
 ******************************************/
int tree_successor ( int x_)
{
	node * x = extract_node ( x_);
	if ( x == NULL){
		return INVALID_NODE;
	}
	printf("finding succ for %d\n", x->key);
	if ( x->right != NULL)
		return tree_minimum ( x->right->ind);
	node * y = x->parent;
	while ( y != NULL && x == y->right){
		x = y;
		y = x-> parent;
	}
	return y->ind;
}

/*****************************************
 * Function del_node
 * delete node
 * Parameters:
 * - node * root - root of the tree
 * - node * x - node which is deleted
 * Return:
 * 	pointer to root or null if tree is 
 * 	fully deleted.
 * Usage: 
 * 	tree = del_node ( root, 124)
 ******************************************/
int del_node ( int root_, int x_)
{
	node * root = extract_node ( root_);
	node * x = extract_node ( x_);
	if ( root == NULL )
		return INVALID_NODE;
	if ( x == NULL )
		return root->ind;
	//case 1
	if ( x->left == NULL && x->right == NULL){
		node * y = x->parent;
		if ( y == NULL)
			return root->ind;
		if ( x->key > y->key)
			y->right = NULL;
		else 
			y->left = NULL;
		return root->ind;
	}
	//case 2
	if ( x->left == NULL){
		node * y = x->parent;
		x->right->parent = y;
		if ( y == NULL)
			return x->right->ind;
		if ( x->key > y->key)
			y->right = x->right;
		else 
			y->left = x->right;
		return root->ind;
	}
	if ( x->right == NULL){
		node * y = x->parent;
		x->left->parent = y;
		if ( y == NULL)
			return x->left->ind;
		if ( x->key > y->key)
			y->right = x->left;
		else 
			y->left = x->left;
		return root->ind;
	}
	//case 3
	node * y = x;
	printf ( "deleting node %d in case 3\n", x->key);
	do{
		y = extract_node ( tree_successor ( y->ind));
	}while ( y != NULL && y->left != NULL);

	if ( y == NULL){
		printf ("suddenly we didn't find any node without left child\n");
		return root->ind;
	}
	printf ("we find %d node without left child\n", y->key);
	//( y->left == NULL)
	node * z = y->parent;
	if( z){
		if ( y->key > z->key)
			z->right = y->right;
		else 
			z->left = y->right;
	}
	else
		return y->right->ind;
	//copy
	//x - deleted node
	//y - finded successor
	//z - parent of successor
	if ( y->right){
		y->right->parent = y->parent;
	}
	y->left = x->left;
	y->right = x->right;
	y->parent = x->parent;

	if (x->left) x->left->parent = y;
	if (x->right )x->right->parent = y;
	if (x->parent) {
		if(x->key > x->parent->key )	
			x->parent->right = y;
		else
			x->parent->left = y;
	}
	else 
		root = y;
	return root->ind;
}

/*****************************************
 * Function find_node 
 * finds node in tree by the key
 * Parameters:
 * - node * root - root of the tree
 * - node * s - key which we are searching
 * Return:
 * 	poiner to the node whith key == s
 * 	or NULL if we can't find such node
 * Usage: 
 ******************************************/
int find_node ( int root_, int s)
{
	node * root = extract_node ( root_);
	node * t = root;
	while ( t != NULL){
		if ( t->key == s)
			return t->ind;
		if ( s > t->key)
			t = t->right;
		else
			t = t->left;
	}
	printf("can't find node with %d,\n", s);
	return INVALID_NODE;
}

/******************************/
//Inner functions:
//this functions can't be used directly by user;
//use them only inside other functions:
/*****************************************/

/*****************************************
 * Function xmalloc
 * safely version of malloc. 
 * I use it to prevent segfault
 * Parameters:
 * - sz - size of memory in bytes
 * Return: pointer to new memory
 * Usage: 
 ******************************************/
void *xmalloc (size_t sz)
{
	void * p = malloc (sz);
	if ( p == NULL){
		perror("malloc fail");
		exit ( EMALLOC);
	}
	return p;
}

void * xrealloc ( void * ptr, size_t sz)
{
	void * p = realloc ( ptr, sz);
	if ( p == NULL){
		perror("realloc fail");
		exit ( EMALLOC);
	}
	return p;
}

node * extract_node ( int n)
{
	if ( n > nar_size || n == INVALID_NODE)
		return NULL;
	return nar [ n];
}

/*****************************************
 * Function create_node
 * allocate new node for inserting to tree
 * Parameters:
 * - ins s - key of new node
 * - node * parent - pointer to new node's 
 *   parent or NULL if there is no such node.
 * Return: 
 * 	pointer to new node.
 * Usage:  node * x = create_node (12, NULL);
 ******************************************/
node * create_node ( int s, node * parent)
{
	node * n;
	n = xmalloc ( sizeof( node));
	n->key = s;
	n->left = NULL;
	n->right = NULL;
	n->parent = parent;

	n->ind = nar_size;
	nar = (node **) xrealloc ( nar, (nar_size+1)*(sizeof(node *)));
	nar [nar_size++] = n;

	return n;
}

/*****************************************
 * Function is_nil
 * determine if its argument leaf of red
 * black tree.
 * Parameters:
 * - node * x -> is x = nil? 
 * Return:
 *	zero on x == NIL
 *	1 on x != NIL
 ******************************************/
int is_nil ( node * x)
{
	if (x)
		return 0;
	else 
		return 1;
}
