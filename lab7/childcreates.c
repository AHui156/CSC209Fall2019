#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int i;
    int iterations;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: childcreates <iterations>\n");
        exit(1);
    }
    
    iterations = strtol(argv[1], NULL, 10);
    int n = 0; 
    int wstatus; 
    printf("%d", getpid());
    fflush(stdout);
    for (i = 0; i < iterations; i++){
        if (n == 0){
            n = fork();
            if (n == 0){
                printf(" -> %d ", getpid());
                fflush(stdout);
            }
        }
    }
    wait(&wstatus);
    return 0;
}
