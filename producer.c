#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <time.h>
#include "helpers.h"

//  Loop in which products are created. Returns exit status
int main(int argc, char *argv[]){
    //Identifies which Producer Process the product came from
    int productNumber = atoi(argv[1]);
    //Setting Random Seed
    srand(time(0)); 
    for (int i = 0; i < 150; ++i){
        int data[2] = {i, productNumber};
        write(1, data, sizeof(data));

        //Sleep between 0.01 and 0.2 seconds before next iteration
        double time = (rand() % (200000000 - 10000000 + 1)) + 10000000; 
        struct timespec remaining, request = {0, time}; 
        nanosleep(&request, &remaining);
    }

    //Sending signal to Consumer Process that this process is complete
    int final[2] = {-1, productNumber};
    write(1, final, sizeof(final));

    exit(0);
}