#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#define SIZE 100

int main(int argc, char** argv){
	int rank;
	int size;
	double t_bgn, t_end;
	char** endptr;
	double T;
	int N, M;
	double h, tau;
	const double a = 0.1;
	double U1[SIZE], U2[SIZE], vsp[2];
	int layer;
	int i, k;
	int lenght;
	int prev, next;
	MPI_Status status;
	FILE* fd;

	MPI_Init(&argc, &argv);

	t_bgn = MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	layer = rank + 1;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	endptr = argv;

	if (argc < 3){
		if (rank == 0)
			printf("You didn't enter T, N\n");
		MPI_Finalize();
		return 0;
	}

	T = strtod(argv[1], endptr);
	if (*endptr == argv[1]){
		if (rank == 0)
			printf("Wrong format of T\n");
		MPI_Finalize();
		return 0;
	}

	N = strtol(argv[2], endptr, 10);
	if (*endptr == argv[2]){
		if (rank == 0)
			printf("Wrong format of N\n");
		MPI_Finalize();
		return 0;
	}


	h = 1.0 / N;
	M = 1;
	while ((2 * (T / M) * a / (h * h)) > 1)
		M++;
	tau = T / M;

	lenght = (N + 1) / size;
	if (lenght > SIZE)
		lenght = SIZE;

	if (layer == M % size ? M % size : size)
		fd = fopen("Davidyuk_Genya.txt", "w");

	
	if (rank == 0)
        	prev = size - 1;
	else
        	prev = rank - 1;
	if (rank == size - 1)
        	next = 0;
        else
        	next = rank + 1;
	
	
	while (layer <= M){
		if (layer == 1){		
			U2[0] = 1 - cos(tau);
			for (k = 0; N + 1 - k >= lenght; k += lenght){
				for (i = 0; i < lenght; i++){
					if ((k == 0) && (i == 0))
						continue;
					else
						U2[i] = 0;
				}
				MPI_Send(U2, lenght, MPI_DOUBLE, next, 0, MPI_COMM_WORLD);
			}
			for (i = 0; i < N + 1 - k; i++){
				U2[i] = 0;
			}
			MPI_Send(U2, lenght, MPI_DOUBLE, next, 0 ,MPI_COMM_WORLD);
		}

		else{		
			for (k = 0; k < N + 1; k += lenght){
				if (k == 0){
					U2[0] = 1 - cos(tau * layer);
					if (layer == M)
						fprintf(fd, "%f\t%f\n", 0.0, U2[0]); 
					MPI_Recv(U1, lenght, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, &status);
				}
				else{
					vsp[0] = U1[lenght - 2];
					vsp[1] = U1[lenght - 1];
					MPI_Recv(U1, lenght, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, &status);
					U2[i] = a * (U1[0] - 2 * vsp[1] + vsp[0]) / (h * h) * tau + vsp[1];
					if (layer != M)
						MPI_Send(U2, lenght, MPI_DOUBLE, next, 0, MPI_COMM_WORLD);
					else
						fprintf(fd, "%f\t%f\n", (k - 1) * h, U2[i]);
					if (N + 1 - k > 1){
						U2[0] = a * (U1[1] - 2 * U1[0] + vsp[1]) / (h * h) * tau + U1[0];
					}
					else
						U2[0] = 0;
					if (layer == M)
						fprintf(fd, "%f\t%f\n", k * h, U2[0]);
				}	
				
				for (i = 1; (i < N - k) && (i < lenght - 1) ; i++){
					U2[i] = a * (U1[i + 1] - 2 * U1[i] + U1[i - 1]) / (h * h) * tau + U1[i];
					if (layer == M)
						fprintf(fd, "%f\t%f\n", (k + i) * h, U2[i]);
				}
			}
			
			k -= lenght;
			if (N + 1 - k > 1){
				U2[i] = 0;
				if (layer == M)
					fprintf(fd, "%f\t%f\n", N * h, U2[i]);
			}
			if (layer != M)
				MPI_Send(U2, lenght, MPI_DOUBLE, next, 0, MPI_COMM_WORLD);
		}
		
		layer += size;
	}

	if (layer == M + size){
		t_end = MPI_Wtime();
		printf("time = %f\n", t_end - t_bgn);
		fclose(fd);
	}

	MPI_Finalize();
	return 0;
}
