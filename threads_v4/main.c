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


typedef void*( *pointerToFunction)(void*);


typedef struct dataToThread {
    double* testTab;
    double delta;
    unsigned long numberOfIterations;
} data;

typedef void(*pointerToTest)(pointerToFunction,data*);
typedef void(*pointerToMixedTest)(pointerToFunction,pointerToFunction,data*);

typedef struct testThread {
    pointerToMixedTest pT;
    pointerToFunction firstFunction;
    pointerToFunction secondFunction;
} test;

void* testFunction(void* arguments);
void* testFunctionOut(void* arguments);
void* testFunctionIn(void* arguments);
void differentThanZero(double* testTab);
void sequentialTest(pointerToFunction function, data* myData);
void concurrentTest(pointerToFunction function, data* myData);
void concurrentTestMixed(pointerToFunction functionOne, pointerToFunction functionTwo, data* myData);

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


    // Preparing data for tests
    pointerToFunction functions[3] = {
            testFunction,
            testFunctionOut,
            testFunctionIn
    };

    pointerToTest tests[2] = {
            sequentialTest,
            concurrentTest
    };

    const char* names[6] = {
            "Sequential test without mutexes!\n",
            "Sequential test with mutexes out of loop!\n",
            "Sequential test with mutexes in loop!\n",
            "Concurrent test without mutexes!\n",
            "Concurrent test with mutexes out of loop!\n",
            "Concurrent test with mutexes in loop!\n",
            };

    //--------------------------------------

    // tutaj inne rozwiązanie podobne do tego co w 5.2 zrobilem

    for( int i=0; i<2; ++i ) {

        for( int j=0; j<3; ++j ) {

            //tests
            printf("%s",names[j+i*3]);
            start = clock();
            tests[i](functions[j],myData);
            end = clock();
            //---------------------

            // Check results and zero table
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            differentThanZero(testTab);
            printf("Time used: %f\n\n", cpu_time_used );
            memset( (double*)testTab, 0, sizeof(double)*SIZE);
            //--------------

        }

    }

    test testFunctions[2] = { {concurrentTestMixed,testFunctionOut,testFunctionIn},
                              {concurrentTestMixed,testFunctionIn,testFunctionOut} };
    const char* namesForMixedTests[2] = {"Concurrent test mixed with out fn first","Concurrent test mixed with in fn first"};

    for( int i=0; i<2; ++i ) {
        printf("%s\n",namesForMixedTests[i]);
        start = clock();
        testFunctions[i].pT(testFunctions[i].firstFunction,testFunctions[i].secondFunction,myData);
        end = clock();
        // Check results and zero table
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        differentThanZero(testTab);
        printf("Time used: %f\n\n", cpu_time_used );
        memset( (double*)testTab, 0, sizeof(double)*SIZE);
        //--------------

    }

    /*
     * tez zostawie dwie wersje jak wczesniej
    printf("Concurrent test mixed with out fn first");

    start = clock();
    concurrentTestMixed(testFunctionOut,testFunctionIn,myData);
    end = clock();

    // Check results and zero table
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    differentThanZero(testTab);
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------

    printf("Concurrent test mixed with in fn first");

    start = clock();
    concurrentTestMixed(testFunctionIn,testFunctionOut,myData);
    end = clock();

    // Check results and zero table
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    differentThanZero(testTab);
    printf("Time used in sequential test: %f\n\n", cpu_time_used );
    memset( (double*)testTab, 0, sizeof(double)*SIZE);
    //--------------
*/
    res = pthread_mutex_destroy(&workMutex);
    if( res != 0 ) {
        perror("Unable to destroy mutex!\n");
    }
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

void sequentialTest(pointerToFunction function, data* myData) {

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


void concurrentTest(pointerToFunction function, data* myData) {

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

void concurrentTestMixed(pointerToFunction functionOne, pointerToFunction functionTwo, data* myData) {
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
    int res = 0;
    for( int i=0; i<helper->numberOfIterations; ++i ) {
        res = pthread_mutex_lock(&workMutex);
        if( res != 0 ) {
            perror("Unable to lock mutex!\n");
            exit(EXIT_FAILURE);
        }
        for( int j=0; j<SIZE; ++j )
            helper->testTab[j] += helper->delta;
        res = pthread_mutex_unlock(&workMutex);
        if( res!= 0 ) {
            perror("Unable to unlock mutex!\n");
            exit(EXIT_FAILURE);
        }
    }
    pthread_exit(0);
}

void* testFunctionIn(void* arguments) {
    data* helper = (data*)arguments;
    int res = 0;
    for( int i=0; i<helper->numberOfIterations; ++i ) {
        for( int j=0; j<SIZE; ++j ) {
            res = pthread_mutex_lock(&workMutex);
            if( res != 0 ) {
                perror("Unable to lock mutex!\n");
                exit(EXIT_FAILURE);
            }
            helper->testTab[j] += helper->delta;
            res = pthread_mutex_unlock(&workMutex);
            if( res!= 0 ) {
                perror("Unable to unlock mutex!\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    pthread_exit(0);
}
