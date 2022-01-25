#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>
#include <time.h>

#ifndef SIZE
#define SIZE 2048
#endif

#ifndef DELTA
#define DELTA 0.0625
#endif

typedef struct dataToThread {
    double* testTab;
    double delta;
    unsigned long numberOfIterations;
} data;


typedef void*( *pointerToFunctions)(void*);
typedef void (*pointerToTest)(pointerToFunctions,data*);

typedef struct testThread {
    pointerToTest pT;
    pointerToFunctions pF;
} test;

void* testFunction(void* arguments);
void differentThanZero(double* testTab);
void sequentialTest(pointerToFunctions function, data* myData);
void concurrentTest(pointerToFunctions function, data* myData);

int main(int argc, char* argv[]) {

    // Checking input
    if( argc!= 2 ) {
        perror("Unknown number of iterations!\n");
        exit(EXIT_FAILURE);
    }
    char* endPointer = NULL;
    unsigned long numberOfIteration = strtoul(argv[1],&endPointer,10);
    if( !(*(argv[1]) != '\0' && *endPointer == '\0') ) {
        perror("Wrong input!\n");
        exit(EXIT_FAILURE);
    }
    //-------------------

    //Preparing data for threads
    double* testTab = (double*)calloc(SIZE,sizeof(double));
    data myData[2] = {0};
    for( int i=0; i<2; ++i ) {
        myData[i].numberOfIterations = numberOfIteration;
        myData[i].testTab = testTab;
    }
    myData[0].delta = DELTA;
    myData[1].delta = -DELTA;
    //--------------------------

    // Preparing data for time
    clock_t start, end;
    double cpu_time_used;
    //-------------------------
    test testStructure[2] = { {sequentialTest,testFunction}, {concurrentTest,testFunction}};

    for( int i=0; i<2; ++i ) {
        start = clock();
        testStructure[i].pT(testStructure[i].pF,myData);
        end = clock();
        // Check results and zero table
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        differentThanZero(testTab);
        printf("Time used in sequential test: %f\n", cpu_time_used );
        memset( (double*)testTab, 0, sizeof(double)*SIZE);
        //--------------
    }
    // zostawie dwie wersje bo skoro kod jest częściej czytany to chyba ta druga wersja jest lepsza
    // do czytania w sensie bardziej zrozumiała
    /*
    //Sequential test
    start = clock();
    sequentialTest(testFunction,myData);
    end = clock();
    //-----------------------

    // Check results and zero table
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    differentThanZero(testTab);
    printf("Time used in sequential test: %f\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

    //Concurrent test
    start = clock();
    concurrentTest(testFunction,myData);
    end = clock();
    //----------------

    // Check results and zero table
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    differentThanZero(testTab);
    printf("Time used in sequential test: %f\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

*/
    free(testTab);


    return 0;
}


void* testFunction(void* arguments) {
    data* helper = (data*)arguments;
    for( int i=0; i<helper->numberOfIterations; ++i ) {
        for( int j=0; j<SIZE; ++j )
            helper->testTab[j] += helper->delta;
    }
    pthread_exit(0);
}

void differentThanZero(double* testTab) {
    int count = 0;
    for( int i=0; i<SIZE; ++i ) {
        if( fabs(testTab[i]) >= DELTA ) {
            count++;
        }
    }
    printf("Numbers different than zero: %d\n",count);
}

void sequentialTest(pointerToFunctions function, data* myData) {

    int result = 0;
    pthread_t aThread[2] = {0};
    void* threadResult[2] = {0}; 
    printf("Sequential test!\n");
    // First thread
    result = pthread_create(&aThread[0], NULL, function, &myData[0]);
    if( result!= 0 ) {
        perror("Thread creation failed!\n");
        exit(EXIT_FAILURE);
    }
    result = pthread_join(aThread[0], &threadResult[0]);
    if (result != 0) {
        perror("Thread join failed!\n");
        exit(EXIT_FAILURE);
    }
    //---------------------------------

    // Second thread
    result = pthread_create(&aThread[1], NULL, function, &myData[1]);
    if( result!= 0 ) {
        perror("Thread creation failed!\n");
        exit(EXIT_FAILURE);
    }
    result = pthread_join(aThread[1], &threadResult[1]);
    if (result != 0) {
        perror("Thread join failed!\n");
        exit(EXIT_FAILURE);
    }
    //----------------------------------

}


void concurrentTest(pointerToFunctions function, data* myData) {

    int result = 0;
    pthread_t aThread[2] = {0};
    void* threadResult[2] = {0};  
    printf("Concurrent test!\n");
    //Starting threads and waiting for them
    for( int i=0; i<2; ++i ) {
        result = pthread_create(&aThread[i], NULL, testFunction, &myData[i]);
        if( result!= 0 ) {
            perror("Thread creation failed!\n");
            exit(EXIT_FAILURE);
        }
    }

    for( int i=0; i<2; ++i ) {
        result = pthread_join(aThread[i], &threadResult[i]);
        if (result != 0) {
            perror("Thread join failed!\n");
            exit(EXIT_FAILURE);
        }
    }
    //--------------------------------------
}
