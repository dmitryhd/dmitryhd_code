/*Copyright 2011 Dmitry Khodakov
  coded: 06.04.11
*/

#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

// mark of every message
#define TAG 0
#define BUFSIZE 256

int numproc, myid;
unsigned int N, T, K; // where is T - end time, N - number of partitions of x=[0,1]
                      // K - number of time steps

double *u_curr, *u_next;
double h, t, a;
char logfile[BUFSIZE];



void master();
void node();
void network_initialization(int argc, char *argv[]);
void calc();
double phi_0(double x);
double phi_1(double x);
double phi(double x);
double f(double x, double t);


void network_initialization(int argc, char *argv[]) {
  numproc = 1;  // number of nodes in network
  // getting number of machines
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &numproc);
  // splitting to tasks
}

double phi_0(double x) {
  return 1 - cos(x);
}

double phi_1(double x) {
  return 0;
}

double phi(double x) {
  return 0;
}

double f(double x, double t) {
  return 0;
}


void calc() {
  /*
     du(x,t)/dt = a^2 * d2u(x,t)/dx^2 + f(x,t)
     0<=x<=1
     0<=t<=T
     f(x,t)=0
     u(x,0)=fi(x)=0
     u(0)=fi0(x)=1-cos(t)
     u(1,t)=fi1(x)=0
     a^2 = 0.1
  */
  a = 0.1;
// fill starting array of solution with start conditions:
  u_curr[0] = phi(0);
  for (unsigned int i = 1; i < N; ++i)
    u_curr[i] = phi_0(i);
// here we got results :-)

  return;
}

int main(int argc, char *argv[]) {
// getting size of time step and coordinate step :
  if (argc != 3 && argc != 4) {
    printf("usage: %s T N\n\
where is T - end time, N - number of partitions of x=[0,1]\n", argv[0]);
    return 1;
  }
  T = atoi(argv[1]); // final time
  N = atoi(argv[2]); // number of partitions
  if (argc == 4)
    strcpy(logfile,argv[3]);
  else
    strcpy(logfile,"data.log");

  printf("N = %d, T = %d\n", T, N);
  // allocate memory for solution
  u_curr = (double *)malloc (sizeof(double)*N);
  u_next = (double *)malloc (sizeof(double)*N);
  if (!u_curr || !u_next) {
    perror("Can't allocate memory\n");
    exit(3);
  }
  // calculate time step from condition of stability :
  h = (double)1/N;
  t = 1;
  while (1) {
    if ( (0.2*t/(h*h) ) > 1)
      t /= 2;
    else
      break;
  }
  printf("time step = %1.5f\n", t);
  K = (int)T/t;

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
void master() {
  MPI_Status stat;  // status of receiving. I didn't use it here. Don't delete.
// gathering info
  /*
  for (int i = 1; i < numproc; ++i) {
    long double sended_res[2] = {0, 1}; // res[0] - result, res[1] - a[i]

    MPI_Recv(&sended_res, 2, MPI_LONG_DOUBLE, i, TAG, MPI_COMM_WORLD, &stat);
    local_res += previous_number * sended_res[0];
    previous_number *= sended_res[1];
  }
  */
  FILE *file;
  if (!(file = fopen(logfile, "w"))) {
    perror("Can't open file\n");
    exit(2);
  }
  double cur_x;
  for (unsigned int j = 0; j < K; ++j) {
  // calculate next arr. :
    // fprintf(file, "t = %f\n", t*j);
    u_next[0] = phi(0); // TODO(dmitryhd) : argument phi!
    fprintf(file, "%1.5f %1.5f %1.5f\n", t*j, 0.0, u_next[0]);
    for (unsigned int i = 1; i < N; ++i) {
      u_next[i] = (a*t/(h*h)) * (u_curr[i-1] - 2*u_next[i-1]) + u_next[i-1] +
        t*f(0,0); // TODO(dmitryhd) : second dir. x^2
      cur_x = i*h;
      fprintf(file, "%1.5f %1.5f %1.5f\n",t*j ,cur_x, u_next[i]);
    }
  }
  fclose(file);
}

void node() {
  //MPI_Send(&local_res, 1, MPI_LONG_DOUBLE, 0, TAG, MPI_COMM_WORLD);
}
