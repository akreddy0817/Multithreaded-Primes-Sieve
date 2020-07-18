/*******************************************************************************
* Name        : mtsieve.c
* Author      : Avaneesh Kolluri worked with Akhilesh Reddy.
* Date        : 4/17/20
* Description : hw 6
* Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
******************************************************************************/

#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <wait.h>
#include <setjmp.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <math.h>



typedef struct arg_struct {     
    int start;     
    int end; 
} thread_args; 


int total_count = 0;
pthread_mutex_t lock;


void usage(){
    printf("Usage: ./mtsieve -s <starting value> -e <ending value> -t <num threads>\n");
}



void *sieve(void *ptr) {
    int thread_count = 0;

   
    thread_args *newptr = (thread_args *)ptr;
    int a = (*newptr).start;
    int b = (*newptr).end;

    //printf("BEGIN THREAD : [%d,%d]\n",a,b);


    int limit = (int) sqrt(b);

    bool lowprime[limit + 1];
    memset(lowprime, true, sizeof(lowprime));


    int len_lowprime = ((int)(sqrt(b))) - 1;
    int low_primes[len_lowprime];
    //b is not inclusive
    //first use standard sieve to find all primes up to sqrt(b), this will be called low_primes[]
    

    for (int i = 2; i<= (int)sqrt(limit) ; i++) 
    { 
        if (lowprime[i] == true) 
        { 
            for (int j = i*i; j <= limit; j += i) {
                lowprime[j] = false; 
            }
        } 
    }


    for (int i = 2; i <= limit; i++){
        // printf("i:%d\n", i);
        if (lowprime[i]){
            low_primes[i-2] = i;
        }
        else{
            low_primes[i-2] = 0;
        }
    } 

    // for (int i = 0; i < ((int)(sqrt(b))) - 1; i++){
    //     printf("Low prime[%d]: %d \n", i, low_primes[i]);
    // }
    //printf("hi\n");
    bool *high_primes;
    if((high_primes = (bool *)malloc((b-a+2) * sizeof(bool ))) == NULL){
        fprintf(stderr, "Error Malloc Failed. %s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < b-a+1; i ++){
         high_primes[i] = true;
    }


    for (int index = 0; index < len_lowprime; index++){
        if (low_primes[index] != 0){
            int p = low_primes[index];
            int i = ceil((double) a / p ) * p - a;
            if (a <= p){
                i = i + p;
            }

            for (int x = i; x <= (b-a+1); x += p){
                high_primes[x] = false;
                
            }
        }
    }

    int tNum = 0;

    for (int i = 0; i < (b-a+1); i++){
        if (high_primes[i]){
            int local_count = 0;

            tNum = a+i;
            int rem = 0;
            while(tNum>0) {
                rem = tNum % 10;
                if(rem==3){
                    local_count++;
                }
                tNum/=10;
            }

             if (local_count >= 2){
                 thread_count++;
             }
            

            //printf("Prime: %d\n", i + a);
            
        }

    }
    free(high_primes);

    //printf("END OF THREAD! LOCAL COUNT: %d\n",thread_count);


    int retval;
    if((retval = pthread_mutex_lock(&lock)) != 0){
        fprintf(stderr, "Warning: cannot lock mutex. %s.\n", strerror(retval));
    }
    
    total_count += thread_count;
    
    if((retval = pthread_mutex_unlock(&lock)) != 0){
        fprintf(stderr, "Warning: cannot unlock mutex. %s.\n", strerror(retval));
    }

    //printf("Total Count: %d\n",total_count);
    pthread_exit(NULL);
        
}

bool is_overflow(int num){
    if(num < 0){
        return true;
    }
    int tNum = num;
    int rem = 0;
    while(tNum>0) {
        rem = tNum % 10;
        rem++;
        tNum/=10;
    }
    if(rem>INT_MAX){
        return true;
    }
    return false;
}


int main(int argc, char *argv[]) {
    
    int opt;
   
    bool s = false;
    bool e = false;
    bool t = false;

    int start = 0;
    int end = 0;
    int num_threads = 0;
    opterr = 0;

    while ((opt = getopt(argc, argv, "s:e:t:")) != -1){
        switch (opt) {
            case 's':
                s = true;
                // printf("S OPT: '-%c' \n",opt);
                // printf("S ARG: %s\n",optarg);
            
                bool dig = true;
                for (int i = 0; i < strlen(optarg); i++){
                    if(!isdigit(optarg[i])){
                        dig = false;
                    }
                }
                if (dig){
                    start = atoi(optarg);
                }
                else {
                    fprintf(stderr,"Error: Invalid input '%s' received for parameter '-%c'.\n",optarg,opt);
                    return EXIT_FAILURE;
                }                    
                break;
            case 'e':
                e = true;
                bool dig1 = true;
                // printf("E OPT: '-%c' \n",opt);
                // printf("E ARG: %s\n",optarg);
                for (int i = 0; i < strlen(optarg); i++){
                    if(!isdigit(optarg[i])){
                        dig1 = false;
                    }
                }
                if (dig1){
                    end = atoi(optarg);
                }
                else {
                    fprintf(stderr,"Error: Invalid input '%s' received for parameter '-%c'.\n",optarg,opt);
                    return EXIT_FAILURE;
                }
                break;

            case 't':
                t = true;
                bool dig2 = true;
                // printf("E OPT: '-%c' \n",opt);
                // printf("E ARG: %s\n",optarg);
                for (int i = 0; i < strlen(optarg); i++){
                    if(!isdigit(optarg[i])){
                        dig2 = false;
                    }
                }
                if (dig2){
                    num_threads = atoi(optarg);
                }
                else {
                    fprintf(stderr,"Error: Invalid input '%s' received for parameter '-%c'.\n",optarg,opt);
                    return EXIT_FAILURE;
                }
                break;
            case '?':
                if (optopt == 'e' || optopt == 's' || optopt == 't') {         
                    fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt);
                } 
                else if (isprint(optopt)) {      
                    fprintf(stderr, "Error: Unknown option '-%c'.\n", optopt);     
                } 
                else {         
                    fprintf(stderr, "Error: Unknown option character '\\x%x'.\n", optopt);     
                }    
                return EXIT_FAILURE;
            }
        }


    if (argc == 1){
        usage();
        return EXIT_FAILURE;
    }
    
    if(is_overflow(start)){
        fprintf(stderr, "Error: Integer overflow for parameter '-s'.\n");
        return EXIT_FAILURE;
    }
    
    if(is_overflow(end)){
        fprintf(stderr, "Error: Integer overflow for parameter '-e'.\n");
        return EXIT_FAILURE;
    }
    
    if(is_overflow(num_threads)){
        fprintf(stderr, "Error: Integer overflow for parameter '-t'.\n");
        return EXIT_FAILURE;
    }
    
    if (optind < argc){
        fprintf(stderr, "Error: Non-option argument '%s' supplied.\n",argv[optind]); 
        return EXIT_FAILURE;
    }

    if(!s){
        fprintf(stderr, "Error: Required argument <starting value> is missing.\n"); 
        return EXIT_FAILURE;
    }
    if (start < 2){
        fprintf(stderr, "Error: Starting value must be >= 2.\n");
        return EXIT_FAILURE;
    }
    if(!e){
        fprintf(stderr, "Error: Required argument <ending value> is missing.\n"); 
        return EXIT_FAILURE;
    }
    if (end < 2){
        fprintf(stderr, "Error: Ending value must be >= 2.\n");
        return EXIT_FAILURE;
    }
    if (start > end){
        fprintf(stderr,"Error: Ending value must be >= starting value.\n");
        return EXIT_FAILURE;
    }
    if(!t){
        fprintf(stderr, "Error: Required argument <num threads> is missing.\n"); 
        return EXIT_FAILURE;
    }
    if (num_threads < 1){
        fprintf(stderr,"Error: Number of threads cannot be less than 1.\n");
        return EXIT_FAILURE;
    }

    int num_processers = get_nprocs();
    if ((2*num_processers) < num_threads){
        fprintf(stderr,"Error: Number of threads cannot exceed twice the number of processors(%d).\n",num_processers);
        return EXIT_FAILURE;
    }
    
    int final_start = start;
    int numvalues = end - start + 1;
   
    if (num_threads > numvalues){
        num_threads = numvalues;
    }



    //match the number of threads to numnber of values 
   
    int retval;
    if ((retval = pthread_mutex_init(&lock, NULL)) != 0) {
        fprintf(stderr, "Error: Cannot create mutex. %s.\n", strerror(retval));
        return EXIT_FAILURE;
    }


    pthread_t threads[num_threads];
    thread_args targs[num_threads];

    printf("Finding all prime numbers between %d and %d.\n", final_start,end);
    

    if (num_threads == 1){
        printf("%d segment:\n",num_threads);
    }
    else if (num_threads > 1){
        printf("%d segments:\n",num_threads);
    }
    
    int num_per_thread = (numvalues / num_threads) - 1;
    int rem = numvalues % num_threads;

    for (int i = 0; i < num_threads; i++){

        targs[i].start = start;

        if (rem != 0){
            start++;
            rem--;
        }
       
        start += num_per_thread;

        targs[i].end = start;
        start++;

        if (i == num_threads - 1){
            targs[i].end = end;
        }
    
        printf("   [%d, %d]\n",targs[i].start,targs[i].end);
    }

    int retval1;
    for(int i = 0; i<num_threads; i++){
        
        if((retval1 = pthread_create(&threads[i], NULL, sieve, &targs[i]))!=0){
            fprintf(stderr, "Error: Cannot create thread %d. %s.\n",i+1,strerror(retval1));
            return EXIT_FAILURE;
        }
    }
    //printf("Here\n");
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Warning: Thread %d did not join properly.\n", i + 1);
        }
    }

    if ((retval = pthread_mutex_destroy(&lock)) != 0) {
        fprintf(stderr, "Error: Cannot destroy mutex. %s.\n", strerror(retval));
        
    }

    printf("Total primes between %d and %d with two or more '3' digits: %d\n",final_start,end,total_count);
    return EXIT_SUCCESS;

}
