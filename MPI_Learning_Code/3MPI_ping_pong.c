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
    
    int ping_pong_count = 0;
    int PING_PONG_LIMIT = 10;
    int partner_rank = (world_rank + 1) % 2;
    while(ping_pong_count < PING_PONG_LIMIT){
        if(world_rank == ping_pong_count % 2){
            ping_pong_count++;
            MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
            printf("Rank %d sended %d to %d\n", world_rank, ping_pong_count, partner_rank);
        }
        else{
            MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Rank %d received %d from %d\n", world_rank, ping_pong_count, partner_rank);
        }
    }

    // 释放MPI资源
    MPI_Finalize();
    
    return 0;
}