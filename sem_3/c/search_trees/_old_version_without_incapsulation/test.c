#include "tree.h"

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
void test_insert ( node ** root)
{
	printf ( "test #1 ---\nSimple insert\n");
	mass_insert ( root, 7, 10, 5, 15, 17, 12, 13, 3 );
	print_tree ( *root);
}

void test_rb_insert ( node ** root)
{
	printf ( "test #4 ---\nSimple insert\n");
	mass_rb_insert ( root, 9, 1, 2, 4, 5, 9, 8, 11, 14, 15 );
	print_tree ( *root);
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
void test_delete ( node ** root)
{
	printf ( "test #2 ---\nSimple delete\n");
	*root = del_node ( *root, find_node ( *root, 3));	
	printf ( "after deleting node 3\n");
	print_tree ( *root);
	printf ( "-----------\n");

	*root = del_node ( *root, find_node ( *root, 6));	
	printf ( "after deleting node 6\n");
	print_tree ( *root);
	printf ( "-----------\n");

	*root = del_node ( *root, find_node ( *root, 10));	
	printf ( "after deleting nod 10\n");
	print_tree ( *root);
	printf ( "-----------\n");

	*root = del_node ( *root, find_node ( *root, 15));	
	printf ( "after deleting node 15\n");
	print_tree ( *root);
	printf ( "-----------\n");
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
void test_rotate ( node ** root)
{

	printf ( "test #3 ---\nright rotate\n");
	mass_insert ( root, 6, 100, 20, 15, 25, 23, 27);
	print_tree ( *root);

	node * tmp = find_node ( *root, 20);
	tmp = left_rotate ( tmp);
	printf ( "after left rotate around 20:\n -----------\n");
	print_tree ( *root);

	printf ( "after right rotate around 25:\n -----------\n");
	tmp = find_node ( *root, 25);
	tmp = right_rotate ( tmp);
	print_tree ( *root);
	printf ( "-----------\n");
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
void test ( node ** root)
{
	//++
	//test_insert ( root);
	//++
	//test_delete( root);
	
	//node ** new = NULL;
	
	//++
	//test_rotate ( root);
	test_rb_insert ( root);
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
void print_tree ( node * root)
{
	if ( root == NULL)
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
	print_tree( root->left);
	print_tree( root->right);
}


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
void mass_insert ( node ** root, int num, ... ) 
{
	va_list ap; //pointer to nameless arguments
	int s;
	int i;
	va_start ( ap, num);

	for ( i = 0; i < num ; i++ ){
		s = va_arg ( ap, int );
		*root = add_node ( *root, s);
	}

	va_end ( ap);
}

void mass_rb_insert ( node ** root, int num, ... ) 
{
	va_list ap; //pointer to nameless arguments
	int s;
	int i;
	va_start ( ap, num);

	for ( i = 0; i < num ; i++ ){
		s = va_arg ( ap, int );
		*root = rb_insert ( *root, s);
	}
	va_end ( ap);
}
