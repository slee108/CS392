/*******************************************************************************
 * Name        : mtsieve.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 06/24/2020
 * Description : mtsieve
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <time.h>

//GLOBALS
int total_count = 0; 
pthread_mutex_t lock;

typedef struct arg_struct {
    int start;
    int end;
} thread_args;


//RESOURCE:
//https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;


/* Convert string s to int out.
 * RESOURCE:
 * https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
 */
str2int_errno str2int(int *out, char *s, int base) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}

//Returns an int * array we can utilize that has the primes of
//sqrt(b) according to spec aka lowPrimes up to max limit
int *findLowPrimes(int * lowPrimes, int max){
    bool * primes;
    //init array
    if ( (primes = (bool *)malloc(sizeof(bool) * (1 + max))) == NULL){
        fprintf(stderr, "Malloc failed in findLowPrimes.\n");
        //we will want to make sure other funcs check if we return null
        return NULL;
    }

    //populate array with all TRUEs
    int pntr = 2;
    while(pntr <= max){
        primes[pntr] = true;
        pntr += 1;
    }

    pntr = 2; //reset
    while(pntr <= (int)sqrt(max)){

        if(primes[pntr] == true){
            for (int i = pntr * pntr; i <= max; i += pntr){
                //mark it false
                primes[i] = false;
            }
        }
        pntr += 1;
    }

    int totalFoundPrimes = 0;
    int highestAmountPrime = 2; //if nothing we'd at least have 2

    //lets count how many trues we have left
    pntr = 2; //reset 
    while (pntr <= max){

        if(primes[pntr] == true){
            //there is a prime! 
            //increase the count
            highestAmountPrime = pntr;
            totalFoundPrimes += 1;
        }

        pntr += 1;
    }

    int *final;
    if ( (final = (int *)malloc(totalFoundPrimes * sizeof(int))) == NULL){
        fprintf(stderr, "Malloc failed in findLowPrimes.\n");
        //we will want to make sure other funcs check if we return null
        return NULL;
    }

    int startIndex = 2;
    int primeIndex = 0;

    while(startIndex <= highestAmountPrime){
        
        if(primes[startIndex] == true){
            final[primeIndex++] = startIndex;
        }

        startIndex += 1;
    }

    * lowPrimes = totalFoundPrimes;
    free(primes);
    return final;

}
// counts the num of threes inside an integer 

int counting_threes(long int num){
    int counter = 0;
    long int one_digit_down = num/10;
    long int compare = num;

    while(compare != 0){
        if((compare - one_digit_down*10) == 3){
            counter++;
        }
        compare = one_digit_down;
        one_digit_down /= 10;
    }
    return counter;
}

// This is the func that he describes in the spec with ceil etc.
// It will have to make a call upon findLowPrimes 
int sieveSeg(int a, int b){


    int primes = 0;

    // Step 1: Perform Low Primes called low_primes per spec
    int lowPrimeVal;
    int *low_primes = findLowPrimes(&lowPrimeVal, floor(sqrt(b))+1);

    // Step 2: Perform High Primes called high_primes per spec
    int lengthForHighPrimes = b - a + 1;
    bool * high_primes = (bool *)malloc((lengthForHighPrimes * sizeof(bool)));
    for(int i = 0; i < lengthForHighPrimes; i++){
        //Initalize each element to true
        high_primes[i] = true;
    }

    // Step 3: The for loop
    int p = 0;  // prime p
    int i = 0;  // set i (shown in spec)
    for(int c = 0; c < lowPrimeVal; c++){
        p = low_primes[c];  // grab the prime from the low prime array
        i = ceil((double)a/p) * p - a;  // spec 
        if (a <= p){
            i = i + p;
        }
        // Starting at i, cross off all multiples of p in high_primes
        while(i < lengthForHighPrimes){
            high_primes[i] = false; //cross off
            i = i + p; //increase the count
        }
    }

    // Step 4: For each high_primes[i] that is true
     for(int i = 0; i < lengthForHighPrimes; i++){
       if (high_primes[i] == true){ 
           //THIS IS WHERE WE ARE COUNTING PRIMES
            if(counting_threes(i + a) >= 2){
                primes+=1;
            }           
        }
    }

    free(low_primes);
    free(high_primes);
    return primes;
}

