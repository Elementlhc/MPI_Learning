#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct $
{
    int comm_rank;
    union{
        float f;
        int i;
    } number;
}CommRankNumber;

void TMPI_Rank(void *send_data, void *recv_data,  MPI_Datatype datatype, MPI_Comm comm);

void* gather_numbers_to_root(void *number, MPI_Datatype datatype, MPI_Comm comm);

int *get_ranks(void *gathered_numbers, int gathered_number_count, MPI_Datatype datatype); 

int compare_float_comm_rank_number(const void *a, const void *b);

int compare_int_comm_rank_number(const void *a, const void *b);

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
    
    srand(time(NULL) * world_rank);
    float num = rand() / (float)RAND_MAX;
    int my_rank;
    TMPI_Rank(&num, &my_rank, MPI_FLOAT, MPI_COMM_WORLD);
    printf("Rank for %f on process %d - %d\n", num, world_rank, my_rank);

    MPI_Barrier(MPI_COMM_WORLD);
    // 释放MPI资源
    MPI_Finalize();
    
    return 0;
}

void TMPI_Rank(void *send_data, void *recv_data,  MPI_Datatype datatype, MPI_Comm comm){
    int comm_rank;
    MPI_Comm_rank(comm, &comm_rank);
    int comm_size;
    MPI_Comm_size(comm, &comm_size);

    void* gathered_numbers = gather_numbers_to_root(send_data, datatype, comm);

    int* ranks = NULL;
    if(comm_rank == 0){
        ranks = get_ranks(gathered_numbers, comm_size, datatype);
    }

    MPI_Scatter(ranks, 1, MPI_INT, recv_data, 1, MPI_INT, 0, comm);

    if(comm_rank == 0){
        free(gathered_numbers);
        free(ranks);
    }
}

void* gather_numbers_to_root(void *number, MPI_Datatype datatype, MPI_Comm comm){
    int world_rank;
    int world_size;
    MPI_Comm_rank(comm, &world_rank);
    MPI_Comm_size(comm, &world_size);

    int datatype_size;
    MPI_Type_size(datatype, &datatype_size);
    void* gathered_numbers;
    if(world_rank == 0)
    {
        gathered_numbers = malloc(datatype_size * world_size);
    }

    MPI_Gather(number, 1, datatype, gathered_numbers, 1, datatype, 0, comm);
    return gathered_numbers;
}

int *get_ranks(void *gathered_numbers, int gathered_number_count, MPI_Datatype datatype){
    int datatype_size;
    MPI_Type_size(datatype, &datatype_size);
    CommRankNumber *comm_rank_numbers = malloc(sizeof(CommRankNumber) * gathered_number_count);
    int i;
    for(i = 0; i < gathered_number_count; i++)
    {
        comm_rank_numbers[i].comm_rank = i;
        memcpy(&(comm_rank_numbers[i].number), gathered_numbers + (i * datatype_size), datatype_size);
    }

    if(datatype == MPI_FLOAT){
        qsort(comm_rank_numbers, gathered_number_count, sizeof(CommRankNumber), &compare_float_comm_rank_number);
    }
    else{
        qsort(comm_rank_numbers, gathered_number_count, sizeof(CommRankNumber), &compare_int_comm_rank_number);

    }

    int *res = malloc(sizeof(int)*gathered_number_count);
    for(i = 0; i < gathered_number_count; i++)
    {
        // res[i] = comm_rank_numbers[i].comm_rank;
        res[comm_rank_numbers[i].comm_rank] = i;
    }
    free(comm_rank_numbers);
    return res;
}

int compare_float_comm_rank_number(const void *a, const void *b){
    CommRankNumber * aa = (CommRankNumber*)a;
    CommRankNumber * bb = (CommRankNumber*)b;

    if(aa->number.f < bb->number.f) return -1;
    if(aa->number.f > bb->number.f) return 1;
    return 0;
}

int compare_int_comm_rank_number(const void *a, const void *b){
    CommRankNumber * aa = (CommRankNumber*)a;
    CommRankNumber * bb = (CommRankNumber*)b;

    if(aa->number.i < bb->number.i) return -1;
    if(aa->number.i > bb->number.i) return 1;
    return 0;
}