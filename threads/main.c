#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
//#include <sys/syscall.h>

typedef struct timespec timespec;
void* threadFunction(void* arguments);

int main() {




    timespec ts[3] = {{3,0.9125*1000000000},{3,0.015*1000000000},{2,0.7*1000000000}};
    int result = 0;
    pthread_t aThread[3] = {0};
    void* threadResult[3] = {0};
    for( int i=0; i<3; ++i ) {

        result = pthread_create(&aThread[i], NULL, threadFunction, &ts[i]);
        if( result!= 0 ) {
            perror("Thread creation failed!\n");
            exit(EXIT_FAILURE);
        }

    }
    for( int i = 0; i<3; ++i ) {
        result = pthread_join(aThread[i], (void*)&threadResult[i]);
        if (result != 0) {
            perror("Thread join failed!\n");
            exit(EXIT_FAILURE);
        }
        /*printf("Kernel thread id: (%lu) | Inside-program thread id: (%lu)\n",
                (unsigned long)threadResult[i], aThread[i]);*/
        printf("Tread id: (%lu) | result: (%lu)\n",aThread[i],(unsigned long)threadResult[i]);

    }

    return 0;
}

void* threadFunction(void* argument) {

    printf("Thread with id: %lu is starting\n",pthread_self());
    nanosleep((timespec*)argument,NULL);
    printf("Thread with id: %lu finished work!\n",pthread_self());
    //printf("%lu\n",syscall(SYS_gettid));
    //pthread_exit((long*)syscall(SYS_gettid));
    pthread_exit((void*)time(NULL));

}
