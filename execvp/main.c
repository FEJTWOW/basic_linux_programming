#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


int main(int argc, char* argv[] ) {

    if( argc < 2 ) {
        perror("You must give at least one argument!@\n");
        return -2;
    }
    char* nameOfFile = argv[argc-1];
    argv[argc-1] = NULL;

    int standardInput = open(nameOfFile,O_RDONLY);
    if( standardInput == -1 ) {
        perror("Error in opening file!@!@\n");
        return -3;
    }
    // replace standard input with input file
    int newFileDescriptor = dup2(standardInput,0);
    if( newFileDescriptor == -1 ) {
        perror("Error in dup2 function\n@!@@!");
        return -4;
    }
    close(standardInput);
    argv[0] = "tr"; 
    execvp("tr",argv);
    return 0;
}