//From LAB 10
void *sum_array(void *ptr) {
    //init
    thread_args * tArgs = (thread_args *)ptr;
    int partial_sum = sieveSeg(tArgs->start, tArgs->end); 

    //loop over with one thread
    // for (int indexOneThread = tArgs->start_index; indexOneThread < tArgs->length; indexOneThread += 2){
    //     partial_sum = partial_sum + array[indexOneThread];
    // }

    int lockVal;
    if ( (lockVal = (pthread_mutex_lock(&lock)) ) != 0){
        fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(lockVal));
    }
    ////////////////CRIT SECTION
    //WE ARE LOCKED
    total_count = partial_sum + total_count;
    ///////////////
    if ( (lockVal = (pthread_mutex_unlock(&lock)) ) != 0){
        fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(lockVal));
    }
    //WE ARE UNLOCKED

    pthread_exit(NULL); //leave
}

//Main runner
int main(int argc, char *argv[]) {

    //if the input was less than = 1, print usage
    if (argc <= 1){
        fprintf(stderr, "Usage: %s -s <starting value> -e <ending value> -t <num threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //for our getopts
    int inputThreadNum = 0;
    int inputStartNum = 0;
    int inputEndNum = 0;
    bool threadHit = false;
    bool startHit = false;
    bool endHit = false;
    char input;

    opterr = 0;

    while ((input = getopt(argc, argv, ":s:e:t:")) != -1){

        //Grabbing what the input number is 
        int inputNum;

        //Special cases 
        if (input != '?'){
            if (optarg != NULL){

                //10 is passed in as base 10
                str2int_errno convResult = str2int(&inputNum, optarg, 10);

                if (convResult != STR2INT_SUCCESS){
                    //go through the cases to why it failed:
                    if (convResult == STR2INT_INCONVERTIBLE){

                        //not an int
                        fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, input);
                        return EXIT_FAILURE;

                    } else if (convResult == STR2INT_OVERFLOW){

                        //overflow
                        fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", input);
                        return EXIT_FAILURE;

                    } else if (convResult == STR2INT_UNDERFLOW){

                        //underflow
                        inputNum = INT_MIN;

                    }
                }
            } else {
                //otherwise we hit a different type of case
                input = '?';
            }
        }

        //Normal GETOPTS
        switch (input){
            case 's':
                startHit = true;
                inputStartNum = inputNum;
                break;
            case 'e':
                endHit = true;
                inputEndNum = inputNum;
                break;
            case 't':
                threadHit = true;
                inputThreadNum = inputNum;
                break;
            case '?':
                if (optopt == 'e' || optopt == 's' || optopt == 't') {
                    fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt); 
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Error: Unknown option '-%c'.\n", optopt); 
                } else {
                    fprintf(stderr, "Error: Unknown option character '\\x%x'.\n", optopt);
                }
                return EXIT_FAILURE;
            default:
                break;
        }
    }

    if(optind < argc){
        fprintf(stderr, "Error: Non-option argument '%s' supplied.\n", argv[optind++]);
        return EXIT_FAILURE;
    }
    //ALL SCENARIOS ACCORDING TO SPEC
    int usersProcessorCount = get_nprocs();
    int highestNumThread = (usersProcessorCount * 2);

    if (startHit == false){
        fprintf(stderr, "Error: Required argument <starting value> is missing.\n");
        return EXIT_FAILURE;
    }

    if (inputStartNum < 2){
        fprintf(stderr, "Error: Starting value must be >= 2.\n");
        return EXIT_FAILURE;
    }

    if (endHit == false){
        fprintf(stderr, "Error: Required argument <ending value> is missing.\n");
        return EXIT_FAILURE;
    }

    if (inputEndNum < 2){
        fprintf(stderr, "Error: Ending value must be >= 2.\n");
        return EXIT_FAILURE;
    }

    if (inputEndNum < inputStartNum){
        fprintf(stderr, "Error: Ending value must be >= starting value.\n");
        return EXIT_FAILURE;
    }

    if (threadHit == false){
        fprintf(stderr, "Error: Required argument <num threads> is missing.\n");
        return EXIT_FAILURE;
    }

    if (inputThreadNum < 1){
        fprintf(stderr, "Error: Number of threads cannot be less than 1.\n");
        return EXIT_FAILURE;
    }

    if (inputThreadNum > highestNumThread){
        fprintf(stderr, "Error: Number of threads cannot exceed twice the number of processors(%d).\n", usersProcessorCount);
        //do we want to print their processors or threads?
        return EXIT_FAILURE;
    }

    //FINALLY WE CAN GO:

    /**
     * your program needs to decide how to segment the range of values. 
     * First, compute how many numbers are being tested for primality. 
     * If the number of threads exceeds the count, reduce the number of threads to match the count.
     */
    int primeCount = inputEndNum - inputStartNum + 1;
    if (inputThreadNum > primeCount) {
        inputThreadNum = primeCount;
    }

    printf("Finding all prime numbers between %d and %d.\n", inputStartNum, inputEndNum);

    //Plural or non plural s for the string
    char * pluralS;
    if (inputThreadNum == 1){
        pluralS = "";
    } else {
        pluralS = "s";
    }
    printf("%d segment%s:\n", inputThreadNum, pluralS);


    /**
     * Otherwise, take the count and divide it by the number of threads the user requested to create. 
     * Each thread will process at least that many numbers. Take the remainder and distribute it among all the threads. 
     * Unless the thread number divides the count evenly, the later segments will contain less values.
     */ 
    int primeForEachSeg = primeCount / inputThreadNum;
    int primesRemain = primeCount % inputThreadNum;
    

    //create our threads
    thread_args targs[inputThreadNum];
    pthread_t thread[inputThreadNum];

    int a = 0;
    int b = 0;
    int lastBVal = 0;
    for(int i = 0; i < inputThreadNum; i++){

        if(i != 0){
            a = 1 + lastBVal;
        } else {
            a = inputStartNum;
        }

        if(i != inputThreadNum - 1){
            b = primeForEachSeg + a - 1;
        } else {
            b = inputEndNum;
        }

        if (i < primesRemain){
            b += 1;
        }
       
        printf("   [%d, %d]\n", a, b);

        lastBVal = b;

        //from lab 10
        //IS THIS CAUSING THE SAME VALGRIND ISSUE AS THE LAB :(
        targs[i].start = a;
        targs[i].end = b;
        int threadVal;
        if( (threadVal = pthread_create(&thread[i], NULL, sum_array, (void*)(&targs[i]))) != 0){
            fprintf(stderr, "Error: Cannot create thread %d. %s.\n", 1 + i, strerror(errno));
            return EXIT_FAILURE; //GET OUT
        }

    }

    //From LAB 10 CLEANUP
    for (int i = 0; i < inputThreadNum; i++) {
        if (pthread_join(thread[i], NULL) != 0) {
            fprintf(stderr, "Warning: Thread %d did not join properly.\n", i + 1);
        }
    }
    int retval;
    if ((retval = pthread_mutex_destroy(&lock)) != 0) {
        fprintf(stderr, "Error: Cannot destroy mutex. %s.\n", strerror(retval));
    }

    printf("Total primes between %d and %d with two or more '3' digits: %d\n", inputStartNum, inputEndNum, total_count);

    return EXIT_SUCCESS;

}