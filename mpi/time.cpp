/*Copyright 2011 Dmitry Khodakov 
  coded: 2.03.11
*/
#include <mpi.h>
#include <stdlib.h>
#include <stdint.h>

// mark of every message
#define TAG 0

void master();
void node();

int main(int argc, char *argv[]) {
  int myid = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  double tbegin, tend;
  tbegin = MPI_Wtime();
  if (myid == 0)
    master();
  else
    node();
  tend = MPI_Wtime();
  if (myid == 0)
    printf("%f\n", tend - tbegin);
  MPI_Finalize();
  return 0;
}

// master (id = 0) process, it split task and get first part of it
// args: last_number - bottom of natural sequence.
void master() {
  int numproc = 1;  // number of nodes in network
  MPI_Comm_size(MPI_COMM_WORLD, &numproc);
  MPI_Status stat;  // status of receiving. I didn't use it here. Don't delete.
  uint32_t sended_res;  // result of partial computation by node
  for (int i = 1, add_count = 0; i < numproc; ++i, ++add_count) {
    MPI_Recv(&sended_res, 1, MPI_UNSIGNED_LONG, i, TAG, MPI_COMM_WORLD, &stat);
    MPI_Send(&sended_res, 1, MPI_UNSIGNED_LONG, i, TAG, MPI_COMM_WORLD);
  }
}

// calculating process, takes tasks from master
void node() {
  MPI_Status stat;
  uint32_t sended_res;
  MPI_Send(&sended_res, 1, MPI_UNSIGNED_LONG, 0, TAG, MPI_COMM_WORLD);
  MPI_Recv(&sended_res, 1, MPI_UNSIGNED_LONG, 0, TAG, MPI_COMM_WORLD, &stat);
}
