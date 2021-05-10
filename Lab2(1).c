#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    int fdCTP[2], fdPTC[2];
    int nbytes = 0;
    pid_t childpid;
    char in[2];
    char readbuffer[2];
    char sumbuffer[2];
    int sum = 0;
    int i = 0;
    char string[10];

    pipe(fdCTP);
    pipe(fdPTC);

    if((childpid = fork()) == -1){
        perror("fork");
        exit(0);
    }

    if(childpid == 0){ //child process
        printf("child process *************\n");

        while(strcmp(in, "-1") != 0){ //gets input until "-1" is entered
            printf("Enter input: ");
            scanf("%s", in);                                                                                                                                                                    
            
            //printf("Your input: %s \n", in);
            write(fdCTP[1], in, (strlen(in) + 1)); //writes to the parent
        }

        close(fdCTP[1]);
        //printf("leaving child process\n");
        
        wait(NULL); //waiting for the parent to send something back

        close(fdPTC[1]);
        
        nbytes = read(fdPTC[0], sumbuffer, sizeof(sumbuffer)); //gets the sum from the parent
        printf("Sum of inputs = %s\n", sumbuffer);
        close(fdPTC[0]);
    }
    else{ //parent process
        //wait(NULL);

        printf("parent process *************\n");
        printf("Getting inputs....\n");
        
        nbytes = read(fdCTP[0], readbuffer, sizeof(readbuffer)); //initialize readbuffer
        while(strcmp(readbuffer, "-1") != 0){ //loops reading from pipeline
            //printf("Read value: %s \n", readbuffer);
            //printf("Byte size: %d \n", nbytes);
            sum = sum + atoi(readbuffer); //adds the pipeline value
            nbytes = read(fdCTP[0], readbuffer, sizeof(readbuffer)); //reading from child
        }

        printf("Done calculating.... \n");
        close(fdCTP[0]); 

        sprintf(string,"%d",sum); //integer to string
        write(fdPTC[1], string, sizeof(int)); //writing to the child process
        close(fdPTC[1]);
        exit(0);
    }
}