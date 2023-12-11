#include <bits/stdc++.h>
#include <algorithm>
#include <mpi.h>

using namespace std;

void nMerge(float* arrayOfArrays[], int numArrays, int arraySize, float mergedArray[]);
void merge(float arr[], int l, int m, int r);

// big idea for naive MPI implementation:
// 1. Split up the array into subarrays of arraySize/n elements and scatter those
// to the different processors. 
// 2. Then run quicksort recursively in each process.
// 3. Once done, gather all back into different buffers on the root and run a merge
// method on the subarrays.

// big idea for optimized MPI implementation:
// 1. Split up the array into subarrays of arraySize/n elements and scatter those to the different processors.

// 2. Divide the number of processes in two, an upper half and a lower half. Choose a pivot in the top process of the upper half. Broadcast this pivot to the other processes.

// Note one: This will be the most inefficient part about this algorithm. 
// If you use a pivot from one process in all processes, it is very possible that most 
// or all of the elements in one of the subarrays will be less than or greater than the
// pivot. In other words, this is the same as choosing a random pivot, and choosing a 
// random pivot from one subarray to use for all of them is worse than using a random 
// pivot selected from each subarray.

// Note two: You could try, if you have time, to implement an optimized pivot selection
// such as taking the average pivot from each, then using as the pivot the average of 
// those pivots.

// 3. Run quicksort on the elements in each subarray.

// 4. Divide the subarray into halves. 
// Send the upper half of the subarray in the lower half to the reciprocal 
// process in the upper half and receive the lower half from the same process. 
// Send the lower half of the subarray in the upper half to the reciprocal 
// process in the lower half and receive the upper half from the same process. 

// 5. Rerun the entire algorithm, starting from step 2. Continue repeating until either
// the subarray in each process is of size 1 or there are no more free processors. 
// If the latter, run serial quicksort until a subarray size of 1 has been reached.

// 6. At this point, the subarray in process 0 will be sorted, and all elements will be
// smaller than in process n-1. Thus, all subarrays may just be combined without 
// merging straight into the buffer on the root process. This is the end of the 
// algorithm.

int nPartition(float arr[], int low, int high) {
    float pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void nQuicksort(float arr[], int low, int high) {
    //int rank, size;
    //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (low < high) {
            int pi = nPartition(arr, low, high);
            nQuicksort(arr, low, pi - 1);
            nQuicksort(arr, pi + 1, high);
    }

    //MPI_Finalize();
}


void nrunQuicksort(float globalArr[], int arraySize) {
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int subarraySize = arraySize / num_procs;

    float localArray[subarraySize];

    // Scatter the array across different processes
    MPI_Scatter(globalArr, subarraySize, MPI_FLOAT, localArray, subarraySize, MPI_FLOAT, 0, MPI_COMM_WORLD);
    nQuicksort(localArray, 0, subarraySize - 1);
    MPI_Gather(localArray, subarraySize, MPI_FLOAT, globalArr, subarraySize, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    
    
    
    
    /*float* arrayOfArrays[subarraySize];
    for (int i = 0; i < num_procs; ++i) {
	    arrayOfArrays[i] = new float[subarraySize];
    }
*/





    //int bufferSize = n / num_procs;
    //float* buffer = new float[bufferSize];

    
   // MPI_Scatter(arr,bufferSize,MPI_FLOAT,buffer,bufferSize,MPI_FLOAT,0,MPI_COMM_WORLD);
    
    //mergeSort(buffer,0,bufferSize-1);

   // MPI_Gather(buffer, bufferSize, MPI_FLOAT, arr, bufferSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if(rank==0){
        for (int i = 1; i <= num_procs; i *= 2) {
            // void merge(float arr[], int l, int m, int r)
            int step = i * subarraySize;
            
            // Adjust the loop limit to handle the last chunk
            for (int j = 0; j < arraySize - step; j += step) {
                merge(globalArr, j, j + step - 1, j + 2 * step - 1);
            }
        }
    }

    //MPI_Gather(localArray, subarraySize, MPI_FLOAT, arrayOfArrays, subarraySize, MPI_FLOAT, 0, MPI_COMM_WORLD);
    //if(rank == 0) {
    //	nMerge(arrayOfArrays, num_procs, subarraySize, globalArr);
   // }











    // Free the allocated memory
    //for (int i = 0; i < num_procs; ++i) {
    //    delete[] arrayOfArrays[i];
    //}

    // MPI_Finalize();
}







//*******HELPER FUNCTIONS***********

void merge(float arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
 
    // Create temp arrays
    float L[n1], R[n2];
 
    // Copy data to temp arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
 
    // Merge the temp arrays back into arr[l..r
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    // Copy the remaining elements of L[],
    // if there are any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    // Copy the remaining elements of R[],
    // if there are any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void nMerge(float* arrayOfArrays[], int numArrays, int subArraySize, float* mergedArray) {
	// Initialize indices for each array in the array of arrays
	int indices[numArrays] = {0};

	// Loop until all arrays are exhausted
	for (int i = 0; i < numArrays * subArraySize; ++i) {
		float min_value = std::numeric_limits<float>::infinity();
		int min_index = -1;

		// Find the minimum value among the current elements of all arrays
		for (int j = 0; j < numArrays; ++j) {
			if (indices[j] < subArraySize && arrayOfArrays[j][indices[j]] < min_value) {
				min_value = arrayOfArrays[j][indices[j]];
				min_index = j;
			}
		}

		// If no minimum value is found, break the loop
		if (min_index == -1) {
			break;
		}

		// Add the minimum value to the merged array and move the index for the corresponding array
		mergedArray[i] = min_value;
		indices[min_index]++;
	}
}