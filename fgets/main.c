#include <stdio.h>
#include <string.h>
#include <unistd.h>

char global[20];

void printTab();

int main(int argc, char* argv[] ) {

    printTab();
    fgets(global,20,stdin);
    printTab();
    if( strcmp("END.",global) == 0 ) {
           return 0;
    }
    execv(argv[0],argv); // argv[argc] is nullptr
}

void printTab() {
    for( int i=0; i<20; ++i ) {
        printf("%x\n", global[i] );
    }
    printf("\n");
}
