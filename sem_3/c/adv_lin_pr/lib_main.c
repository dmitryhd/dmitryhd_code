//gcc -L. -ltest -o lib_main lib_main.c -Wl,-rpath,`pwd`
//shared library example
//files: lib_main.c, libtest.c
int main(){
	print_hello();
	return 0;
}

