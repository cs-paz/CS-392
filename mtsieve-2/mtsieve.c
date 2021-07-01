/**
 * Name: Christian Szablewski-Paz
 * Partner: Zac Schuh
 * Pledge: I pledge my honor that I have abided by the Stevems Honor System.
 * */
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <sys/sysinfo.h>

typedef struct arg_struct {
    int start;
    int end;
} thread_args;

int total_count = 0;
pthread_mutex_t lock;

void *segsieve(void *ptr){
    thread_args* args = (thread_args*) ptr;
    
    int start = args->start;
    int end = args->end;
    int len = end - start + 1;
    int size = floor(sqrt(end)) + 1;
    bool * low = malloc(size * sizeof(bool));
    memset(low, 1, (size) * sizeof(bool));
    
    for(int i = 0; i < sqrt(size); i++) {
    	if(i < 2) {
    		low[i] = false;
    		continue;
    	}
        if(low[i] == true) {
            for(int j = pow(i, 2); j < size; j += i) {
                low[j] = false;
            }
        }
    }
    bool * high = malloc(len * sizeof(bool));
    memset(high, 1, len * sizeof(bool));
    for(int i = 0; i < size; i++) {
        if(low[i] == true){
            int x = ceil((double)start / i) * i - start;
            if(start <= i) {
                x += i;
            }
            while(x < len) {
                high[x] = false; 
                x += i;
            }
        }
    }
    free(low);
    
    int partial_count = 0;
    int numSize = 50;
    for(int i = 0; i < len; i++) {
        if(high[i] == true) {
        	char num[numSize];
            bool first = false;
            bool second = false;
            sprintf(num, "%d", i + start);
            for(int j = 0; j < strlen(num); j++) {
                if(!first && num[j] == '3') {
                    first = true;
                }
                else if(!second && num[j] == '3') {
                    second = true;
                }
            }
            if(first && second) {
                partial_count +=1;
            }
        }
    }
    free(high);
    
    int ret;
    if((ret = pthread_mutex_lock(&lock)) != 0){
        fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(ret));
    }
    total_count += partial_count;
    if((ret = pthread_mutex_unlock(&lock)) != 0){
        fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(ret));
    }
    pthread_exit(NULL);
}


