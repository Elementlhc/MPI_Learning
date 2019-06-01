#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void my_bcast(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator){
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(world_rank == root){
        int i;
        for(i = 0; i < world_size; i++){
            if(i != root){
                MPI_Send(data, count, datatype, i, 0, MPI_COMM_WORLD);
                // printf("Rank %d sent to rank %d.\n", root, i);
            }
        }
    }
    else{
        MPI_Recv(data, count, datatype, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("Rank %d received from rank %d.\n", world_rank, root);
    }
}

int main(int argc, char** argv)
{
    // 初始化MPI环境
    MPI_Init(&argc, &argv);
    
    // 获得工作进程数量
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // 获得当前进程的秩（进程号）
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    int data[1600000];
    data[0] = 'h';
    data[1] = 'i';
    data[2] = 0;

    int i;
    int num_trials = 10000;
    double total_my_bcast_time = 0;
    double total_mpi_bcast_time = 0;
    for(i = 0; i < num_trials; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        total_my_bcast_time -= MPI_Wtime();
        my_bcast(data, 400000, MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        total_my_bcast_time += MPI_Wtime();
        
        MPI_Barrier(MPI_COMM_WORLD);
        total_mpi_bcast_time -= MPI_Wtime();
        MPI_Bcast(data, 400000, MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        total_mpi_bcast_time += MPI_Wtime();
    }

    if(world_rank == 0){
        printf("%lf %lf\n", total_my_bcast_time, total_mpi_bcast_time);
    }

    // 释放MPI资源
    MPI_Finalize();
    
    return 0;
}

