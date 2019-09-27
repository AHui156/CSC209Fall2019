#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
void read_image(int num_rows, int num_cols, 
                int arr[num_rows][num_cols], FILE *fp);
                
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]);

// Remember to include the function prototype for count_cells
int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]);

int main(int argc, char **argv) {
    // Print a message to stderr and exit with an argument of 1 if there are
    // not the right number of parameters, or the second argument is not -p
    if(argc > 3 || argc == 1){
        fprintf(stderr, "Incorrect number of arguments\n"); 
        return 1;
    }
    else if(argc == 3 && strcmp(argv[2], "-p") != 0){
        fprintf(stderr, "-p is the optional argument\n" ); 
        return 1;
    }
    
    int rows = 0, columns = 0; 
    FILE* fp = fopen(argv[1], "r"); 
    if (fp != NULL){
        fscanf(fp, "%d %d\n", &rows, &columns); 
        int a[rows][columns]; 
        read_image(rows, columns, a, fp);  
        if (argc == 3){ print_image(rows, columns, a);}
        printf("Number of Cells is %d\n", count_cells(rows, columns, a));
        fclose(fp);
    } else {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
    }    
    return 0;
}
