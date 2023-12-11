// C program for merge sort
// Functions taken and adapted from GeeksforGeeks.org : https://www.geeksforgeeks.org/merge-sort/#

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
#include <time.h>

void matrixMergeSort(float arr[], int n);

/* Function to sort an array using insertion sort*/
void matrixMerge(float arr[], int l, int m, int r){
    //############### Initialization ################
    
    int lengthLeft = m - l;
    int lengthRight = r - m;
    int lengthArr = r - l;
    int** grid = new int*[lengthLeft];
    for (int i = 0; i < lengthLeft; ++i) {
        grid[i] = new int[lengthRight];
    }
    float* buffer = new float[lengthArr];
    //################# COMPARISON ##################
    for(int i = 0; i<lengthLeft; i++){
        for(int j=0;j<lengthRight;j++){
            if(arr[l+i]>arr[m+j]){
                grid[i][j]=1;
            }
            else{
                grid[i][j]=0;
            }
        }
    }
    //################ READ/MERGE ###################
    for(int i=0;i<lengthLeft;i++){
        for(int j=0;j<lengthRight;j++){
            if(grid[i][j]){
                if(i==0){
                    buffer[i+j]=arr[m+j];
                }
                else if(!grid[i-1][j]){
                    buffer[i+j]=arr[m+j];
                }
                if(j==lengthRight-1){
                    buffer[i+j+1]=arr[l+i];
                }
                else if(!grid[i][j+1]){
                    buffer[i+j+1]=arr[l+i];
                }
            }
            else{
                if(i==lengthLeft-1){
                    buffer[i+j+1]=arr[m+j];
                }
                if(j==0){
                    buffer[i+j]=arr[l+i];
                }
            }
        }
    }
    //Sync Point for paralellized systems.
    //#################### SWAP #####################
    for(int i = 0; i<lengthArr;i++){
        arr[l+i]=buffer[i];
    }
    //################### CLEANUP ###################
    for (int i = 0; i < lengthLeft; ++i) {
        delete[] grid[i];
    }
    delete[] grid;
    delete[] buffer;

}

 
// l is for left index and r is right index of the
// sub-array of arr to be sorted
//Modified from SMergesort
void matrixMergeSort(float arr[], int n) {
    int start,end;
    for(int step=1;step<n;step*=2){
        for(start=0;start<n;start+=2*step){
            end=start+2*step;
            matrixMerge(arr,start,start+step,end);
        }
    }
}

// void matrixMergeSort(float arr[], int l, int r){
//     if (l < r) {
//         int m = l + (r - l) / 2;
 
//         // Sort first and second halves
//         matrixMergeSort(arr, l, m);
//         matrixMergeSort(arr, m + 1, r);
 
//         matrixMerge(arr, l, m, r);
//     }
// }