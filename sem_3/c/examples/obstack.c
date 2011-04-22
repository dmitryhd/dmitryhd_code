#include <obstack.h>

#include <stdlib.h>
#include <stdio.h>

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

char * string_copy (char *in)
{
	struct obstack os;
	obstack_init ( &os);
	char * out;
	int len = strlen (in) + 1;
	out = (char *) obstack_alloc (&os, len);
	memcpy ( out, in, len);
	return out;
}
//or
char * string_copy_obs (char *in)
{
	struct obstack os;
	obstack_init ( &os);
	char * out;
	int len = strlen (in) + 1;
	out = (char *) obstack_copy0 (&os, in, len);
	return out;
}

int main ()
{
	char *p = "abacaba";
	char *q;
	q = string_copy ( p);
	printf ("%s\n",q);
	q = string_copy_obs ( p);
	printf ("%s\n",q);
	return 0;
}
