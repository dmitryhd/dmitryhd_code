#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]){
	long a, max=-pow(2,24);
	char n;
	do{	
		n=scanf("%ld",&a);
		if(a>max)
			max=a;
	}
	while (n==1);
	printf("%ld",max);
	return 0;
}
