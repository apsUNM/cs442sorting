// C program for insertion sort
// Functions taken and adapted from GeeksforGeeks.org : https://www.geeksforgeeks.org/insertion-sort/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
#include <time.h>

void insertionSort(int arr[], int n);
void printArray(int arr[], int n);
bool verifyArray(int arr[],int n);
//void printVerification(int arr[], int n);


/* Function to sort an array using insertion sort*/
void insertionSort(int arr[], int n)
{
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
 
        /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}
 
// A utility function to print an array of size n
void printArray(int arr[], int n)
{
    int i;
    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

/*
 * verifyArray checks the contents of an array and makes sure the are sorted in acsending order.
 * int arr[] is the array of ints that will be verified.
 * int n is the size of the array.
 * returns true if it verified and sorted and false if it is still unsorted.
*/
bool verifyArray(int arr[],int n){
    if(n==1){
        return true;
    }
    int i,prev,curr;
    arr[0]=prev;
    for (i=1;i<n;i++){
        curr = arr[i];
        if(curr<prev){
            return false;
        }
        else{
            prev=curr;
        }
    }
    return true;
}

/*void printVerification(int arr[], int n){
    printf("Verification: %s\n",verifyArray(arr,n)? "true":"false");
}*/
 
/* Driver program to test insertion sort */
int main(int argc, char* argv[])
{
    int arr[] = { 12, 11, 13, 5, 6 };
    int n = sizeof(arr) / sizeof(arr[0]);
 
    insertionSort(arr, n);
    printArray(arr, n);
    printf("Verification: %s\n",verifyArray(arr,n)? "true":"false");
    //printVerification(arr,n);
 
    int totalTests, dataSize;
    if(argc==3){
        totalTests = atoi(argv[1]);
        dataSize = atoi(argv[2]);
    }
    else {
        totalTests = 100;
        dataSize = 25;
    }

    double start=0; 
    double end=0;
    double timeElapsed;
    double msg = 0;
    double *data[(1<<dataSize)];
    int size;
    for(size=1;size<=dataSize;size++){
        double max = 0;
        double min = 1<<31;
        double average = 0;
        int runs;
        int validRuns=0;
        for(runs=0;runs<=totalTests;runs++){

            //Initialize Random Array
            int i;
            for(i=0;i<size;i++){
                data[i] = rand();
            }

            //Barrier
            //Start Timer

            //Sort
            insertionSort(data,1<<size);
            
            //Stop Timer
            timeElapsed=end-start;
            //Verify and Collect Stats
            if(verifyArray(data,1<<size)){
                if(timeElapsed>max){
                    max=timeElapsed;
                }
                if(timeElapsed< min){
                    min = timeElapsed;
                }
                validRuns++;
                average = (average*(runs)+timeElapsed)/(runs+1);

            }
            //
        }
        //Print Results
        printf("n:%d, min:%d, average:%d, max:%d, validRuns: %d/%d\n",size,min,average,max,validRuns,totalTests);
    }
    //MPI_Status recv_status;
    
    //MPI Finalize

    return 0;
}