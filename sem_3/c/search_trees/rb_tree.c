#include "tree.h"

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
int left_rotate ( int x_)
{
	if ( x_ == INVALID_NODE )
		return x_;
	node * x = extract_node ( x_);
	if ( x->right == NULL)
		return x->ind;
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
	return y->ind;
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
int right_rotate ( int x_)
{

	if ( x_ == INVALID_NODE )
		return x_;
	node * x = extract_node ( x_);
	if ( x->left == NULL)
		return x->ind;
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
	return y->ind;
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
int rb_insert ( int root_, int s )
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
	new->color = RED;
	if ( !y) // new tree
		return new->ind;
	else{
		if ( y->key > s)
			y->left = new;
		else
			y->right = new;
		
	}
	rb_insert_fixup ( root, new);
	
	return new->ind;	
}

/******************************/
//Inner functions:
//this functions can't be used directly by user;
//use them only inside other functions:
/*****************************************/

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
					left_rotate ( z->ind);
				}
				//3
				zp->color = BLACK;
				zp->parent->color = RED;
				right_rotate ( zp->parent->ind );
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
					right_rotate ( z->ind);
				}
				//3
				zp->color = BLACK;
				zp->parent->color = RED;
				left_rotate ( zp->parent->ind );
			}
		}
		zp = z->parent;
	}
	root->color = BLACK;
	return root;
}
