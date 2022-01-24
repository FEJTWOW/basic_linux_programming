#define _GNU_SOURCE
#include <string.h>
//#include <signal.h>
//#include <sys/stat.h>
//#include <sys/wait.h>
//#include <fcntl.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#define TR 10
#define TP 1
//#include <sys/mman.h>


int fn(void *);

typedef struct data {
    char* parameter;
    int number;
} processData;

int main(int argc, char* argv[]) {

    void *pchild_stack = malloc(1024 * 1024);
    processData* data = calloc( argc-1, sizeof(processData ));
    for( int i=1; i<argc; ++i ) {
        data[i-1].number = i;
        data[i-1].parameter = argv[i];
        int success = clone(fn, pchild_stack + (1024*1024), NULL, &data[i-1] );
        if( success == -1 ) {
            printf("Error in creating child process!@!!@!");
            return -5;
        }
    }
    sleep(TR);
    free(pchild_stack);
    free(data);



    return 0;
}

int fn(void* arg) {
    processData* helper = (processData *)arg;
    printf("Child process number %d\n", helper->number );
    printf("Parent process id %d\n", getppid() );
    printf("Process id %d\n", getpid() );
    for( int i=0; i<3; ++i ) {
        printf("Process parameter %s\n", helper->parameter );
        sleep(TP);
    }
    return 1;
}
