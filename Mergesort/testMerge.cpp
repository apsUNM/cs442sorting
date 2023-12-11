/*
Andrei Popa-Simil
December 5th, 2023
CS-442 

Tester Program to time and test a MPI and CUDA sorting algorithms on variously
sized arrays multiple times to gather timings on the algorithms. 

*/

//Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//Required Libraries
#include <time.h>
#include <math.h>
#include <float.h>

//Getopt Library
#include <getopt.h>

//MPI + CUDA
#include <mpi.h>

//Sort Files get included here
//#include "Serial/SMergesort.cpp"
#include "Parallel/PMergesort.cpp"
#include "Serial/MSMergesort.cpp"


//Method Calls
bool verifyArray(float arr[],int n);
void randomize(float arr[], int);
void printHelp();
void printArray(float arr[], int n);
void printTests(int power, int validRuns, double max, double min, double avg);

int main(int argc, char* argv[])
{

    MPI_Init(&argc, &argv);
    
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    if(rank==0){
        printf("Number of processors:%d\n\n",num_procs);
    }
    //Command line arg variables.
    int n=0;
    int s=0;
    char* type = new char[25];
    int verbose =0;

  //########################## CommandLine Args ################################
    int opt;

    while((opt = getopt(argc, argv, "hvs:n:t:")) != -1){
        switch(opt){
            case 'h':
                printHelp();
                break;
            case 'v':
                verbose=0;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'n':
                n = atoi(optarg);
                break;
            case 't':
                strcpy(type,optarg);
                break;
            default : 
                printf("No args\n");
                return 5;
                break;
        }
    }

  //############################### Initialize #################################

    //Timer declaration
    double start, end;
    double timeElapsed;
    double msg = 0; //Remove if not used
    bool valid = 0;

    double min = DBL_MAX;
    double max = 0;
    double avg = 0;
    int validRuns = 0;
    int size=0;
    //Array declaration
    float* h_data = new float[1<<s];
    
    
  //############################### TIME SORT #################################
    for(int power=0;power<=s;power++){ //Loop to go over array sizes
        MPI_Barrier(MPI_COMM_WORLD);
        size=1<<power; //Size of array
        if(rank==0){
            randomize(h_data,size);
        }
        min = DBL_MAX;  //Reset Min
        max = 0;        //Reset Max
        avg = 0;        //Reset Average
        validRuns = 0;  //Reset valid run count

        for(int attempts=0;attempts<n;attempts++){ //Loops to do multiple attemps.
            
            //Barrier to syncronize before timing.
            MPI_Barrier(MPI_COMM_WORLD);
            if(rank==0){
                start=MPI_Wtime();
            }
            
            //##################### START SORT #################################
            
            if(!strcmp(type,"serial")){
                mergeSort(h_data,0,size); //Serialsort...
            }
            else if(!strcmp(type,"matrixSerial")){
                matrixMergeSort(h_data,size); //Serialsort...
            }
            else if(!strcmp(type,"parallel")){
                if(size<=num_procs){
                    
                    mergeSort(h_data,0,size);
                    
                }
                else{
                    Pmergesort(h_data,size);
                }
                
            }
            else{ //Error case... clean up and exit.
                printf("No matching sort selected with -t\n");
                delete[] h_data;
                delete[] type;
                MPI_Finalize();
                return 1;
            }


            //###################### END SORT ##################################
            
            
            if(rank==0){
                end=MPI_Wtime();
            }
            //################# VERIFICATION ###################################
            //Verify array correctness and update stats.
            if(rank==0){
                valid=verifyArray(h_data,size);
                
                if(valid){ //If valid track stats.
                    //printArray(h_data,size); //Danger to output
                    timeElapsed=end-start;//Calculate Time
                    if(timeElapsed>max){max=timeElapsed;}//Check if Max
                    if(timeElapsed<min){min=timeElapsed;}//Check if Min
                    avg=((avg*validRuns)+timeElapsed)/(validRuns+1.0); //Update Avg
                    //printf("avg*valid:%f)
                    validRuns++; //Add another valid run
                    
                }//End of if valid
            }//End of rank 0 checking.
        }//End of Attempts Loops
        if(rank==0){
            if(min==DBL_MAX){
                min=-1;
            }
            printTests(power,validRuns,max,min,avg);
        }
    }//End of Size Loops

  //############################### CLEAN UP #################################
    delete[] h_data;
    delete[] type;


    MPI_Finalize();
    return 0;
}

//############################ HELPER FUNCTIONS ################################

void printTests(int power, int validRuns, double max, double min, double avg){
    printf("Power: %d, Valid: %d, Max Time: %lf, Min Time: %lf, Avg Time: %lf\n",
            power,validRuns,max,min,avg);
}

// A utility function to print an array of size n
//Use only on small arrays as it quickly floods the output with n>16 or s>4
void printArray(float arr[], int n)
{
    //printf("Called print Array on size: %d\n",n);
    for(int i = 0; i < n; i++)
        printf("%lf ", arr[i]);
    printf("\n");
}

//Prints usage and options of this file.
void printHelp(){
    printf("Usage: ./testMerge [-h] -s <s> -m <m> -t <sort>\n");
    printf("-h: Optional help flag that prints usage info\n");
    printf("-s <s>: Max size of array to test (1<<m)\n");
    printf("-n <n>: Number of tests per array size\n");
    printf("-t <sort>: type of sort to test. ie: serial, parallel, or gpu)\n");
}



bool verifyArray(float arr[],int n){
    if(n==1){
        return true;
    }
    float prev,curr;
    arr[0]=prev;
    for(int i=1;i<n;i++){
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


void randomize(float arr[], int n){
    float min = 0.0;
    float max = 9.9;
    for (int i = 0; i < n; i++) { 
        float num = ((float)rand() / RAND_MAX) * (max - min) + min;
        arr[i]=num; 
    } 
}