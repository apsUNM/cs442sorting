#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#include "mpi_cannon.hpp"


void gpu_merge(float* A, float* B, float** grid){
  //############################ PASSED PARAMETERS #############################
    // float *A = h_A;
    // float *B = h_B;
    // float **grid = h_grid;


  //######################### INITIALIZING VARIABLES #############################

    //CPU
    //Findout how many processes
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    //GPU
    //Set Size of Blocks and Threads
    dim3 dimBlock(32,32);
    int grid_dim = ceil(n /32.0);
    dim3 dimGrid(grid_dim,grid_dim);
}

//copy_to_cpu_cannon(h_A, h_B, h_C_copy, n, sq_num_procs, rank_row, rank_col);
void copy_to_cpu_cannon(float* A, float* B, float* C,
        int n, int sq_num_procs, int rank_row, int rank_col)
{
  //############################ PASSED PARAMETERS ###############################
    // float *A = h_A;
    // float *B = h_B;
    // float *C = h_C_copy;
    // int n = n;
    // int sq_num_procs = sq_num_procs
    // int rank_row = rank_row
    // int rank_col = rank_col



  //######################### INITIALIZING VARIABLES #############################

    //CPU
    //Findout how many processes
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    //GPU
    //Set Size of Blocks and Threads
    dim3 dimBlock(32,32);
    int grid_dim = ceil(n /32.0);
    dim3 dimGrid(grid_dim,grid_dim);

    //CPU
    //Identify size of array
    int size = n*n;
    int fsize = size * sizeof(float);

    //CPU
    //Create an array of total size
    float* send_A = new float[size];
    float* recv_A = new float[size];
    float* send_B = new float[size];
    float* recv_B = new float[size];


    //GPU
    //Setup Arrays
    float* d_send_A=nullptr;
    float* d_recv_A=nullptr;
    float* d_send_B=nullptr;
    float* d_recv_B=nullptr;

    float* d_C=nullptr;
    
    //GPU
    //cudaMalloc(void** buffer, int size)
    cudaMalloc((void**)&d_send_A,fsize);
    cudaMalloc((void**)&d_recv_A,fsize);
    cudaMalloc((void**)&d_send_B,fsize);
    cudaMalloc((void**)&d_recv_B,fsize);
    cudaMalloc((void**)&d_C,fsize);



    //CPU
    //Idk picking processors to shift with
    int send_proc_A, send_proc_B;
    int recv_proc_A, recv_proc_B;
    int tag_a = 1234;
    int tag_b = 4321;

    //CPU
    //Sets Array c to 0
    memset(C, 0, size*sizeof(float));
    cudaMemcpy(d_C,C,fsize,cudaMemcpyHostToDevice);

  //########################## CANNON INITIAL SHIFT ############################## 
    
    //CPU
    //Sets the send/recv procs to correct values for initial shift
    get_init_procs(rank_row, rank_col, sq_num_procs,
            &send_proc_A, &send_proc_B, &recv_proc_A, &recv_proc_B);
    //Sends the array A to next proc
    communicate(send_proc_A, recv_proc_A, tag_a, size, 
            rank_row && rank_col / rank_row % 2 == 0, A, recv_A);
    //Sends the array B to next proc
    communicate(send_proc_B, recv_proc_B, tag_b, size, 
            rank_col && rank_row / rank_col % 2 == 0, B, recv_B);
    //Does Matrix Multiplication of A and B and puts it in C
    //matmat(n, recv_A, recv_B, C);
    
    
    //GPU
    //CopyPortions to GPU
    cudaMemcpy(d_recv_A,recv_A,fsize,cudaMemcpyHostToDevice);
    cudaMemcpy(d_recv_B,recv_B,fsize,cudaMemcpyHostToDevice);
    //Do Multiplication on GPU
    matrixMultKernel<<<dimGrid,dimBlock>>>(n,d_recv_A,d_recv_B,d_C);



  //########################## CANNON GENERAL SHIFT ##############################

    //CPU
    // Send and recv A and B from neighborhing processes in proc grid
    get_rotation_procs(rank_row, rank_col, sq_num_procs,
            &send_proc_A, &send_proc_B, &recv_proc_A, &recv_proc_B);
    for (int i = 1; i < sq_num_procs; i++)
    {
        //Pointer Swap between send_A and recv_A, Swap between send_B and recv_B
        swap(&send_A, &recv_A, &send_B, &recv_B);
        
        communicate(send_proc_A, recv_proc_A, tag_a, size, rank_col % 2 == 0,
                send_A, recv_A);
        communicate(send_proc_B, recv_proc_B, tag_b, size, rank_row % 2 == 0,
                send_B, recv_B);
        //matmat(n, recv_A, recv_B, C);
        cudaMemcpy(d_recv_A,recv_A,fsize,cudaMemcpyHostToDevice);
        cudaMemcpy(d_recv_B,recv_B,fsize,cudaMemcpyHostToDevice);
        matrixMultKernel<<<dimGrid,dimBlock>>>(n,d_recv_A,d_recv_B,d_C);
    }

  //################################ CLEANUP #####################################

    //GPU
    //Return C
    cudaMemcpy(C,d_C,fsize,cudaMemcpyDeviceToHost);

    //CPU
    //Frees Allocated Arrays
    delete[] send_A;
    delete[] recv_A;
    delete[] send_B;
    delete[] recv_B;

    //GPU
    //Free Allocated Arrays
    //cudaFree(void** buffer)
    cudaFree(d_send_A);
    cudaFree(d_recv_A);
    cudaFree(d_send_B);
    cudaFree(d_recv_B);
    cudaFree(d_C);
}



