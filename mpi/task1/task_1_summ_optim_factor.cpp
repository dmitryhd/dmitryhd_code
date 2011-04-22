/*Copyright 2011 Dmitry Khodakov gr 811
  task 1
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
long double previous_number;

void master();
void node();
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
  local_res = 0;
  previous_number = 1;
  for (int n = from_num; n < last_num; ++n) {
    previous_number *= (long double)2*n / (n*n*4);
    previous_number *= -1;
    local_res += previous_number/(1-2*n);
    printf("id:%d last=%Lf, n=%d\n", myid, previous_number/(1-2*n), n);
  }
 // printf("my id = %d, from %d, last %d, res %LE\n", myid, from_num, last_num, local_res);
  return;
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: %s number\n", argv[0]);
    return 1;
  }
  num = atoi(argv[1]);
  ++num; // for using for iterator
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

// gathering info
  for (int i = 1; i < numproc; ++i) {
    long double sended_res[2] = {0, 1}; // res[0] - result, res[1] - a[i]

    MPI_Recv(&sended_res, 2, MPI_LONG_DOUBLE, i, TAG, MPI_COMM_WORLD, &stat);
    printf("get res=%LE last=%LE from id %d\n", sended_res[0], sended_res[1], i);
    local_res += previous_number * sended_res[0];
    previous_number *= sended_res[1];
  }
  printf("result: %LE\n", local_res);
}


// calculating process, takes tasks from master
void node() {
  MPI_Send(&local_res, 1, MPI_LONG_DOUBLE, 0, TAG, MPI_COMM_WORLD);
}
