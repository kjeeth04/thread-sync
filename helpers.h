#ifndef helpers_h
#define helpers_h

/*  Struct for circular queue
        @param buffer : where the elements are stored
        @param size : size of buffer
        @param filled : amount of positions filled
        @param head : "Front" of queue
        @param tail : "Back" of queue
        @param consumed : counter for the amount of product consumed
        @param isActive : controls worker loop
        @param output : output file to write to
        @param mutex : mutex lock for shared memory
        @param fill : condition variable to signal that there is a product in the buffer
        @param empty : condition variable to signal that there are no products in the buffer
*/
typedef struct myarg_buff {
        int* buffer;
        int size;
        int filled;
        int head;
        int tail;
        int consumed;
        int isActive;
        FILE* output;
        pthread_mutex_t mutex;
        pthread_cond_t fill;
        pthread_cond_t empty;

 } myarg_buff;

/*  Struct for passing arguments into threaded function
        @param buffer1 : buffer used to store products from Producer Process 1
        @param buffer2 : buffer used to store products from Producer Process 2
*/
typedef struct myarg_dist {
        myarg_buff* buffer1;
        myarg_buff* buffer2;

 } myarg_dist;

void producer(int productNumber, int mypipe[2]);

void *distributor(void *arg);

void *worker(void *arg);

void put(int* buffer, int val, int* size, int* head, int* filled);

void get(int* buffer, int* tail, int* size, int* filled, int* consumed, int* isActive, pthread_cond_t* empty, FILE* output);

#endif /* helpers_h */