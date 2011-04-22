#include "tree.h"
#include "test.h"

/*****************************************
 * Function test_insert
 * This function is part of complex test of 
 * tree implementaion.
 * test insertions in rb-tree
 * Parameters:
 * - root - pointer to pointer root of tree
 * Return:
 * Usage: node * root; test_insert (&root);
 ******************************************/
 
int test_insert ( int root)
{
	printf ( "test #1 ---\nSimple insert\n");
	root = mass_insert ( root, 7, 10, 5, 15, 17, 12, 13, 3 );
	print_tree ( root);
	return root;
}

int test_rb_insert ( int root)
{
	printf ( "test #4 ---\nSimple insert\n");
	root = mass_rb_insert ( root, 9, 1, 2, 4, 5, 9, 8, 11, 14, 15 );
	print_tree ( root);
	return root;
}
/*****************************************
 * Function test_delete
 * This function is part of complex test of 
 * tree implementaion.
 * test deletions in rb-tree
 * Parameters:
 * - root - pointer to pointer root of tree
 * Return:
 * Usage: node * root; test_rotate (&root);
 ******************************************/
int test_delete ( int root)
{
	printf ( "test #2 ---\nSimple delete\n");
	root = del_node ( root, find_node ( root, 3));	
	printf ( "after deleting node 3\n");
	print_tree ( root);
	printf ( "-----------\n");

	root = del_node ( root, find_node ( root, 6));	
	printf ( "after deleting node 6\n");
	print_tree ( root);
	printf ( "-----------\n");

	root = del_node ( root, find_node ( root, 10));	
	printf ( "after deleting nod 10\n");
	print_tree ( root);
	printf ( "-----------\n");

	root = del_node ( root, find_node ( root, 15));	
	printf ( "after deleting node 15\n");
	print_tree ( root);
	printf ( "-----------\n");
	return 1;
}

/*****************************************
 * Function test_rotate
 * This function is part of complex test of 
 * tree implementaion.
 * test rotations in rb-tree
 * Parameters:
 * - root - pointer to pointer root of tree
 * Return:
 * Usage: node * root; test_rotate (&root);
 ******************************************/
int test_rotate ( int root)
{

	printf ( "test #3 ---\nright rotate\n");
	root = mass_insert ( root, 6, 100, 20, 15, 25, 23, 27);
	print_tree ( root);

	int tmp = find_node ( root, 20);
	tmp = left_rotate ( tmp);
	printf ( "after left rotate around 20:\n -----------\n");
	print_tree ( root);

	printf ( "after right rotate around 25:\n -----------\n");
	tmp = find_node ( root, 25);
	tmp = right_rotate ( tmp);
	print_tree ( root);
	printf ( "-----------\n");
	return root;
}
/*****************************************
 * Function test
 * This function starts complex test of 
 * tree implementaion.
 * Parameters:
 * - root - pointer to pointer root of tree
 * Return:
 * Usage: node * root; test (&root);
 ******************************************/
void test ( int root)
{
	//++
	root = test_insert ( root);
	//++
	root = test_delete( root);
	//++
	root = test_rotate ( root);
	return;
}

/*****************************************
 * Function print_tree
 * debug function to print tree 
 * Parameters:
 * - root - pointer to root of tree
 * Return:
 * Usage: 
 ******************************************/
void print_tree ( int root_)
{
	node * root = extract_node ( root_);
	if ( !root)
		return;
	printf( "|->%d", root->key);
	printf( " color=%d\n", root->color);
	if( root->left != NULL)
		printf( " left->%d", root->left->key);
	if( root->right != NULL)
		printf( " right->%d", root->right->key);
	if( root->parent != NULL)
		printf( " parent->%d", root->parent->key);
	
	printf( "\n");
	if ( root->left)
		print_tree( root->left->ind);
	if ( root->right)
		print_tree( root->right->ind);
}


/*****************************************
 * Function mass_insert
 * debug function which adds many nodes in
 * one time.
 * Parameters:
 * - node ** root - root of the tree
 * - num - number of nodes which is added.
 * - nameless args is keys of nodes (type int)
 * Return:
 * Usage: 
 * 	mass_insert ( tree, 3, 1, 2 ,3);
 * 	//adds three nodes 1 2 3 in tree
 ******************************************/
int mass_insert ( int root, int num, ... ) 
{
	va_list ap; //pointer to nameless arguments
	int s;
	int i;
	va_start ( ap, num);

	for ( i = 0; i < num ; i++ ){
		s = va_arg ( ap, int );
		root = add_node ( root, s);
	}

	va_end ( ap);
	return root;
}
int mass_rb_insert ( int root, int num, ... ) 
{
	va_list ap; //pointer to nameless arguments
	int s;
	int i;
	va_start ( ap, num);

	for ( i = 0; i < num ; i++ ){
		s = va_arg ( ap, int );
		root = rb_insert ( root, s);
	}
	va_end ( ap);
	return root;
}
