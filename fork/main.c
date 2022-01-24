
#include <stdio.h>
#include <stdlib.h>
//#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#define TP 1
#define TR 10
//#define check

int main(int argc, char* argv[] ) {
    struct timespec tsChild = { .tv_sec = TP, .tv_nsec = 0 };
    struct timespec tsParent = { .tv_sec = TR, .tv_nsec = 0 };
    for( int i=1; i<argc; ++i ) {

        pid_t pid = fork();
        switch( pid ) {
            case -1:
                perror("Error in creating process\n");
                return -2;
            case 0:
                printf("PROCESS NUMBER %d\n", i );
                printf("Child process ID %d\n", getpid() );
                printf("Parent process ID %d\n", getppid() );
                for( int j=0; j<3; ++j ) {
                    printf("Child process parameter %s\n", argv[i] );
                    #ifdef check
                        char bufforP[30] = {0};
                        sprintf(bufforP,"ps -p %d -o pid,ppid,stat", getpid() );
                        system(bufforP);
                    #endif
                    nanosleep(&tsChild,NULL);
                }
                exit(0);
            default: break;
        }
    }
    #ifdef check
        char buffor[20];
        sprintf(buffor,"pstree -s %d",getpid() );
        system(buffor);
    #endif
    nanosleep(&tsParent,NULL );
    return 0;
}
