#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char *argv[]) {

  int i,j,N,K,T,base = 10, myid, numprocs;
  int a,b;
  char *nptr, *endptr;
  char ptr[] = "myfile.txt";
  int seg;
  int current_level  = 0;
  double start, end1;
  double h,t;
  MPI_Status stat;

  if(argc != 3){
    printf("Invalid argument\n");
    return -1;
  }

  nptr = argv[1];
  N = strtol(nptr, &endptr, base);

  nptr = argv[2];
  T = strtol(nptr, &endptr, base);

  h = (double)1/N;
  t = 1;

  while(1){
    if((0.2*t/(h*h)) <= 1)
      break;
    else
      t/=2;
  }

  K = (int)T/t;

  double X[N+1], Y[K+1];
  double F[K+1];

  for(i = 0;i<N;i++)
    X[i] = i*h;
  X[N] = 1;

  for(i = 0;i < K;i++){
    Y[i] = i*t;
    F[i] = 1 - cos(Y[i]);
  }
  Y[K] = T;
  F[K] = 1 - cos(Y[K]);

  double U[N+1], U1[N+1];
  for(i = 0;i<=N;i++)
    U[i] = U1[i] = 0;

  double c = 0.1 * t /(h*h);

  MPI_Init(&argc, &argv);
  start = MPI_Wtime();
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  //printf("\n %d %d %lf %lf\n\n", N, T, t, h);

  //seg = (int)ceil((double)N/numprocs);
  //a = (myid * seg) +1;
  //if(myid == numprocs - 1)
  //   b = N;
  //else
  //   b = (myid + 1) * seg;
  seg = numprocs;
  //double send_arg;
  double send[seg];
  // for(i = 0;i<=K;i++)
  // printf("%lf\n",F[i]);

  current_level = myid;
  int end = 0;//the end of cycle
  int begin = 0;//the begin of level
  int srank,rrank;

  if(myid == 0) rrank = numprocs - 1;
  else rrank = myid - 1;
  if(myid == numprocs - 1) srank = 0;
  else srank = myid + 1; // ???
  int m;
  if(numprocs != 1){
    while(1){
      if(current_level >= K) break;  //is there  a work
      j = 0;
      m = 0;
      //fill level
      while(1){
        if(current_level == 0){
          for(i = 0;i < seg;i++){
            if(j == 0) send[i] = U1[j] = F[current_level];
            else{
              if(j == N){
                send[i] = U1[j] = 0;
                MPI_Send(send, seg, MPI_DOUBLE, 1, 0,MPI_COMM_WORLD);
                end = 1;
                break;
              }
              send[i] = U1[j] = U[j] + c * (U[i+1] - 2 * U[i] + U[i-1]);
            }
            j++;
          }
          if(end == 1) break;
          MPI_Send(send,seg,MPI_DOUBLE,1,0,MPI_COMM_WORLD);
        }


        else{
          if(begin != 0){
            //recieve data
            MPI_Recv(send,seg,MPI_DOUBLE,rrank,0,MPI_COMM_WORLD,&stat);
            for(i = 0;i < seg;i++){
              if(m == N){
                U[m] = send[i];
                break;
              }
              U[m] = send[i];
              m++;
            }

            //process the result
            for(i = 0;i < seg; i++){
              if(j == N){
                send[i] = U1[j] = 0;
                MPI_Send(send, seg, MPI_DOUBLE, srank,0,MPI_COMM_WORLD);
                end = 1;
                break;
              }
              send[i] = U1[j] = U[j] + c * (U[i+1] - 2 * U[i] + U[i-1]);
              j++;
            }
            if(end == 1) break;
            MPI_Send(send,seg,MPI_DOUBLE, srank,0,MPI_COMM_WORLD);
          }

          else{
            if(myid  == 0){
              MPI_Recv(send,seg,MPI_DOUBLE,rrank,0,MPI_COMM_WORLD, &stat);
              U[0] = send[0];
              begin = 1;
            }
            else{
              MPI_Recv(send,seg,MPI_DOUBLE,rrank,0,MPI_COMM_WORLD,&stat);
              for(i=0;i<seg-myid+1;i++)
                U[i] = send[i];
              send[0] = U1[0] = F[current_level];
              for(i = 1;i<seg-myid;i++)
                send[i] = U1[i] = U[i] + c * (U[i+1] - 2*U[i] + U[i-1]);
              MPI_Send(send,seg,MPI_DOUBLE,srank,0,MPI_COMM_WORLD);
            }
          }
        }
      }
      if(current_level == K-1){
        for(i=0;i<=N;i++)
          printf("%lf ",U1[i]);
        printf("\n");
        break;
      }
      //current_level+=numprocs;
      printf("\n\n %d \n\n",current_level);
      current_level+=numprocs;
    }

  }

  end = MPI_Wtime();
  MPI_Finalize();
  /* if(myid == 0){
     FILE *in;
     in = fopen(ptr, "w");

     printf("\n\n%lf\n\n",end - start);
     fprintf(in, "%lf \n X[i],     U(x,T)\n", end-start);
     for(i=0;i<=N;i++){
     fprintf(in,"%lf    %lf\n", X[i], U1[i]);
     }
     fclose(in);

     }*/

  return 0;
}

