#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char* argv[] ) {

    if( argc == 1 ) { 
        perror("You have to give an argument!@!\n");
        return 200;
    }

    int failedCount = 0;
    for( int i=1; i<argc; ++i ) {

        char *endPointer = NULL;
        long double longDoubleNumber = strtold(argv[i], &endPointer);
       
        if( *(argv[i]) != '\0' && *endPointer == '\0' ) {

            unsigned long long unsignedLongLongNumber = strtoull(argv[i], NULL, 10);
          

            if (unsignedLongLongNumber == longDoubleNumber) {
         
                    printf("(UNSIGNED LONG): %llu\n", unsignedLongLongNumber);
            } else {

                long long longLongNumber = strtoll(argv[i], NULL, 10);
                if (longLongNumber == longDoubleNumber) {
                        printf("(LONG): %lli\n", longLongNumber);
                } else {

                    float floatNumber = strtof(argv[i], NULL);
                    if (floatNumber == longDoubleNumber) {
                        printf("(FLOAT): %f\n", floatNumber);
                    } else {

                        double doubleNumber = strtod(argv[i], NULL);
                        if (doubleNumber == longDoubleNumber) {
                            printf("(DOUBLE): %lf", doubleNumber);
                        } else printf("(LONG DOUBLE): %Lf\n", longDoubleNumber);
                    }
                }
            }
        } else {
            printf("(FAIL): %s\n", argv[i]);
            failedCount++;
            continue;
        }
    }

    return ((double) failedCount / (argc - 1)) * 100;
}
