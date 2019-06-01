#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    int color = world_rank / 4;
    int key = world_rank % 4;
    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, key, &row_comm);

    int row_rank, row_size;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_size(row_comm, &row_size);    

    printf("The rank: %d of %d is row rank: %d of %d\n", world_rank, world_size, row_rank, row_size);

    MPI_Comm_free(&row_comm);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}

