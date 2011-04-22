#include <math.h>
#include <stdlib.h>
#include <stdio.h>

long int Fact (long int n) 
{ 
  if(n == 0)
    return 1;
  long int r = 1; 
  for (long int i=2; i<=n; i++) 
    r *= i; 
  return r; 
}

int main(int argc, char *argv[])
{
  double res = 0.0;
  double x = 1.0;
  long int iter = atoi(argv[2]);
  long int prec = atoi(argv[1]);
  for(long int i = 0; i < iter; ++i)
  {
    res = 0.0;
    for(long int j = 0; j < prec; ++j)
    {
      res += pow((-1),j) * pow(x,2*j) / Fact(j);
    }
    //printf("res = %f\n", res);
  }  
  return 0;
}
