#include <mpi.h>
#include <stdio.h>

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
    
    int token;
    if(world_rank != 0){
    MPI_Recv(&token, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Rank %d received %d from %d\n", world_rank, token, world_rank-1);
    }else{
        token = -1;
    }

    MPI_Send(&token, 1, MPI_INT, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);

    if(world_rank == 0){
        MPI_Recv(&token, 1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank %d received %d from %d\n", world_rank, token, world_size-1);
    }

    // 释放MPI资源
    MPI_Finalize();
    
    return 0;
}