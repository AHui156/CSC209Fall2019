#include <stdio.h>

int main(){
    char phone[11];
    int in_int; 
    scanf("%s" ,phone); 
    // see that a pointer/addres must be passed into scanf
    scanf("%d", &in_int); 
    if (in_int == -1){
        printf("%s", phone);
        return 0; 
    } else if (in_int >= 0 && in_int <= 9){
        printf("%c", phone[in_int]); 
        return 0;
    } else {
        printf("ERROR"); 
        return 1; 
    }
} 
