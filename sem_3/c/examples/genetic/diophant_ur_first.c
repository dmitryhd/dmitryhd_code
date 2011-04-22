#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
//#include <sys\types.h>
#define N 3	//количество неизвестных
#define POPSIZE 100 //величина популяции

typedef struct _ans{
	int a[N]; //корни
	float fitness;
} ans;

int koeff[N], S, r; // коэффициенты ур-я, количество выживших решений, r = свободный член
int max_ans;	 // максимальное значение корня
float mut_ver;	 // вероятность мутации i-го решения
int n, popsize;


ans genetic_sel();
int get_rand(int max);
int get_fitness(ans a);
int cmp(const void *a, const void *b);
ans genetic_sel();
void crossover(ans *par, ans *child);
void mutate(ans *a);
void print_pop(ans *a);
void print_ans(ans a);

int main(int argc, char *argv[]){
	mut_ver=0.1;
	
	koeff[0]=1;
	koeff[1]=1;
	koeff[2]=3;
	r=100;	
	max_ans=r;
	//printf("%d = (%d) + (%d)\n",r,res.a[0],res.a[1], res.a[2],res.a[3]);
	
/*	int i;
	for(i=0;i<N;i++)
		scanf("%d ",&koeff[i]);
	scanf("%d",&r);
*/
	ans res;
	res=genetic_sel();
	print_ans(res);
	return 0;
}

int get_rand(int max){
	//res => (-max ; max)
	return rand()%(max*2)-max/2;
}

int get_fitness(ans a){
	//return - 0 if correct ansver
	int res=0,i;
	for(i=0;i<N;i++)
		res+=a.a[i]*koeff[i];
	res=abs(r-res);
	if (res==0)
		return 0;
	a.fitness=1/res;
	return 1;
}
int cmp(const void *a, const void *b){
	return (int)(((ans *)a)->fitness - ((ans *)b)->fitness)*1000;
}

void crossover(ans *par, ans *child){
	int i,j,k,m;
	ans a,b;
	for(m=0;m<S;){
		a=par[m++];
		b=par[m++];
		for(i=0,k=1;k<N;k++){
			for(j=0;j<k;j++)
				child[i].a[j]=a.a[j];
			for(j=k;j<N;j++)
				child[i].a[j]=b.a[j];
			i++;
			for(j=0;j<k;j++)
				child[i].a[j]=b.a[j];
			for(j=k;j<N;j++)
				child[i].a[j]=a.a[j];
			i++;
		}
		//get N-1 * 2 child	
	}
}
void mutate(ans *a){
	int i;
	for(i=0;i<POPSIZE;i++)
		if(abs(get_rand(1000))>=1000*mut_ver) // random mutation ver.
			a[i].a[abs(get_rand(N))]=get_rand(max_ans);
}

void print_pop(ans *a){
	int i,j;
	for(i=0;i<POPSIZE;i++){
		for(j=0;j<N;j++)
			printf("%d ",a[i].a[j]);
		printf("\n");
	}
}
void print_ans(ans a){
	int j;
	for(j=0;j<N;j++)
		printf("%d ",a.a[j]);
	printf("\n");
}

ans genetic_sel(){
	int i,j;
	struct timeb tp;
	ans par[POPSIZE],child[POPSIZE];
	//because crossover gen N-1 * 2 children
	S=POPSIZE/((N-1)*2);
//err?
	if(S%2)
		S--;	//even
	//printf("s=%d\n",S);

	ftime(&tp);
	srand((unsigned int)tp.millitm);

	//set random ans
	for(i=0;i<POPSIZE;i++){
		for(j=0;j<N;j++)
			par[i].a[j]=get_rand(max_ans);
	}
	//printf("random gen:\n");
	//print_pop(par);
	while(1){
		//process fitness
		//is any valid?
		printf("iteration\n");
		for(i=0;i<POPSIZE;i++)
			if(get_fitness(par[i])==0){
			//	printf("got it!: ");
			//	print_ans(par[i]);
				return par[i];
			}
			
		//sort by fitness
		qsort(par,POPSIZE,sizeof(ans),cmp);

		//crossover
		crossover(par,child);	
		
		//mutate
		ftime(&tp);
		srand((unsigned int)tp.millitm);
		mutate(child);	

		//par=child;
		for(i=0;i<POPSIZE;i++)
			par[i]=child[i];
	}
}
