#include "tree.h"

int main ()
{
	node * tree = NULL;
	printf ("tree sample: \n"); 
	int i;
	for ( i = 5; i < 10; i++ ){
		tree = add_node ( tree, i);	
	}
	for ( i = 1; i < 5; i++ )
		tree = add_node ( tree, i);	
	print_tree ( tree);
	del_node ( tree, find_node ( tree, 6));	
	printf ( "after deleting node 6\n");
	print_tree ( tree);
	return 0;
}
