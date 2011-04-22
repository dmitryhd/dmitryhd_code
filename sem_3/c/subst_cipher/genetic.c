#include "genetic.h"
#include "my_template.h"
#include "crypt.h"
#include "word.h"
int S;				//number of survived genes
float mut_prb;	 		//mutation probability
int suc_thr;			//succes threshold (genes with fitness
				//above this val. add to successfull list
int gen;			//number of generations
int s_res;			//Start length of successfull list
int 
unsigned long long max_gen;	//number of generations
ans *res;			//succesfull genes
int key[N];
char open_str[STRLEN];
char en_str[STRLEN]; 
int res_cnt;

int mess_size;			//size of encrypted message

int debug_level;
/************************************************
 *Crossover function	
 *Survived 1/2 of all population
 *Every survived gene is parent of 2 offspring
 * **********************************************/
void one_point_crossover(ans *par, ans *child){
	int i,j,k,m,l,zero_cnt,tmp, hamming_dist;
	int mut; 	//if we got equal genes
			//then mutate one of them

	ans a,b;	//parents
	trace(4,"one point crossower\n");
	S=POPSIZE/2;	//define num, of survived genes
	//for(i=0;i<POPSIZE;i++)
		//if(par[i].dstr!=NULL)
		//	free(par[i].dstr);
	//	printf("dstr = %s",par[i].dstr);
	for(i=0,m=0;i<S;){
		mut=zero_cnt=0;
		tmp=random_lim(0,S-1);
		a=par[i++];
		b=par[tmp];
		for(j=0, hamming_dist=0;j<N;j++) //calculate hamming dist
			if(a.a[j]!=b.a[j]){
				hamming_dist++;
				break;
			}
		if(hamming_dist==0)
			mut=1;
		trace(4,"parents: \n");
		print_ans(4,a);
		print_ans(4,b);
		trace(4,"--------\n");
		//first child
		zero_cnt=0;
		j=random_lim(0+N/10,N*9/10); 	//define point of gap
		for(k=0;k<j;k++)
			child[m].a[k]=a.a[k];
		for(k=j;k<N;k++){
			child[m].a[k]=b.a[k];
			for(l=0;l<j;l++)
				if(child[m].a[l]==b.a[k]){
					child[m].a[k]=0;
					zero_cnt++;
					break;
				}
		}
		//alloc_mem(child[m].dstr,char,mess_size);
		//decr_str(en_str,child[m].dstr,child[m].a);
		if(zero_cnt) //ugly hack, but it works
			fill_zeros(child[m].a);
		if(mut)
			mutate(&child[m]);
		child[m].fitness=child[m].right_char=0;
		trace(4,"child %d : ",m);
		print_ans(4,child[m]);	
		m++;
		//get 1 child	
	}
	for(i=S;i<POPSIZE;i++)
		child[i]=par[i-S];
	
}
/************************************************
 *mm, see one_point_crossover
 * **********************************************/
void fill_zeros(int *a){
	int zero_cnt=0,tmp[N+1],zero[N],i;
	for(i=0;i<N+1;i++)
		tmp[i]=0;
	for(i=0;i<N;i++){
		if(!a[i]){
			zero[zero_cnt++]=i;
			tmp[a[i]]=0;
		}
		else
			tmp[a[i]]=1;
	}
	for(i=1;i<=N;i++)
		if(!tmp[i])
			a[ zero[--zero_cnt] ]=i;
}
/************************************************
 *Just mutation: swap two pieces of gene	
 * **********************************************/
void mutate(ans *a){
	int tmp[2], sw;
	trace(4,"mutate: ");
	print_ans(4,*a);
	tmp[0]=random_lim(0,N-1);
	tmp[1]=random_lim(0,N-1);
	sw=a->a[tmp[0]];
	a->a[tmp[0]]=a->a[tmp[1]];
	a->a[tmp[1]]=sw;
	trace(4,"        ");
	print_ans(4,*a);
}

/************************************************
 *Main part: cicle of generations
 *BUG: it's add equal genes to survivors
 * **********************************************/
