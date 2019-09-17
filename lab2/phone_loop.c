#include <stdio.h>

int main(){
    int return_flag = 0;
    char phone[11];
    int in_int; 
    scanf("%s" ,phone); 
    // see that a pointer/addres must be passed into scanf
    while(1){
        scanf("%d", &in_int); 
        if (in_int == -1){
            printf("%s", phone);
        } else if (in_int >= 0 && in_int <= 9){
            printf("%c", phone[in_int]); 
        } else {
            printf("ERROR"); 
            return_flag = 1;
        }
    }
    return return_flag;        
}
