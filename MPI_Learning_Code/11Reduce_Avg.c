#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    
    const int SIZE_OF_ARR = 4000;
    float recv_arr[SIZE_OF_ARR/4];
    srand(time(NULL));
    float arr[SIZE_OF_ARR];

    if(world_rank == 0){
        for(int i = 0; i < SIZE_OF_ARR; i++)
        {
            arr[i] = rand() / (float)RAND_MAX;
        }
    }

    MPI_Scatter(arr, SIZE_OF_ARR/4, MPI_FLOAT, recv_arr, SIZE_OF_ARR/4, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float local_sum = 0;
    for(int i = 0; i < SIZE_OF_ARR/4; i++){
        local_sum += recv_arr[i];
    }
    float local_avg = local_sum /(SIZE_OF_ARR/4);

    printf("Process %d, avg: %f\n", world_rank, local_avg);
    float sum;
    MPI_Reduce(&local_avg, &sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(world_rank == 0){
        float last_avg = sum / 4;
        printf("Total avg: %f\n", last_avg);

        float big_sum = 0.0;
        for(int i = 0; i < SIZE_OF_ARR; i++)
        {
            big_sum += arr[i];
        }
        printf("原本：%f\n", big_sum/SIZE_OF_ARR);

    }

    MPI_Barrier(MPI_COMM_WORLD);
    // printf("------------\n");
    // 释放MPI资源
    MPI_Finalize();
    
    return 0;
}