void gselection(ans *res){
	int i,j,k; 
	unsigned long long cnt=0;	//gen. count
	ans par[POPSIZE],child[POPSIZE];//parents and childs
	trace(2,"start selection\n");
	randomize();
	get_init_pop(par);
	while(1){	
		if(cnt==gen)
			break;
		for(i=0;i<POPSIZE;i++)
			//if(par[i].dstr!=NULL)
			free(par[i].dstr);
		for(i=0;i<POPSIZE;i++) //process fitness
			if(get_fitness(&par[i])==0){
				if(res_cnt>=s_res)
					goto selection_end;
				for(j=0;j<res_cnt;j++){
					for(k=0;k<N;k++){
						if(res[j].a[k]!=par[i].a[k]){
						//just another ugly hack
							goto inner;
							break;		
						}
					}
					goto outer;
				}	
				inner:
				res=(ans *)realloc(res, (res_cnt+1)*sizeof(ans));
				res[res_cnt++]=par[i];
				outer:			
				trace(2,"got one, genesis - %d\n",cnt);
			}
		//sort by fitness
		qsort(par,POPSIZE,sizeof(ans),cmp);
		trace(3,"+++++++population:%d+++++++++\n",cnt);
		print_ans_arr(3,par,POPSIZE);
		trace(3,"+++++++end pop+++++++++++++\n");
		cnt++;
		one_point_crossover(par,child);
		randomize();
		for(i=0;i<POPSIZE;i++) 	//random mutation
			if(abs(random_lim(0,1000))<=1000*mut_prb)
				mutate(&child[i]);	
		for(i=0;i<POPSIZE;i++) 	//par:=child
			par[i]=child[i];
	}
	selection_end:
	qsort(res,res_cnt,sizeof(ans),cmp); }
/************************************************
 *Calculate fitness	
 *return 0 - if result succesfull, 1 if no
 * **********************************************/
int get_fitness(ans * a){
	int cnt=0;
	char tmp_str[STRLEN];
	trace(4,"counting right char \n");
	decr_str(en_str,tmp_str,a->a);
	trace(4,"dec: %s\n",tmp_str);
	is_any_word(tmp_str,&cnt);
	a->right_char=cnt;
	a->fitness=cnt;
	if(cnt>=suc_thr)
		return 0;
	return 1;
}
/************************************************
 *Comparison of two genes
 *if fitness of a>b - return 1	
 * **********************************************/
int cmp(const void *a, const void *b){
	return (int)(((ans *)b)->fitness - ((ans *)a)->fitness);
}
/************************************************
 *Create init population
 * **********************************************/
void get_init_pop(ans * pop){
	int i,j;
	randomize();
	mess_size=strlen(en_str);
	for(i=0;i<POPSIZE;i++)
		for(j=1;j<=N;j++)
			pop[i].a[j-1]=j;
	for(i=0;i<POPSIZE;i++){
		permute_by_swap(&pop[i]);
		alloc_mem(pop[i].dstr,char,mess_size);
		decr_str(en_str,pop[i].dstr,pop[i].a);
		//printf("encr : %s\n",pop[i].dstr);
	}
	
}
/************************************************
 *Set variables by default
 * **********************************************/
void set_vars(){
	mut_prb=I_MUT_PRB;
	s_res=I_S_RES;
	gen=I_GEN;
	suc_thr=I_SUC_THR;
}
/************************************************
 *Create key, decrypt open message, and alloc 
 *mem for successfull genes
 * **********************************************/
void gen_init(){
	int i,tmp,sw;
	for(i=0;i<N;i++)	//get random key 
		key[i+1]=i+1;	//fill key with numbers from 1 to 26
	for(i=0;i<N-1;i++){	//permute key by swap. it's exactly normal 
		tmp=random_lim(i,N);//distribution (see random algorithms 
		sw=key[i];	//in Introduction to Algorithms)
		key[i]=key[tmp];
		key[tmp]=sw;
	}
	trace(0,"key:");
	for(i=1;i<=N;i++)
		trace(0,"%d ",key[i]);
	trace(0,"\n");
	encr_str(open_str,en_str,key);
	printf("open:%s\n",open_str);
	printf("enc :%s\n",en_str);
	decr_str(en_str,open_str,key);
	printf("open:%s\n",open_str);
	alloc_mem(res,ans,s_res);
}
/************************************************
 *create completely random genotype
 * **********************************************/
void permute_by_swap(ans * x){
	int i, tmp, sw;
	for(i=0;i<N-1;i++){
		tmp=random_lim(i,N);
		//swap
		sw=x->a[i];
		x->a[i]=x->a[tmp];
		x->a[tmp]=sw;
	}
}
/************************************************
 *Output and debug
 * **********************************************/

void print_ans_arr(int lev,ans *a, int n){
	if(lev<0 || lev > debug_level)
		return;
	int i;
	for(i=0;i<n;i++)
		print_ans(lev,a[i]);
}
void print_ans(int lev,ans a){
	if(lev<0 || lev > debug_level || a.fitness==0)
		return;
	int j;
	for(j=0;j<N;j++)
		printf("%d ",a.a[j]);	
	printf("| %f\n",a.fitness);
}
/************************************************
 *random
 * **********************************************/
void randomize(){
	struct timeb tp;
	ftime(&tp);
	srand((unsigned int)tp.millitm);
}
int random_lim(int min,int max){
	//res => [min; max]
	int x=rand();
	return min+x%(max-min);
}
