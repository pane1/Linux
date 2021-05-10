#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

// user-defined signal handler for alarm
void alarm_handler(int signo){
    printf("Alarm \n");
}

void input_handler(int signo){
    printf("CTRL + C is pressed! \n");
}

void stop_handler(int signo){
    printf("CTRL + Z is pressed! \n");
    exit(0);
}

int main(void){

    signal(SIGALRM, alarm_handler);
    signal(SIGINT, input_handler);
    signal(SIGTSTP, stop_handler);
    
    while(1){    //set alarm to fire in 3 seconds
        sleep(2);   //sleeps for 2 seconds before alarm
        raise(SIGALRM); 
        printf("%d\n",getpid());
    }
}