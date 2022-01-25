#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[] ) {
    setenv("LANG","pl_PL.UTF-8",1);
    argv[0] = "man";
    execvp("man",argv);
    return 0;
}