//cuda_aware_cannon(h_A, h_B, h_C_cuda, n, sq_num_procs, rank_row, rank_col);
void cuda_aware_cannon(float* A, float* B, float* C,
        int n, int sq_num_procs, int rank_row, int rank_col)
{
  //############################ PASSED PARAMETERS ###############################
    // float *A = h_A;
    // float *B = h_B;
    // float *C = h_C_copy;
    // int n = n;
    // int sq_num_procs = sq_num_procs
    // int rank_row = rank_row
    // int rank_col = rank_col



  //######################### INITIALIZING VARIABLES #############################

    //CPU
    //Findout how many processes
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    //GPU
    //Set Size of Blocks and Threads
    dim3 dimBlock(32,32);
    int grid_dim = ceil(n /32.0);
    dim3 dimGrid(grid_dim,grid_dim);

    //CPU
    //Identify size of array
    int size = n*n;
    int fsize = size * sizeof(float);

    //CPU
    //Create an array of total size
    float* send_A = new float[size];
    float* recv_A = new float[size];
    float* send_B = new float[size];
    float* recv_B = new float[size];


    //GPU
    //Setup Arrays
    float* d_send_A=nullptr;
    float* d_recv_A=nullptr;
    float* d_send_B=nullptr;
    float* d_recv_B=nullptr;

    float* d_C=nullptr;
    
    //GPU
    //cudaMalloc(void** buffer, int size)
    cudaMalloc((void**)&d_send_A,fsize);
    cudaMalloc((void**)&d_recv_A,fsize);
    cudaMalloc((void**)&d_send_B,fsize);
    cudaMalloc((void**)&d_recv_B,fsize);
    cudaMalloc((void**)&d_C,fsize);



    //CPU
    //Idk picking processors to shift with
    int send_proc_A, send_proc_B;
    int recv_proc_A, recv_proc_B;
    int tag_a = 1234;
    int tag_b = 4321;

    //CPU
    //Sets Array c to 0
    memset(C, 0, size*sizeof(float));
    cudaMemcpy(d_send_A,A,fsize,cudaMemcpyHostToDevice);
    cudaMemcpy(d_send_B,B,fsize,cudaMemcpyHostToDevice);
    cudaMemcpy(d_C,C,fsize,cudaMemcpyHostToDevice);

  //########################## CANNON INITIAL SHIFT ############################## 
    
    //CPU
    //Sets the send/recv procs to correct values for initial shift
    get_init_procs(rank_row, rank_col, sq_num_procs,
            &send_proc_A, &send_proc_B, &recv_proc_A, &recv_proc_B);
    //Sends the array A to next proc
    cuda_aware_comm(send_proc_A, recv_proc_A, tag_a, size, 
            rank_row && rank_col / rank_row % 2 == 0,cudaMemcpyDeviceToDevice, d_send_A, d_recv_A);
    //Sends the array B to next proc
    cuda_aware_comm(send_proc_B, recv_proc_B, tag_b, size, 
            rank_col && rank_row / rank_col % 2 == 0,cudaMemcpyDeviceToDevice, d_send_B, d_recv_B);
    //Does Matrix Multiplication of A and B and puts it in C
    //matmat(n, recv_A, recv_B, C);
    
    
    //GPU
    //CopyPortions to GPU
    //cudaMemcpy(d_recv_A,recv_A,fsize,cudaMemcpyHostToDevice);
    //cudaMemcpy(d_recv_B,recv_B,fsize,cudaMemcpyHostToDevice);
    //Do Multiplication on GPU
    matrixMultKernel<<<dimGrid,dimBlock>>>(n,d_recv_A,d_recv_B,d_C);



  //########################## CANNON GENERAL SHIFT ##############################

    //CPU
    // Send and recv A and B from neighborhing processes in proc grid
    get_rotation_procs(rank_row, rank_col, sq_num_procs,
            &send_proc_A, &send_proc_B, &recv_proc_A, &recv_proc_B);
    for (int i = 1; i < sq_num_procs; i++)
    {
        //Pointer Swap between send_A and recv_A, Swap between send_B and recv_B
        swap(&d_send_A, &d_recv_A, &d_send_B, &d_recv_B);
        
        cuda_aware_comm(send_proc_A, recv_proc_A, tag_a, size, rank_col % 2 == 0,
                cudaMemcpyDeviceToDevice,d_send_A, d_recv_A);
        cuda_aware_comm(send_proc_B, recv_proc_B, tag_b, size, rank_row % 2 == 0,
                cudaMemcpyDeviceToDevice,d_send_B, d_recv_B);
        //matmat(n, recv_A, recv_B, C);
        //cudaMemcpy(d_recv_A,recv_A,fsize,cudaMemcpyHostToDevice);
        //cudaMemcpy(d_recv_B,recv_B,fsize,cudaMemcpyHostToDevice);
        matrixMultKernel<<<dimGrid,dimBlock>>>(n,d_recv_A,d_recv_B,d_C);
    }

  //################################ CLEANUP #####################################

    //GPU
    //Return C
    cudaMemcpy(C,d_C,fsize,cudaMemcpyDeviceToHost);

    //CPU
    //Frees Allocated Arrays
    delete[] send_A;
    delete[] recv_A;
    delete[] send_B;
    delete[] recv_B;

    //GPU
    //Free Allocated Arrays
    //cudaFree(void** buffer)
    cudaFree(d_send_A);
    cudaFree(d_recv_A);
    cudaFree(d_send_B);
    cudaFree(d_recv_B);
    cudaFree(d_C);
}





