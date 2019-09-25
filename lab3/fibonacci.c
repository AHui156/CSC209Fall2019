#include <stdlib.h>
#include <stdio.h>
/*
 * Define a function void fib(...) below. This function takes parameter n
 * and generates the first n values in the Fibonacci sequence.  Recall that this
 * sequence is defined as:
 *         0, 1, 1, 2, 3, 5, ... , fib[n] = fib[n-2] + fib[n-1], ...
 * The values should be stored in a dynamically-allocated array composed of
 * exactly the correct number of integers. The values should be returned
 * through a pointer parameter passed in as the first argument.
 *
 * See the main function for an example call to fib.
 * Pay attention to the expected type of fib's parameters.
 */

/* Write your solution here */

void fib(int** ptr, int n){
    int* temp = calloc(n, sizeof(int));
    // int* temp = malloc(sizeof(int) * n);
    for(int i = 0; i < n; i++){
        // set for 0 and 1 
        if (i == 0 || i == 1){ temp[i] = i; }
        else {
            temp[i] = temp[i - 1] + temp[i - 2];
        }
    }
    *ptr = temp;
}

int main(int argc, char **argv) {
    /* do not change this main function */
    int count = strtol(argv[1], NULL, 10);
    int *fib_sequence;

    fib(&fib_sequence, count);
    for (int i = 0; i < count; i++) {
        printf("%d ", fib_sequence[i]);
    }
    free(fib_sequence);
    return 0;
}

