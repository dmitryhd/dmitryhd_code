#include "my_template.h"
#include "crypt.h"
#include "genetic.h"
#include "word.h"
int debug_level=1;

void print_help(char *prog_name);

char open_str[STRLEN];
char en_str[STRLEN];
int res_cnt; 

int main(int argc, char *argv[]){
	char *opts="he:d:o:g:m:r:u:",opt;
	char *encrypt_file,*dict_file,*open_file;
	int i,l,cnt=0;
	set_vars();
	if(argc==1){
		print_help(argv[0]);
		return 0;
	}
	while((opt=getopt(argc,argv,opts))!=-1){
		switch(opt){
			case 'h':
				print_help(argv[0]);
				return 0;
			case 'd':
				dict_file=optarg;
				break;
			case 'e':
				encrypt_file=optarg;
				break;
			case 'o':
				//read open text from file.
				open_file=optarg;
				FILE *f;
				openf(f,open_file,"r");
				for(i=0;(open_str[i]=getc(f))!=EOF;i++){}
				open_str[i]=0;

				//count chars in opentext
				l=strlen(open_str);
				for(i=0;i<l;i++)
					if(!isspace(open_str[i]))
						cnt++;
				suc_thr=cnt*0.8;
				printf("success threshold = %d\n",suc_thr);	
				break;
			case 'g':
				gen=atoi(optarg);	
				break;
			case 'u':
				mut_prb=atof(optarg);	
				break;
			case 'r':
			case 'm':
				s_res=atoi(optarg);	
				break;
			default:
				print_help(argv[0]);
				return 0;
		}
	}
	init_tree(dict_file);
	gen_init();
	gselection(res);
	trace(0,"answers!============\n");

	//print_ans_arr(0,res,res_cnt);
	
	char str[STRLEN];
	for(i=0;i<res_cnt;i++){
		decr_str(en_str,str,res[i].a);
		printf("%d: %s\n",i,str);
	}
	
	return 0;
}
void print_help(char *prog_name){
	printf("usage: %s [-e file] [-d file] \n \
\t-h help\n\
\t-d dictionary_file help\n\
\t-o opentext_file\n\
\t-g n, n = generations number [n=10000]\n\
\t-u n, n = mutation ver. [n=0.3]\n\
\t-r n, n = number of ansvers. [n=2000]\n\
\t-e file_name - encrypt file\n",prog_name);
}


