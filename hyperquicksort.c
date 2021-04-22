//hyper quick sort
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

int cmpfunc (const void * a, const void * b){
   return  *(int*)a >= *(int*)b ? 1 : -1;
}
int* readArrayValues(char* filename, int *n){
	int *A;
	FILE *file = fopen(filename,"r");
    if (file == NULL) {
		fprintf(stderr, "Error while opening file\n");
		exit(EXIT_FAILURE);
	}
	fscanf(file,"%d",n);
    A = (int*) malloc(sizeof(int)* *n);
    for(int i = 0; i < *n; i++){
    	fscanf(file,"%d",&A[i]);
    }
    fclose(file);
    return A;
}
void writeArrayValues(char* filename, int n, int* arr){
	FILE *file = fopen(filename,"w");
    if (file == NULL) {
		fprintf(stderr, "Error while opening file\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0 ; i < n; i++)
		fprintf(file,"%d \n",arr[i]);
   	for(int i = 1 ; i < n; i++)
    	if(arr[i-1] > arr[i])
			printf("ERROR: %d\n",i);
	fclose(file);
	return; 
}
int findMedianIndex(int size, int *arr){
	return arr[size/2];
}
int roundUp(int numToRound, int multiple)
{
    int ans = log(numToRound) / log(2);
    return pow(2, ans);
}
int findPivot(int median, int n, int **arr, int threadRank){
	//can be optimized by doubling technique later
	int pivot = 0;
	while(pivot < n && arr[threadRank][pivot] < median)
        pivot++;
    return pivot;
}
int main(int argc, char* argv[]){
	if(argc != 4){
		fprintf(stderr, "incorrect arguments, use as :\n");
		fprintf(stderr, "./a.out <inputfile.txt> <outputfile.txt> <num_threads>\n");
		exit(EXIT_FAILURE);
	}
	int n, *A;
	A = readArrayValues(argv[1], &n);
	int nthreads = roundUp(atoi(argv[3]), 2);
	int *pivots, *sizes;
	pivots = (int*) malloc(sizeof(int) * nthreads);
	sizes = (int*) malloc(sizeof(int) * nthreads);
	memset(sizes, 0, sizeof(int) * nthreads);
	memset(pivots, 0, sizeof(int) * nthreads);
	
	//shared array accross processes
	int **arr;
	arr = (int **) malloc(nthreads * sizeof(int*));
	for(int i = 0 ; i < nthreads ; i++){
		arr[i] = (int *)malloc(n * sizeof(int));
		for(int j = 0 ; j < n ; j++){
			arr[i][j] = INT_MAX;
		}
	}

	for(int i = 0 ; i < n ; i++){
		arr[i%nthreads][i/nthreads] = A[i];
		sizes[i%nthreads]++;
	}
	int logT = round(log(nthreads) / log(2));
	#pragma omp parallel default (none) num_threads (nthreads) shared (arr, sizes, pivots, n, nthreads, logT)
	{
		int threadRank = omp_get_thread_num();
		int sz = nthreads;
		qsort(arr[threadRank], sizes[threadRank], sizeof(int), cmpfunc);
		for(int iter = 0 ; iter < logT ; iter++){
			int medianBroadcasterThreadRank = pow(2, iter) * threadRank/nthreads;
			medianBroadcasterThreadRank *= sz;
        	int median = arr[medianBroadcasterThreadRank][sizes[medianBroadcasterThreadRank] / 2];
           	int pairThreadRank = (threadRank - medianBroadcasterThreadRank+(sz>>1))%sz + medianBroadcasterThreadRank;
        	int* pairThreadArr = (int*)malloc(sizes[pairThreadRank] * sizeof(int));
			int merge_buf[n];
			pivots[threadRank] = findPivot(median, sizes[threadRank], arr, threadRank);
			int sizeOfPairThread = sizes[pairThreadRank];

			// memcpy(pairThreadArr, arr[pairThreadRank], sizes[pairThreadRank]*sizeof(int));	
  			for(int i = 0 ; i < sizeOfPairThread ; i++){
  				pairThreadArr[i] = arr[pairThreadRank][i];
  			}
        	//wait for all threads to compute the pivot and store in the shared pivots array
           	#pragma omp barrier
           	int pivotOfPairThread = pivots[pairThreadRank];

        	int i,j = 0,k = 0, i_end, j_end;
        	if((threadRank - medianBroadcasterThreadRank) >= (sz>>1)){
        		i = pivots[threadRank], i_end = sizes[threadRank];
            	sizes[threadRank] = sizes[threadRank] - pivots[threadRank] + sizeOfPairThread - pivotOfPairThread ;
            	j_end = sizeOfPairThread;
            	j = pivotOfPairThread;
        	}
        	else{
        		j_end = pivotOfPairThread;
        		i = 0, i_end = pivots[threadRank];
            	sizes[threadRank] = pivots[threadRank] + j_end;
	        }
	        while(i < i_end && j < j_end){
	            if(arr[threadRank][i] < pairThreadArr[j])
	                merge_buf[k++] = arr[threadRank][i++];
	            else
	                merge_buf[k++] = pairThreadArr[j++];
        	}
	        while(i < i_end)
	            merge_buf[k++] = arr[threadRank][i++];
	        while(j < j_end)
	            merge_buf[k++] = pairThreadArr[j++];
			
			// memcpy(arr[threadRank], merge_buf, sizes[threadRank]*sizeof(int));
			for(int i = 0 ; i < sizes[threadRank] ; i++){
				arr[threadRank][i] = merge_buf[i]; 
			}
        	sz /= 2;	
			#pragma omp barrier
		}
	}
	int k = 0;
	for(int i = 0 ; i < nthreads ; i++){
		for(int j = 0 ; j < sizes[i] ; j++){
			A[k++] = arr[i][j];
		}
	}
	writeArrayValues(argv[2], n, A);
	return 0;
}