#ifndef _GENETIC_H_
#define _GENETIC_H_

#define I_MUT_PRB 0.3		//mutation probability
#define I_SUC_THR 50		//succes threshold (genes with fitness
				//above this val. add to successfull list
#define I_GEN 16000		//number of generations
#define I_S_RES 1000		//Start length of successfull list
#define I_MAX_RES 2000 		//Max length of successfull list
#define POPSIZE 2400 		//population size
// POPSIZE % 2 == 0 (!!!!)
#define N 26			//number
#define STRLEN 2048 		//?

typedef struct _ans{		//structure of gene
	int a[N+1]; 		//ключ
				//
	int right_char;		//число символов в опознанных словах 
	float fitness;		//выживаемость
} ans;

extern int S;				//number of survived genes
extern float mut_prb;	 		//mutation probability
extern int suc_thr;			//succes threshold (genes with fitness
				//above this val. add to successfull list
extern int gen;			//number of generations
extern int s_res;			//Start length of successfull list
extern unsigned long long max_gen;	//number of generations
extern ans *res;			//succesfull genes
extern int key[N];
extern int res_cnt;
void mutate(ans *a);
void gselection(ans *res);
void one_point_crossover(ans *par, ans *child);
int get_fitness(ans * a);
int cmp(const void *a, const void *b);
void fill_zeros(int *a);
//init
void gen_init();
void set_vars();
void get_init_pop(ans * pop);
void permute_by_swap(ans *x);
//output
void print_ans_arr(int,ans *a, int n);
void print_ans(int,ans a);
//random
int random_lim(int min,int max);
void randomize();

extern char open_str[STRLEN];
extern char en_str[STRLEN]; 
//[end]
#endif
