// C program for merge sort
// Functions taken and adapted from GeeksforGeeks.org : https://www.geeksforgeeks.org/merge-sort/#

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
#include <time.h>

#include "../Serial/SMergesort.cpp"



void Pmergesort(float* arr, int n){
    
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int bufferSize = n / num_procs;
    float* buffer = new float[bufferSize];

    
    MPI_Scatter(arr,bufferSize,MPI_FLOAT,buffer,bufferSize,MPI_FLOAT,0,MPI_COMM_WORLD);
    
    mergeSort(buffer,0,bufferSize-1);

    MPI_Gather(buffer, bufferSize, MPI_FLOAT, arr, bufferSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if(rank==0){
        for (int i = 1; i <= num_procs; i *= 2) {
            // void merge(float arr[], int l, int m, int r)
            int step = i * bufferSize;
            
            // Adjust the loop limit to handle the last chunk
            for (int j = 0; j < n - step; j += step) {
                merge(arr, j, j + step - 1, j + 2 * step - 1);
            }
        }
    }
    // for(int i = 1;i<=num_procs;i*=2){
    //     //void merge(float arr[], int l, int m, int r)
    //     int step = i*bufferSize;
    //     for(int j=0;j<n-2;j+=step){
    //         merge(arr,j*step,(j+1)*step,(j+2)*step);
    //     }
    // }


    delete[] buffer;

}