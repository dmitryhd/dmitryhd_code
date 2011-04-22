/*Copyright 2011 Dmitry Khodakov
  Just sum up complex numbers with MPI
  coded: 1.03.11
*/

#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

// mark of every message
#define TAG 0

int numproc, myid, num;
int from_num, last_num;
long double local_res;

void master();
void node();
long double factorial(uint32_t n);
long double number(uint32_t n);
void network_initialization(int argc, char *argv[]);
void calc();



void network_initialization(int argc, char *argv[]) {
  numproc = 1;  // number of nodes in network
// getting number of machines
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &numproc);
// splitting to tasks
// TODO(dimert): dynamically change task size depending on sum size
// sending tasks
  int pack_size = num/numproc;
  int appendix = num % numproc;
  int calc_size = 0;
  if (myid < appendix)
    calc_size = pack_size + 1;
  else
    calc_size = pack_size;

  from_num = pack_size*myid;
  if (myid < appendix)
    from_num += myid;
  else
    from_num += appendix;

  last_num = from_num + calc_size;
  if (from_num == 0)
    from_num = 1;
}

void calc() {
// first piece of computations
  long double res = 0;  // result of partial computation by node
  for (int n = from_num; n < last_num; ++n) {
    printf("id=%d, num=%LE n=%d\n", myid, number(n), n);
    res += number(n);
  }
  local_res = res;
  printf("my id = %d, from %d, last %d, res %LE\n", myid, from_num, last_num,
      local_res);
  return;
}

long double factorial(uint32_t n) {
  long double res = 1;
  for (uint32_t i = 2; i <= n; ++i)
    res *= i;
  return res;
}


long double number(uint32_t n) {
  //return pow(-1,n) * factorial(2*n) /( (1-2*n)*pow(factorial(n),2)*pow(4,n) );
  int sign;
  if (n % 2 == 0)
    sign = 1;
  else 
    sign = -1;
  long double res;
  res = (long double)sign;
  res *= factorial(2*n);
  res /= factorial(n)*factorial(n);
  res /= (long double)1 - 2*n;
  res /= (long double)pow(4, n);
  //printf("n=%d res=%LE\n", n, res);
  return res;
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: %s number\n", argv[0]);
    return 1;
  }
  num = atoi(argv[1]);
  ++num; // very ugly hack
  network_initialization(argc, argv);
  double tbegin, tend;
  tbegin = MPI_Wtime();
  calc();
  if (myid == 0)
    master();
  else
    node();
  tend = MPI_Wtime();
  if (myid == 0)
    printf("time: %f\n", tend - tbegin);
  MPI_Finalize();
  return 0;
}


// master (id = 0) process, it split task and get first part of it
// args: last_number - bottom of natural sequence.
void master() {
  MPI_Status stat;  // status of receiving. I didn't use it here. Don't delete.
  long double res = 0;     // overall result

// gathering info
  res = local_res;
  for (int i = 1; i < numproc; ++i) {
    long double sended_res = 0;

    MPI_Recv(&sended_res, 1, MPI_LONG_DOUBLE, i, TAG, MPI_COMM_WORLD, &stat);
    printf("get %LE from id %d\n", sended_res, i);
    res += sended_res;
  }
  printf("result: %LE\n", res);
}


// calculating process, takes tasks from master
void node() {
  MPI_Send(&local_res, 1, MPI_LONG_DOUBLE, 0, TAG, MPI_COMM_WORLD);
}
