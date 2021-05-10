#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

//global variable
#define n 4 //array size (n x n)

int a[n][n]; //array to be sorted

int sort_count = 0; //variable to keeptrack of the number of times the array is sorted
int phase_cond = 0; //variable to keeptrack of the number of times the phase
int max_phase = 0;

pthread_t threads[n];
pthread_mutex_t count_m;
pthread_cond_t thres;

void printArray(int arr[][n]){ //prints arrays
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

int bubbleSort(int ind){
    int temp;
    if((sort_count + 1)%2 == 0){
        for(int i = 0; i < n - 1; i++){
            for(int j =  0; j < n - i - 1; j++){
                if(a[j][ind] > a[j + 1][ind]){
                    temp = a[j + 1][ind];
                    a[j + 1][ind] = a[j][ind];
                    a[j][ind] = temp;
                }
            }
        }
    }
    else{
        if((ind + 1)%2 == 0){
            for(int i = 0; i < n - 1; i++){
                for(int j = n - 1; j > i; j--){
                    if(a[ind][j] > a[ind][j - 1]){
                        temp = a[ind][j - 1];
                        a[ind][j - 1] = a[ind][j];
                        a[ind][j] = temp;
                    }
                }
            }
        }
        else{
            for(int i = 0; i < n - 1; i++){
                for(int j = 0; j < n - i - 1; j++){
                    if(a[ind][j] > a[ind][j + 1]){
                        temp = a[ind][j + 1];
                        a[ind][j + 1]= a[ind][j];
                        a[ind][j] = temp;
                    }
                }
            }
        }
    }
}

void *shearsort(void *index){
    int temp;
    long ind = (long)index;

    while(sort_count < max_phase){
        pthread_mutex_lock(&count_m); //mutex block
        
        bubbleSort(ind);

        phase_cond++;

        if(phase_cond == n){
            printArray(a);
            printf("Thread #%ld: Finished Phase #%d \n", ind, sort_count);
            sort_count++;
            phase_cond = 0;

            pthread_cond_broadcast(&thres);
        }
        else{
            printf("Thread #%ld: Now waiting \n", ind);
            pthread_cond_wait(&thres, &count_m);
        }
        pthread_mutex_unlock(&count_m); //mutex unblock
    }
    printf("Done thread %ld \n", ind);    
}

int main(void){
    int value; //value from file
    int thread; //value for thread

    //initialization
    pthread_mutex_init(&count_m, NULL);
    pthread_cond_init(&thres, NULL);

    max_phase = (log(n*n)/log(2)) + 1;

    FILE *readFile = fopen("Lab3 readfile.txt","r");

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            fscanf(readFile, "%d", &value);
            a[i][j] = value;
        }
    }

    printf("Original Array: \n");
    printArray(a); //prints original array from file
 
    for(long i = 0; i < n; i++){
        printf("creating Thread #%ld \n", i);

        thread = pthread_create(&threads[i], NULL, shearsort, (void *)i);
          
        if(thread != 0){
            printf("Error in pthread_create() for thread #%d \n", thread);
            exit(1);
        }
    }
    
    //exits the threads
    pthread_exit(NULL);
    printf("New Array: \n"); 
    printArray(a); //prints the new array
}