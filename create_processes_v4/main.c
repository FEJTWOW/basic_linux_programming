#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/wait.h>
#include <errno.h>
#include <time.h>

int validation( const char* , const char* );

int main(int argc, char* argv[] ) {

    if( argc != 2 ) {
        printf("Program takes only one parameter which is the number of processes!!\n");
        return -5;
    }
    char *endPointer = NULL;
    errno = 0;
    long numberOfProcesses = strtol( argv[1], &endPointer, 10  );

    if( validation(argv[1],endPointer ) == 0 ) {
        perror("Incorrect argument\n");
        return -4;
    }
    if( errno == ERANGE ) {
        perror("Number out of range@!@!\n");
        return -3;
    }
    if( numberOfProcesses < 0 ) {
        perror("Number of processes must be a positive number!@!\n");
        return -3;
    }
    printf("Main process pid: (%d)\n", getpid());
    pid_t* pid = (pid_t*)calloc( numberOfProcesses, sizeof(pid_t));
    for( int i=0; i<numberOfProcesses; ++i ) {

        pid[i] = fork();
        switch( pid[i] ) {
            case -1: perror("Error in creating process\n" ); break;
            case 0:
                srand(getpid()+i);
                double K = rand()%76+25;
                struct timespec ts = { .tv_sec = (int)(K/22.5), .tv_nsec = ((K/22.5)-(int)(K/22.5))*1000000000L };
                nanosleep(&ts,NULL);
                exit(0);
            default: break;
        }

    }
    int status = -1;
    for( int i=0; i< numberOfProcesses; ++i ) {
        int childPid = waitpid(pid[i],&status,0);
        if( WIFEXITED(status) ) {
            printf("Child pid: (%d), status: (%d)\n", childPid, WEXITSTATUS(status));
        }
        else {
            perror("Error in wait!@!");
        }
    }
    free(pid);
    return 0;
}

int validation( const char* argument, const char* endPointer ) {

    if( *argument != '\0' && *endPointer == '\0' ) {
        return 1;
    }
    else {
        return 0;
    }
}

