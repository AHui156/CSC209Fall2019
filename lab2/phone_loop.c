#include <stdio.h>

int main(){
    int return_flag = 0;
    char phone[11];
    int in_int; 
    scanf("%s", phone); 
    // see that a pointer/addres must be passed into scanf
    while(1){
        if (scanf("%d", &in_int)==EOF){return return_flag;} 
        if (in_int == -1){
            printf("%s\n", phone);
        } else if (in_int >= 0 && in_int <= 9){
            printf("%c\n", phone[in_int]); 
        } else {
            printf("ERROR\n"); 
            return_flag = 1;
        }
    }
}
