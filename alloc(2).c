#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include <fcntl.h>

#include <string.h>

#define KEY (1492)

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(){
    //semaphore initalization
    int id = semget(KEY, 1, 0666 | IPC_CREAT);
    if(id < 0){
        printf("Error with semget()");
        exit(0);
    }
    struct sembuf op1 = {0, -1, SEM_UNDO};
    struct sembuf op2 = {0, 1, SEM_UNDO};
    
    union semun u;
    u.val = 1;
    if(semctl(id, 0, SETVAL, u) < 0){
        printf("Error with semctl()");
        exit(0);
    }

    //getting file size
    const char *filepath = "/home/sh3/Desktop/res.txt";
    int value = open("/home/sh3/Desktop/res.txt", O_RDWR);
    if(value < 0){
        printf("Error with open() \n");    
        exit(0);
    }

    //gets the file information
    struct stat buffer;
    int err;
    if(err = fstat(value, &buffer)){
        printf("Error with fstat() \n");    
        exit(0);
    }
    printf("File size: %ld Bytes \n\n", buffer.st_size);
    
    //mapping the file
    char *map = mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, value, 0);
    if(map == MAP_FAILED){
        printf("Error with mmap() \n");
        exit(0);
    }

    printf("Map created!!\n");

    int modeOn = 1;
    while(modeOn){
        
        if(semop(id, &op1, 1) < 0){
            printf("Error with semop1 with alloc");
        }
        printf("\n\nCurrently Running ALLOC.C \n\n");
        //gets the user input
        int input;
        int resType;
        int inputMode = 1;
        int notFound = 0;

        printf("What type of resource do you need: ");
        scanf("%d", &resType);
        printf("\n");

        printf("Enter the amount of resource needed: ");
        scanf("%d", &input);
        printf("\n");
        
        //edits the map
        for(int i = 0; i < buffer.st_size; i++){
            if(i % 4 == 0){
                if(map[i] == resType + 48){
                    if(map[i+2] - input >= 48){
                    map[i+2] = map[i+2] - input;
                    }
                    else{
                        printf("Error: Not enough resources. \n\n");
                    }
                }
                else{
                    notFound++;
                }
            }
            if(map[i] - 48 >= 0){
                printf("index[%d] = %d \n", i, map[i] - 48);
            }
        }
        if(notFound == buffer.st_size/4){
            printf("\n\nError: resource type not found. \n\n");
        }

        //syncs the resource file
        if(msync((void *)map,buffer.st_size, MS_SYNC) < 0){
            printf("Error with msync() \n");
            exit(0);
        }

        //exits the while loop
        if(resType == -1 && input == -1){
            modeOn = 0;
        }
        
        if(semop(id, &op2, 1) < 0){
            printf("Error with semop2 with alloc");
        }
    }
    
    printf("\n");
    printf("Done allocating resources. \n");

    err = munmap(map, buffer.st_size);

    return 0;
}