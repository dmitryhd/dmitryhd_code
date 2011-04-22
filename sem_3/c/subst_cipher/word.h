#ifndef _WORD_H_
#define _WORD_H_

#define N 26			//number
#define STRLEN 2048 		//?
#define WSIZE 40		//?
typedef struct _wtree wtree; 
struct _wtree{	//dictionary
	int is_word;
	wtree *a[N+1];
	char *word;
};
wtree root, *tmp;//tree;

wtree* mknod();
void add_word(char *word);
int is_word(char *s, int *cnt);
int crypt[STRLEN], crlen;
void init_tree(char *file);
char n2c(int n);
int c2n(char c);
void to_lower(char *s);
int is_any_word(char *s,int *);
void str2num(char *str,int *num, int *);
void num2str(int *, char*, int *);
void print_int(int *num, int);

void trace(short level, char * format, ...);
#endif

