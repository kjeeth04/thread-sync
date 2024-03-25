#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <pthread.h>
#include "helpers.h"

int main(int argc, char *argv[]){    

    //Creating pipe for communication between Producer Processes and Consumer Process
    int mypipe[2];
    pipe(mypipe);

    //Creating Producer and Consumer Proccesses
    int pid0 = fork();
    //In case forking fails+
    if (pid0 == -1){
        perror("Process Creation Failed. Exiting Program");
        exit(1);
    }
    
    //Contains both Producer Processes
    else if (pid0 == 0){
        //Redirect STDOUT to write to end of pipe, and close pipe ends for both Producer Processes
        dup2(mypipe[1], 1);
        close(mypipe[1]);
        close(mypipe[0]);
        //Creating Producer Process 1 and Producer Process 2
        int pid1 = fork();

        //In case forking fails
        if (pid1 == -1){
            perror("Process Creation Failed. Exiting Program");
            exit(1);  
        }

        //Producer Process 1
        else if (pid1 == 0){
            //Production loop is ran inside producer executable
            char* arg_list[3] = {"./producer", "1", NULL};
            execv(arg_list[0], arg_list);
        }

        //Producer Process 2
        else {
            //Ensures that multiple processes are able to run simultaneously
            int wstatus;
            waitpid(-1, &wstatus, WNOHANG);
            
            //Production loop is ran inside producer executable
            char* arg_list[3] = {"./producer", "2", NULL};
            execv(arg_list[0], arg_list);
        }
    }

    //Consumer Process
    else {
        //Ensures that multiple processes are able to run simultaneously
        int wstatus;
        waitpid(-1, &wstatus, WNOHANG);

        //Redirect STDIN to read from end of pipe, and close pipe ends
        dup2(mypipe[0], 0);
        close(mypipe[0]);
        close(mypipe[1]);

        //Distributor and Worker Threads are ran inside consumer executable
        char* arg_list[2] = {"./consumer", NULL};
        execv(arg_list[0], arg_list);
    }
}