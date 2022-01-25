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

typedef void*( *pointerToTests)(void*);
// pointerToFunction jest wskaźnikiem do funkcji przyjmującej jako argument void*
// i zwracającej void*

void* testFunction(void* arguments);
void* testFunctionOut(void* arguments);
void* testFunctionIn(void* arguments);
void differentThanZero(double* testTab);
void sequentialTest(pointerToTests function, data* myData);
void concurrentTest(pointerToTests function, data* myData);
void concurrentTestMixed(pointerToTests functionOne, pointerToTests functionTwo, data* myData);

pthread_mutex_t workMutex; // tworze dwa bo potrzebuje je do miks-testu

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

    //Preparing data and mutexes for threads
    double* testTab = (double*)calloc(SIZE,sizeof(double));
    data myData[2] = {0};
    for( int i=0; i<2; ++i ) {
        myData[i].numberOfIterations = numberOfIteration;
        myData[i].testTab = testTab;
    }
    myData[0].delta = DELTA;
    myData[1].delta = -DELTA;

    int res = 0;
    res = pthread_mutex_init(&workMutex, NULL);
    if( res != 0 ) {
        perror("Mutex initialization failed\n");
        exit(EXIT_FAILURE);
    }
    //--------------------------

    // Preparing data for time
    clock_t start, end;
    double cpu_time_used;
    //-------------------------

    //Sequential test
    printf("Sequential test without mutexes!\n");
    start = clock();
    sequentialTest(testFunction,myData);
    end = clock();
    //-----------------------

    // Check results and zero table
    differentThanZero(testTab);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

    //Concurrent test
    start = clock();
    printf("Concurrent test without mutexes!\n");
    concurrentTest(testFunction,myData);
    end = clock();
    //----------------

    // Check results and zero table
    differentThanZero(testTab);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

    //Sequential test with mutexes out of loop
    printf("Sequential test with mutexes out of loop!\n");
    start = clock();
    sequentialTest(testFunctionOut,myData);
    end = clock();
    //-----------------------

    // Check results and zero table
    differentThanZero(testTab);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

    //Concurrent test with mutexes out of loop
    start = clock();
    printf("Concurrent test with mutexes out of loop!\n");
    concurrentTest(testFunctionOut,myData);
    end = clock();
    //----------------

    // Check results and zero table
    differentThanZero(testTab);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

    //Sequential test with mutexes in loop
    printf("Sequential test with mutexes in loop!\n");
    start = clock();
    sequentialTest(testFunctionIn,myData);
    end = clock();
    //-----------------------

    // Check results and zero table
    differentThanZero(testTab);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

    //Concurrent test with mutexes in loop
    start = clock();
    printf("Concurrent test with mutexes in loop!\n");
    concurrentTest(testFunctionIn,myData);
    end = clock();
    //----------------

    // Check results and zero table
    differentThanZero(testTab);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

    //Concurrent test mixed
    start = clock();
    printf("Concurrent test mixed!\n");
    concurrentTestMixed(testFunctionIn,testFunctionOut,myData);
    end = clock();
    //----------------

    // Check results and zero table
    differentThanZero(testTab);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

    //Concurrent test mixed
    start = clock();
    printf("Concurrent test mixed!\n");
    concurrentTestMixed(testFunctionOut,testFunctionIn,myData);
    end = clock();
    //----------------

    // Check results and zero table
    differentThanZero(testTab);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------


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

void sequentialTest(pointerToTests function, data* myData) {

    int result = 0;
    pthread_t aThread[2] = {0};
    void* threadResult[2] = {0}; // nie wiem czy to nam jest potrzebne chyba nie
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


void concurrentTest(pointerToTests function, data* myData) {

    int result = 0;
    pthread_t aThread[2] = {0};
    void* threadResult[2] = {0}; // nie wiem czy to nam jest potrzebne chyba nie
    //Starting threads and waiting for them
    for( int i=0; i<2; ++i ) {
        result = pthread_create(&aThread[i], NULL, function, &myData[i]);
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

void concurrentTestMixed(pointerToTests functionOne, pointerToTests functionTwo, data* myData) {
    int result = 0;
    pthread_t aThread[2] = {0};
    void* threadResult[2] = {0}; // nie wiem czy to nam jest potrzebne chyba nie
    //Starting threads and waiting for them
    result = pthread_create(&aThread[0], NULL, functionOne, &myData[0]);
    if( result!= 0 ) {
        perror("Thread creation failed!\n");
        exit(EXIT_FAILURE);
    }
    result = pthread_create(&aThread[1], NULL, functionTwo, &myData[1]);
    if( result!= 0 ) {
        perror("Thread creation failed!\n");
        exit(EXIT_FAILURE);
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

void* testFunctionOut(void* arguments) {
    data* helper = (data*)arguments;

    for( int i=0; i<helper->numberOfIterations; ++i ) {
        pthread_mutex_lock(&workMutex);
        for( int j=0; j<SIZE; ++j )
            helper->testTab[j] += helper->delta;
        pthread_mutex_unlock(&workMutex);
    }
    pthread_exit(0);
}

void* testFunctionIn(void* arguments) {
    data* helper = (data*)arguments;
    for( int i=0; i<helper->numberOfIterations; ++i ) {
        for( int j=0; j<SIZE; ++j ) {
            pthread_mutex_lock(&workMutex);
            helper->testTab[j] += helper->delta;
            pthread_mutex_unlock(&workMutex);
        }
    }
    pthread_exit(0);
}
