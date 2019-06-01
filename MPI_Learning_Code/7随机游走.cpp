#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>

using namespace std;

typedef struct $
{
    int location;
    int num_steps_left_in_walk;
}Walker;

void decompose_domain(int domain_size, int world_rank, int world_size, int* subdomain_start, int* subdomain_size);
void initialize_walkers(int num_walkers_per_proc, int max_walk_size, int subdomain_start, int subdomain_size, vector<Walker>* incoming_walkers);
void walk(Walker* walker, int subdomain_start, int subdomain_size, int domain_size, vector<Walker>* outgoing_walkers);
void send_outgoing_walkers(vector<Walker>* outgoing_walkers, int world_rank, int world_size);
void receive_incoming_walkers(vector<Walker>* incoming_walkers, int world_rank, int world_size);

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

    int domain_size = 100;
    int subdomain_start;
    int subdomain_size;
    int max_walk_size = 500;
    int num_walkers_per_proc = 20;
    vector<Walker> incoming_walkers;
    vector<Walker> outgoing_walkers;

    decompose_domain(domain_size, world_rank, world_size, &subdomain_start, &subdomain_size);
    initialize_walkers(num_walkers_per_proc, max_walk_size, subdomain_start, subdomain_size, &incoming_walkers);

    int max_sends_recvs = max_walk_size / (domain_size / world_size) + 1;

    for(int m = 0; m < max_sends_recvs; m++){
        for(int i = 0; i < incoming_walkers.size(); i++){
            walk(&incoming_walkers[i], subdomain_start, subdomain_size, domain_size, &outgoing_walkers);
        }

        if(world_rank % 2 == 0){
            send_outgoing_walkers(&outgoing_walkers, world_rank, world_size);
            receive_incoming_walkers(&incoming_walkers, world_rank, world_size);
        }
        else{
            receive_incoming_walkers(&incoming_walkers, world_rank, world_size);
            send_outgoing_walkers(&outgoing_walkers, world_rank, world_size);
        }
    }

    // 释放MPI资源
    MPI_Finalize();
    
    return 0;
}

void decompose_domain(int domain_size, int world_rank, int world_size, int* subdomain_start, int* subdomain_size){

    if (world_size > domain_size) {
        // Don't worry about this special case. Assume the domain
        // size is greater than the world size.
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int size_every = domain_size / world_size;
    *subdomain_start = world_rank * size_every;
    if(world_rank != world_size - 1){
        *subdomain_size = size_every;
    }
    else{
        *subdomain_size = size_every + domain_size % world_size;
    }
}

void initialize_walkers(int num_walkers_per_proc, int max_walk_size, int subdomain_start, int subdomain_size, vector<Walker>* incoming_walkers){
    Walker walker;
    for(int i = 0; i < num_walkers_per_proc; i++)
    {
        walker.location = subdomain_start;
        walker.num_steps_left_in_walk = (int)((rand() / (float)RAND_MAX) * max_walk_size);
        incoming_walkers->push_back(walker);
    }
}

void walk(Walker* walker, int subdomain_start, int subdomain_size, int domain_size, vector<Walker>* outgoing_walkers){
    while(walker->num_steps_left_in_walk > 0){
        if(walker->location < subdomain_start + subdomain_size){
            walker->location++;
            walker->num_steps_left_in_walk--;
        }
        else{
            if(walker->location == domain_size)
                walker->location = 0;
            outgoing_walkers->push_back(walker);
            break;
        }
    }
}

void send_outgoing_walkers(vector<Walker>* outgoing_walkers, int world_rank, int world_size){
    MPI_Send((void*)outgoing_walkers->data(), outgoing_walkers->size() * sizeof(Walker), MPI_BYTE, (world_rank+1) % world_size, 0, MPI_COMM_WORLD);
    outgoing_walkers->clear();
}

void receive_incoming_walkers(vector<Walker>* incoming_walkers, int world_rank, int world_size){
    MPI_Status status;
    int incoming_rank = world_rank==0 ? world_size-1 : world_rank-1;
    MPI_Probe(incoming_rank, 0, MPI_COMM_WORLD, &status);

    int incoming_walkers_size;
    MPI_Get_count(&status, MPI_INT, &incoming_walkers_size);
    // int *recvNumbers = (int*)malloc(sizeof(int)*incoming_walkers_size);
    MPI_Recv((void*)incoming_walkers->data(), incoming_walkers_size * sizeof(Walker), MPI_BYTE, incoming_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("1 received %d numbers from 0.\n Message source: %d.\n Message tag: %d\n", number_amount, status.MPI_SOURCE, status.MPI_TAG);
    free(recvNumbers);
}