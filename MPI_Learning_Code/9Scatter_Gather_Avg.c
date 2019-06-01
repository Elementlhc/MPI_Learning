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

    float sum = 0;
    for(int i = 0; i < SIZE_OF_ARR / 4; i++){
        sum += recv_arr[i];
    }
    float avg = sum / (SIZE_OF_ARR / 4);
    // printf("%f\n", avg);
    float recv_avg[4];
    MPI_Gather(&avg, 1, MPI_FLOAT, recv_avg, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if(world_rank == 0){
        float last_avg = (recv_avg[0] + recv_avg[1] + recv_avg[2] + recv_avg[3]) / 4;
        printf("  %f\n", last_avg);

        float big_sum = 0.0;
        for(int i = 0; i < SIZE_OF_ARR; i++)
        {
            big_sum += arr[i];
        }
        printf("原本：%f\n", big_sum/SIZE_OF_ARR);

    }

    MPI_Barrier(MPI_COMM_WORLD);

    // 释放MPI资源
    MPI_Finalize();
    
    return 0;
}

