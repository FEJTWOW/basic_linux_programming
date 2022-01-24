#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/wait.h>
#include <time.h>
#include <errno.h>

#define T0 1
#define TS 0.5

int validation( char , const char* ); 
int createProcess( int, unsigned long );

int main(int argc, char* argv[]) {

    if( argc != 2 ) {
        printf("Program takes only one parameter which is the number of processes!!\n");
        return -5;
    }
    char *endPointer = NULL;
    errno = 0;
    long numberOfProcesses = strtol( argv[1], &endPointer, 10  );
    if( validation(*(argv[1]),endPointer ) == 0 ) {
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
    printf("Main process pid %d\n", getpid());
    createProcess(0, numberOfProcesses);


    return 0;
}
int validation( char argument, const char* endPointer ) {

    if( argument != '\0' && *endPointer == '\0' ) {
        return 1;
    }
    else {
        return 0;
    }
}


int createProcess( int currentNumberOfProcesses, unsigned long numberOfProcesses ) {

    if( currentNumberOfProcesses < numberOfProcesses ) {
        pid_t pid = fork();
        switch( pid ) {
            case -1:
                perror("Error in creating process\n");
                return -2;
            case 0:
                //printf("Liczba procesow %d\n",currentNumberOfProcesses );
                createProcess(currentNumberOfProcesses+=1, numberOfProcesses);
                break;
            default: {
                int status = 0;
                pid_t childPid = wait(&status);
                if( WIFEXITED(status) ) {
                    printf("Child pid: (%d), status: (%d)\n", childPid, WEXITSTATUS(status));
                }
                else if( WIFSIGNALED(status) ) {
                    if( WCOREDUMP(status) ) {
                        printf("Child produced a core dump!@!\n");
                    }
                    printf("Child pid: (%d), terminated by sginal: (%d)\n", childPid,WTERMSIG(status));
                }
                else {
                    perror("Error in wait!@@!@!\n");
                }

            }
        }
    }
    double timeToWait = T0+currentNumberOfProcesses*TS;
    int timeInNanoSeconds = ( timeToWait - (int)(timeToWait) ) *1000000000;
    struct timespec ts = { .tv_sec = timeToWait, .tv_nsec = timeInNanoSeconds };
    nanosleep(&ts,NULL);
    srand( getpid() + currentNumberOfProcesses );
    while(1) {

        int number = rand() % 129; // poprawiony wynik
        //printf("Wylosowana liczba %d\n", number );
        if( number > 0 && number < 17 ) kill( getpid(), number );
        else if ( number < 65 ) exit(number);
    }

}
