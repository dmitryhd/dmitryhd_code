#include "tree.h"

/*****************************************
 * Function is_nil - description
 * Parameters:
 * - node * x -> is x = nil? 
 * - more strings
 * Return:
 *	zero on success [etc]
 ******************************************/


/*****************************************
 * Function left_rotate
 * left rotate in red-black tree (Kormen)
 * Parameters:
 * - node * x - node which is rotated
 * Return:
 * 	node whish is rotated:
 * Usage: 
 * 	x = left_rotate ( x);
 ******************************************/
node * left_rotate ( node * x)
{
	if ( x == NULL )
		return NULL;
	if ( x->right == NULL)
		return x;
	node * y = x->right;
	x->right = y->left;
	y->parent = x->parent;
	if ( y->left )
		y->left->parent = x;
	if ( x->parent ){
		if ( x->key < x->parent->key) //x == x->parent->left
			x->parent->left = y;
		else //x == x->parent->right
			x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
	return x;
}

/*****************************************
 * Function right_rotate
 * left rotate in red-black tree (Kormen)
 * Parameters:
 * - node * x - node which is rotated
 * Return:
 * 	node whish is rotated:
 * Usage: 
 * 	x = right_rotate ( x);
 ******************************************/
node * right_rotate ( node * x)
{
	if ( x == NULL )
		return NULL;
	if ( x->left == NULL)
		return x;
	node * y = x->left;
	x->left = y->right;
	y->parent = x->parent;
	if ( y->right)
		y->right->parent = x;
	if ( x->parent ) {
		if ( x->key < x->parent->key) //x == x->parent->left
			x->parent->left = y;
		else //x == x->parent->right
			x->parent->right = y;
	}
	y->right = x;
	x->parent = y;
	return x;
}

/*****************************************
 * Function rb_insert
 * insert node to the rb-tree (Kormen)
 * Parameters:
 * - node * root - root of the tree
 * - node * x - node which is added
 * Return:
 * Usage: 
 ******************************************/
node * rb_insert ( node * root, int s )
{
	printf ( "add node: %d\n", s);
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
			return root;
		}
	}
	node * new = create_node ( s, y);
	new->color = RED;
	if ( !y) // new tree
		return new;
	else{
		if ( y->key > s)
			y->left = new;
		else
			y->right = new;
		
	}
	rb_insert_fixup ( root, new);
	return root;	
}

node * rb_insert_fixup ( node * root, node * z )
{
	node *zp = z->parent;
	node * y;
	while ( zp && zp->color == RED){
		if ( zp->parent && zp->key < zp->parent->key){ //is left
			//z->parent->key < zpp->key
			y = zp->parent->right;
			if (y->color == RED){ 
				//1
				zp->color = BLACK;
				y->color = BLACK;
				zp->parent->color = RED;
				z = zp->parent;
			}
			else {
				if ( z->key > zp->key){ //if right
				//2
					z = zp;
					left_rotate ( z);
				}
				//3
				zp->color = BLACK;
				zp->parent->color = RED;
				right_rotate ( zp->parent );
			}
		}
		else {
			//z->parent->key < zpp->key
			if ( !zp->parent)
				break;
			y = zp->parent->left;
			if (y->color == RED){ 
				//1
				zp->color = BLACK;
				y->color = BLACK;
				zp->parent->color = RED;
				z = zp->parent;
			}
			else {
				if ( z->key < zp->key){ //if left
				//2
					z = zp;
					right_rotate ( z);
				}
				//3
				zp->color = BLACK;
				zp->parent->color = RED;
				left_rotate ( zp->parent );
			}
		}
		zp = z->parent;
	}
	root->color = BLACK;
	return root;
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
	return n;
}


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
node * add_node ( node * root, int s)
{
	printf ( "add node: %d\n", s);
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
			return root;
		}
	}
	node * new = create_node ( s, y);
	if ( !y) // new tree
	{
		return new;
	}
	else{ 
		if ( y->key > s){
			y->left = new;
		}
		else{
			y->right = new;
		}
	}
	return root;
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
node * tree_minimum ( node * x)
{
	if ( x == NULL)
		return x;
	while ( x->left != NULL)
		x = x->left;
	return x;
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
node * tree_successor ( node * root, node * x)
{
	if ( x == NULL){
		return NULL;
	}
	printf("finding succ for %d\n", x->key);
	if ( x->right != NULL)
		return tree_minimum ( x->right);
	node * y = x->parent;
	while ( y != NULL && x == y->right){
		x = y;
		y = x-> parent;
	}
	return y;
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
node * del_node ( node * root, node * x)
{
	if ( x == NULL)
		return root;
	//case 1
	if ( x->left == NULL && x->right == NULL){
		node * y = x->parent;
		if ( y == NULL)
			return root;
		if ( x->key > y->key)
			y->right = NULL;
		else 
			y->left = NULL;
		return root;
	}
	//case 2
	if ( x->left == NULL){
		node * y = x->parent;
		x->right->parent = y;
		if ( y == NULL)
			return x->right;
		if ( x->key > y->key)
			y->right = x->right;
		else 
			y->left = x->right;
		return root;
	}
	if ( x->right == NULL){
		node * y = x->parent;
		x->left->parent = y;
		if ( y == NULL)
			return x->left;
		if ( x->key > y->key)
			y->right = x->left;
		else 
			y->left = x->left;
		return root;
	}
	//case 3
	node * y = x;
	printf ( "deleting node %d in case 3\n", x->key);
	do{
		y = tree_successor ( root, y);
	}while ( y != NULL && y->left != NULL);

	if ( y == NULL){
		printf ("suddenly we didn't find any node without left child\n");
		return root;
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
		return y->right;
	//copy
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
	return root;
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
node * find_node ( node * root, int s)
{
	node * t = root;
	while ( t != NULL){
		if ( t->key == s)
			return t;
		if ( s > t->key)
			t = t->right;
		else
			t = t->left;
	}
	printf("can't find node with %d,\n", s);
	return t;
}

