#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int i;
    int iterations;

    if (argc != 2) {
        fprintf(stderr, "Usage: parentcreates <iterations>\n");
        exit(1);
    }

    iterations = strtol(argv[1], NULL, 10);
    int n = 1; 
    int wstatus; 
    for (i = 0; i < iterations; i++) {
        if (n < 0) {
            perror("fork");
            exit(1);
        } else if (n != 0){
            n = fork();
            if (n != 0){
                printf("%d -> %d\n", getpid(), n);
                wait(&wstatus);
            }
        }
    }
    return 0;
}
