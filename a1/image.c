#include <stdio.h>
/* Reads the image from the open file fp into the two-dimensional array arr 
 * num_rows and num_cols specify the dimensions of arr
 */
void read_image(int num_rows, int num_cols, 
                int arr[num_rows][num_cols], FILE *fp) {
    for(int i = 0; i < num_rows; i++){
        for(int m = 0; m < num_cols; m++){
            fscanf(fp, "%d ", &arr[i][m]);
        }
    }
}

/* Print to standard output the contents of the array arr */
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
    for(int i = 0; i < num_rows; i++){
        for(int m = 0; m < num_cols; m++){
            printf("%d ", arr[i][m]);
        }
        printf("\n"); 

    }
}

/*
* Gets the current cell coordinates and recursively checks adjacent cells until 
* all cells that are connected are set to 0.
*/
void set_cells(int num_rows, int num_cols, int i, int m, int arr[num_rows][num_cols]) {
    if (i - 1 >= 0){
        if(arr[i - 1][m] == 255) {
            arr[i - 1][m] = 0;
            set_cells(num_rows, num_cols, i - 1, m, arr);
        }
    }
    if (i + 1 < num_rows){
        if(arr[i + 1][m] == 255) {
            arr[i + 1][m] = 0;
            set_cells(num_rows, num_cols, i + 1, m, arr);
        }
    } 
    if (m - 1 >= 0){
        if (arr[i][m - 1] == 255) {
            arr[i][m - 1] = 0;
            set_cells(num_rows, num_cols, i, m - 1, arr);
        }
    }
    if (m + 1 < num_cols){
        if (arr[i][m + 1] == 255) {
            arr[i][m + 1] = 0;
            set_cells(num_rows, num_cols, i, m + 1, arr);
        }
    }
}

int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
    int cells = 0;
    for(int i = 0; i < num_rows; i++){
        for(int m = 0; m < num_cols; m++){
            if (arr[i][m] == 255){ 
                cells++;
                arr[i][m] = 0; 
                set_cells(num_rows, num_cols, i, m, arr); 
            }
        }
    }
    return cells;
}
