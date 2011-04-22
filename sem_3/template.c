#include <stdio.h>
#include <unistd.h>

int main( int argc, char *argv[])
{
	int opt, i;
	/* Options */
	int is_preserve = 0;
	int is_interactive = 0;
	int is_verbose = 0;
	int is_force = 0;
	/* Parsing aguments */
	for( i=1;i<argc;i++){
		/* If current argument is option */
		if(argv[i][0] == '-'){
			optind = i ;
			opt=getopt(argc,argv,"ipfv");
			switch(opt){
				case 'p':
					is_preserve = 1;
					break;
				case 'i':
					is_interactive = 1;
					break;
				case 'v':
					is_verbose = 1;
					break;
				case 'f':
					is_force = 1;
					break;
			}
		}
		/* If current argument is parameter */
		else{
		}
	}
	
	return 0;
}
