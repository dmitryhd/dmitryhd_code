#include <stdio.h>
//gcc -fPIC -c lib.c 
//gcc -shared -fPIC -o libtest.so libtest.o
int print_hello(){
	printf("hello!\n");
}
