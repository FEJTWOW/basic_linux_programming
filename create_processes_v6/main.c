#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/wait.h>
#include <time.h>
#include <errno.h>

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
    pid_t* pid = (pid_t*)calloc(numberOfProcesses, sizeof(pid_t));
    for( int i=0; i<numberOfProcesses; ++i ) {

        pid[i] = fork();
        switch( pid[i] ) {
            case -1: perror("Error in creating process\n" ); break;
            case 0: {
                srand( getpid()+i );
                int K = rand()%76+25;
                struct timespec ts = { .tv_sec = (int)(K/22.5), .tv_nsec = ((K/22.5)-(int)(K/22.5))*1000000000L };
                nanosleep(&ts,NULL);
                exit(0);
            }
            default: break;
        }

    }
    struct timespec ts = { .tv_sec = 1, .tv_nsec = 0.2*1000000000L};
    unsigned int j = 0;
    int status = -1;
    while( 1 ) {
        pid_t childPid = waitpid( pid[j],&status,WNOHANG );
        if( childPid == -1 ) {
            fprintf(stderr,"Error in wait function!@!\n");
            return -5;
        }
        if( childPid == 0  ) {
            fprintf(stderr,"Unable to read child process with pid: (%d) \n", pid[j] );
            j = (j+1)%numberOfProcesses;
            nanosleep(&ts,NULL);
        }
        else {
            if( WIFEXITED(status) ) {
                fprintf(stderr,"Child pid: (%d), status: (%d)\n", childPid, WEXITSTATUS(status));
            }
            else {
                perror("Wrong status\n");
            }
            numberOfProcesses-=1;
            if( numberOfProcesses == 0 ) break;
            for( unsigned int k = j; k<numberOfProcesses; ++k ) {
                pid[k] = pid[k+1];
            }
            j = j%numberOfProcesses;

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


