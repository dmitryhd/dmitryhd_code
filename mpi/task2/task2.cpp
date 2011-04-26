/*Copyright 2011 Dmitry Khodakov
  Solve Diffusion equation with pipelining data transmission
  coded: 1.03.11
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define BUFFSIZE 100
#define TAG 0

const char FILENAME[] = "task2_data.dat";

void usage_and_exit( char ** argv, int rank)
{
		if (rank == 0)
			fprintf(stderr, "usage %s: Time, N(number of points)\n", argv[1]);
		MPI_Finalize();
		exit(0);
}

int main(int argc, char** argv)
{

	int rank, size; // our mpi rank, and network size
	int prev, next; // ranks of neighbours to send and receive data

	double start_time, end_time; // for time measuring
  // task params:
	double T, h, tau, a = 0.1;
	int N, M, layer, lenght, i, k;
	MPI_Status status;
	FILE* fd;
  // current and next layer
	double u0[BUFFSIZE], u1[BUFFSIZE], border[2];

	MPI_Init(&argc, &argv);
	start_time = MPI_Wtime();
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	layer = rank + 1;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (argc < 3)
    usage_and_exit(argv, rank);

	T = strtod(argv[1], NULL);
	N = strtol(argv[2], NULL, 10);

	if (T == 0 || N == 0) // in case of fail of strtod
    usage_and_exit(argv, rank);
  if (rank == 0)
    printf("T=%f, N=%d\n", T, N);

	h = 1.0 / N;
	M = 1;
  // calculate time step from condition of stability :
	while ((2 * (T / M) * a / (h * h)) > 1)
		M++;
	tau = T / M;
	lenght = (N + 1) / size;
	if (lenght > BUFFSIZE)
		lenght = BUFFSIZE;

  // ?
  if (layer == M % size ? M % size : size)
    fd = fopen(FILENAME, "w");

  // calculate neighbour id to sending and receiving data
  if (rank == 0)
    prev = size - 1;
  else
    prev = rank - 1;
	if (rank == size - 1)
    next = 0;
  else
    next = rank + 1;

  // calculation loop:
	while (layer <= M) {
		if (layer == 1) {
			u1[0] = 1 - cos(tau);
  // fill solution with start conditions
			for (k = 0; N + 1 - k >= lenght; k += lenght) {
				for (i = 0; i < lenght; i++)
					if ((k == 0) && (i == 0))
						continue;
					else
						u1[i] = 0;
				MPI_Send(u1, lenght, MPI_DOUBLE, next, TAG, MPI_COMM_WORLD);
			}

			for (i = 0; i < N + 1 - k; ++i)
				u1[i] = 0;

			MPI_Send(u1, lenght, MPI_DOUBLE, next, TAG, MPI_COMM_WORLD);
		}

		else {
			for (k = 0; k < N + 1; k += lenght){
				if (k == 0) {
          // border condition
					u1[0] = 1 - cos(tau * layer);
          // print result:
					if (layer == M)
						fprintf(fd, "%1.4f %1.4f\n", 0.0, u1[0]);
					MPI_Recv(u0, lenght, MPI_DOUBLE, prev, TAG, MPI_COMM_WORLD, &status);
				}
				else {
					border[0] = u0[lenght - 2];
					border[1] = u0[lenght - 1];

					MPI_Recv(u0, lenght, MPI_DOUBLE, prev, TAG, MPI_COMM_WORLD, &status);

					u1[i] = a * (u0[0] - 2 * border[1] + border[0]) / (h * h) * tau + border[1];

					if (layer != M)
						MPI_Send(u1, lenght, MPI_DOUBLE, next, 0, MPI_COMM_WORLD);
					else
						fprintf(fd, "%1.4f %1.4f\n",(k - 1) * h, u1[0]);
					if (N + 1 - k > 1)
            // difference scheme
						u1[0] = a * (u0[1] - 2 * u0[0] + border[1]) / (h * h) * tau + u0[0];
					else
						u1[0] = 0;
          // print result:
					if (layer == M)
						fprintf(fd, "%1.4f %1.4f\n",k * h, u1[0]);
				}

				for (i = 1; (i < N - k) && (i < lenght - 1) ; ++i){
					u1[i] = a * (u0[i + 1] - 2 * u0[i] + u0[i - 1]) / (h * h) * tau + u0[i];
          // print result:
					if (layer == M)
					  fprintf(fd, "%1.4f %1.4f\n", (k + i) * h, u1[i]);
				}
			}
			k -= lenght;
			if (N + 1 - k > 1){
				u1[i] = 0;
        // print result:
				if (layer == M)
					fprintf(fd, "%1.4f %1.4f\n",N * h, u1[i]);
			}
			if (layer != M)
				MPI_Send(u1, lenght, MPI_DOUBLE, next, 0, MPI_COMM_WORLD);
		}

		layer += size;
	}
  layer -= size; // last iteration compensation
	if (layer == M){
		end_time = MPI_Wtime();
		printf("running time :%f\n", end_time - start_time);
		fclose(fd);
	}
	MPI_Finalize();
	return 0;
}
