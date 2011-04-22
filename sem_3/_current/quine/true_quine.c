#include <stdio.h>
int main() { char q = '"'; 
	puts("#include <stdio.h>");char *x = "int main() { char q = '%c'; puts(%c#include <stdio.h>%c);char *x = %c%s%c;printf (  x,q,q,q,q,x,q,q,q); return puts(%c%c); }"; printf ( x,q,q,q,q,x,q,q,q); return puts(""); }
