#include <bits/stdc++.h>
#include <algorithm>
#include <mpi.h>
//#include "../Serial/serialQuicksort.cpp"

using namespace std;

int finalSubarraySize;

int swapLowAndHigh(float* arr, int arrLow, int arrHigh, int upperStart, int procLow, int procHigh, float* newBuffer);

void oQuicksort(float* arr, int arrLow, int arrHigh, int procLow, int procHigh);





void orunQuicksort(float *globalArr, int arraySize) {
	// Initialize MPI variables
	int rank, num_procs;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	// initialize the local array that will hold each process's initial subarray.
	// Note: despite receiving a subarray of size = initSubarraySize, localArray
	// must be of size = arraySize, as it is possible that, depending on the chosen
	// pivot, all array elements may be in one process and none in the others.
	int initSubarraySize = arraySize / num_procs;
	float *localArray = new float[initSubarraySize];
	int allSubarraySizes[num_procs];

	// Scatter the array to the processes
	MPI_Scatter(globalArr, initSubarraySize, MPI_FLOAT, localArray, 
			initSubarraySize, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// Run oQuicksort to sort the localArrays
	oQuicksort(localArray, 0, initSubarraySize - 1, 0, num_procs-1);

	// Gather the sizes of the subarrays 
	MPI_Gather(&finalSubarraySize, 1, MPI_INT, allSubarraySizes, 1, MPI_INT, 0, 
			MPI_COMM_WORLD);

/*
	// Calculate displacements for the MPI_Gatherv operation
	int *displacements = new int[num_procs];
	displacements[0] = 0;
	for (int i = 1; i < num_procs; ++i) {
		displacements[i] = displacements[i - 1] + allSubarraySizes[i - 1];
	}
*/
	// Gather all localArrays back into the original array on the root process 
	// (process 0).
/*	MPI_Gatherv(
			localArray, 
			finalSubarraySize, 
			MPI_FLOAT, 
			globalArr, 
			allSubarraySizes, 
			displacements, 
			MPI_FLOAT, 
			0, 
			MPI_COMM_WORLD);
*/
	MPI_Finalize(); // delete me
}

//*******HELPER FUNCTIONS***********


void oQuicksort(float* arr, int arrLow, int arrHigh, int procLow, int procHigh) {
	// Define MPI variables
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Define total # of processors in this oQuicksort iteration (currProcCount) 
	// and total used by next iteration (halfProcs).
	int currProcCount = (procHigh - procLow + 1);
	int halfProcs = currProcCount / 2;

	// Choose a pivot from the subarray on one process and broadcast that to each o
	// the process
	int pivot;
	if (rank == procLow) {
		pivot = arr[arrLow];
	}
	MPI_Comm currProcs;

    	MPI_Comm_split(MPI_COMM_WORLD, procLow <= rank && rank <= procHigh, rank, &currProcs);
	MPI_Bcast(&pivot, 1, MPI_INT, 0, currProcs);

	// Sort the array in-place from arrLow to arrHigh (indices of bounds of 
	// subarray in the current process)
	int lowerIndex = 0, upperIndex = arrHigh-1;
	for (int k = lowerIndex; k == upperIndex; k++) {


		// Lower processes transfer recv_subarray into lower
		if (arr[k] < pivot) {
			swap(arr[lowerIndex], arr[k]);
			lowerIndex++;
		}
		else
		{
			swap(arr[upperIndex], arr[k]);
			upperIndex--;
			k--;
		}
	}
/*
	// for the purposes of recursive calls on the upper and lower halves some lines        // below, identify the start of the upper array.
	int upperStart = lowerIndex;

	// use MPI to send the lower part of the arrays in the upper half of processes to the lower processes and vice versa. Use newBuffer to capture the new subarray generated.	
	float *newBuffer;
	int newBufferSize = swapLowAndHigh(arr, arrLow, arrHigh, upperStart, procLow, procHigh, newBuffer);
	finalSubarraySize = newBufferSize;

	if ((rank / halfProcs) == 0) {
		if (newBufferSize > 1 && currProcCount != 1) {
			oQuicksort(newBuffer, 0, newBufferSize-1, procLow, halfProcs-1);
		}
		else if (newBufferSize > 1 && currProcCount == 1) {
			finalSubarraySize = newBufferSize;
			sQuicksort(newBuffer, 0, newBufferSize-1);
		}
		else {
			finalSubarraySize = newBufferSize;
		}
	} else {
		if (newBufferSize > 1 && currProcCount != 1) {
			oQuicksort(newBuffer, 0, newBufferSize-1, halfProcs, procHigh);
		}
		else if (newBufferSize > 1 && currProcCount == 1) {
			finalSubarraySize = newBufferSize;
			sQuicksort(newBuffer, 0, newBufferSize-1);
		}
		else {
			finalSubarraySize = newBufferSize;
		}
	}
*/
//	arr = newBuffer;
}







int swapLowAndHigh(float* arr, int arrLow, int arrHigh, int upperStart, int procLow, int procHigh, float* newBuffer) {

// Declare MPI variables
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Divide processes into two groups: higher and lower
	int num_procs = procHigh - procLow + 1;
	int group_size = num_procs / 2;
	int group_rank = rank / group_size; // 0 for lower, 1 for higher

	// Determine the rank of the corresponding process in the other group
	int target_rank = (group_rank == 0) ? rank + group_size : rank - group_size;
	// Determine size of the lower and upper subarrays
	int lowerSize = (upperStart - 1) - arrLow + 1;
	int upperSize = arrHigh - upperStart + 1;

	// Initialize incomingSize, the size of the array being received by this process
	int incomingSize;

	// if a process is in the lower half of processes, it receives the upper subarray of the (rank + num_procs)th process.
	if (group_rank == 0) {
		// Send size	
		MPI_Send(&upperSize, 1, MPI_INT, target_rank, 0, MPI_COMM_WORLD);
		MPI_Recv(&incomingSize, 1, MPI_INT, target_rank, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// Initialize the receive buffer
		float arr_recv[incomingSize + lowerSize];

		// Send array
		MPI_Send(&arr[upperStart], upperSize, MPI_FLOAT, target_rank, 2, MPI_COMM_WORLD);
		MPI_Recv(&arr_recv, incomingSize, MPI_FLOAT, target_rank, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// Combine the elements from lowerSize with the received elements
		int j = arrLow;
		for (int i = 0; i < lowerSize; i++) {
			arr_recv[incomingSize + i] = arr[j];
			j++;
		}

		// point newBuffer to arr_recv which is now the array containing smaller elements than those found in upper
		newBuffer = arr_recv;

		// return the size of the new array
		return incomingSize + lowerSize;

		// if a process is in the upper half of processes, it receives the lower subarray of the (rank - num_procs)th process.
	} else {

		// Send size	
		MPI_Send(&lowerSize, 1, MPI_INT, target_rank, 1, MPI_COMM_WORLD);
		MPI_Recv(&incomingSize, 1, MPI_INT, target_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// Initialize the receive buffer
		float arr_recv[incomingSize + upperSize];

		// Send array
		MPI_Send(&arr[arrLow], lowerSize, MPI_FLOAT, target_rank, 3, MPI_COMM_WORLD);
		MPI_Recv(&arr_recv, incomingSize, MPI_FLOAT, target_rank, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int j = upperStart;
		for (int i = 0; i < upperSize; i++) {
			arr_recv[incomingSize + i] = arr[j];
			j++;
		}
		// point newBuffer to arr_recv which is now the array containing smaller elements than those found in upper
		newBuffer = arr_recv;

		// return the size of the new array
		return incomingSize + upperSize;
	}
}

void printArray(float arr[], int size) {
    std::cout << "Array: ";
    for (int i = 0; i < size; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	float arr[4] = {2, 3, 1, 4};
	printArray(arr, 4);
	orunQuicksort(arr, 4);
	printArray(arr, 4);
	return 0;
}