bool getInt(char *input){
    long long x;
    int num;
    if (sscanf(input, "%lld", &x) == 1)
    {
        num = (int)x;
        if (x != (long long)num)
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char ** argv) {

    if(argc == 1){
        fprintf(stderr, "Usage: ./mtsieve -s <starting value> -e <ending value> -t <num threads>\n");
        return EXIT_FAILURE;
    }
    
    int option = -1;
    int start = -1;
    int end = -1;
    int num_threads = -1;
    char *endptr;
    bool sFlag = false;
    bool eFlag = false;
    bool tFlag = false;
    
    while((option = getopt(argc,argv, ":s:e:t:")) != -1){
        switch(option) {
            case 's':
                sFlag = true;
                if (!getInt(optarg)) {
                    printf("Error: Integer overflow for parameter '-%c'.\n", option);
                    return EXIT_FAILURE;
                }
                start = strtol(optarg, &endptr, 10);
                if(endptr == optarg) {
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, option);
                    return EXIT_FAILURE;
                }
                break;
            case 'e':
                eFlag = true;
                if (!getInt(optarg)) {
                    printf("Error: Integer overflow for parameter '-%c'.\n", option);
                    return EXIT_FAILURE;
                }
                end = strtol(optarg, &endptr, 10);
                if(endptr == optarg) {
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, option);
                    return EXIT_FAILURE;
                }
                break;
            case 't':
                tFlag = true;
                if (!getInt(optarg)) {
                    printf("Error: Integer overflow for parameter '-%c'.\n", option);
                    return EXIT_FAILURE;
                }
                num_threads = strtol(optarg, &endptr, 10);
                if(endptr == optarg) {
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, option);
                    return EXIT_FAILURE;
                }
                break;
            case '?':
                if (option == 'e' || option == 's' || option == 't') {
                    fprintf(stderr, "Error: Option -%c requires an argument.\n", option);
                } 
                else if (isprint(option)) {
                    fprintf(stderr, "Error: Unknown option '-%c'.\n", option);
                }
                else {
                    fprintf(stderr, "Error: Unknown option character '\\x%x'.\n",
                        option);
                }
                return EXIT_FAILURE;
        }
    }
    
    if(strcmp(argv[argc - 1] ,"-s") == 0) {
        fprintf(stderr, "Error: Option -s requires an argument.\n");
        return EXIT_FAILURE;
    }
    if(strcmp(argv[argc - 1] ,"-e") == 0) {
        fprintf(stderr, "Error: Option -e requires an argument.\n");
        return EXIT_FAILURE;
    }
    if(strcmp(argv[argc - 1] ,"-t") == 0) {
        fprintf(stderr, "Error: Option -t requires an argument.\n");
        return EXIT_FAILURE;
    }
    if(optind < argc) {
       fprintf(stderr, "Error: Non-option argument '%s' supplied.\n", argv[optind]);
       return EXIT_FAILURE;
    }
    if(!sFlag) {
        fprintf(stderr, "Error: Required argument <starting value> is missing.\n");
        return EXIT_FAILURE;
    }
    if(start < 2) {
        fprintf(stderr, "Error: Starting value must be >= 2.\n");
        return EXIT_FAILURE;
    }
    if(!eFlag) {
        fprintf(stderr, "Error: Required argument <ending value> is missing.\n");
        return EXIT_FAILURE;
    }
    if(end < 2) {
        fprintf(stderr, "Error: Ending value must be >= 2.\n");
        return EXIT_FAILURE;
    }
    if(end < start) {
        fprintf(stderr, "Error: Ending value must be >= starting value.\n");
        return EXIT_FAILURE;
    }
    if(!tFlag) {
        fprintf(stderr, "Error: Required argument <num threads> is missing.\n");
        return EXIT_FAILURE;
    }
    if(num_threads < 1) {
        fprintf(stderr, "Error: Number of threads cannot be less than 1.\n");
        return EXIT_FAILURE;
    }
    if(get_nprocs() * 2 < num_threads) {
        fprintf(stderr, "Error: Number of threads cannot exceed twice the number of processors(%d).\n", get_nprocs());
        return EXIT_FAILURE;
    }

    int retval;
    if ((retval = pthread_mutex_init(&lock, NULL)) != 0) {
        fprintf(stderr, "Error: Cannot create mutex. %s.\n", strerror(retval));
        return EXIT_FAILURE;
    }
    int range_len = end - start + 1;
    if (num_threads > range_len){

        num_threads = range_len;

    }

    pthread_t threads[num_threads];
    thread_args ranges[num_threads];
    int base_segment_len = range_len / num_threads;
    ranges[0].start = start;
    ranges[0].end = start + base_segment_len;

    for (int i = 1; i < num_threads; i++){
        ranges[i].start = start + (base_segment_len * i) + 1;
        ranges[i].end = start + (base_segment_len * (i + 1));
    }
    if (base_segment_len != ceil((float)range_len / num_threads)){
        int j = 1;
        for (; j <= (range_len - 1) % num_threads; j++){
            for (int k = j; k < num_threads - 1; k++){
                ranges[k + 1].start++;
                ranges[k].end++;
            }
        }
    }
    ranges[num_threads - 1].end = end;

    printf("Finding all prime numbers between %d and %d.\n", start, end);
    if(num_threads == 1){
        printf("%d segment:\n", num_threads);
    }else{
        printf("%d segments:\n", num_threads);
    }
    for(int i = 0; i < num_threads; i++){
        printf("   [%d, %d]\n", ranges[i].start, ranges[i].end);
    }
    
    for(int i = 0; i < num_threads; i++){
        if((retval = pthread_create(&threads[i], NULL, segsieve, &ranges[i])) != 0){
            fprintf(stderr, "Error: Cannot create thread %d. %s.\n", i+1, strerror(retval));
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Warning: Thread %d did not join properly.\n",
                    i + 1);
        }
    }
    printf("Total primes between %d and %d with two or more '3' digits: %d\n", start, end, total_count);

    return EXIT_SUCCESS;
}
