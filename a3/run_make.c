#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"

// This function compares the time of last modification between 2 files 
// If the first is earlier than the second, return 0, else return 1
// int compare_mtime(const char *pathname1, const char *pathname2){
//     struct stat ret1, ret2; 
//     stat(pathname1, &ret1); 
//     stat(pathname2, &ret2); 
//     if (ret1.st_mtimespec.tv_sec < ret2.st_mtimespec.tv_sec){ return 0; } 
//     else if(ret1.st_mtimespec.tv_sec > ret2.st_mtimespec.tv_sec){ return 1; } 
//     else {
//         // seconds are equal  
//         if (ret1.st_mtimespec.tv_nsec < ret2.st_mtimespec.tv_nsec){ return 0; }
//          else { return 1; }
//     }
// }

int compare_mtime(const char *pathname1, const char *pathname2){
    struct stat ret1, ret2; 
    stat(pathname1, &ret1); 
    stat(pathname2, &ret2); 
    if (ret1.st_mtimespec.tv_sec < ret2.st_mtimespec.tv_sec){ return 0; } 
    else { return 1; } 
}

// need to check if target file exists! 


// what does pflag do? 
void run_make(char *target, Rule *rules, int pflag) {
    // TODO
    // update dependencies - check the rule that it points to 
    // If target does not exist, or dependencies is more recent than target -> execute actions 

    // Traverse rules, find the target, extract dependencies 
    // for each dependency -> run_make on the rule it points to 
    Rule *check_rule = rules; 
    if (target != NULL){
        while (check_rule != NULL){
            if(strcmp(check_rule->target, target) == 0) break;
            else check_rule = check_rule->next_rule; 
        }
    }

    printf("Running make on: %s\n", check_rule->target); 

    Dependency* dep_head = check_rule->dependencies; 
    int update = 0;
    while(dep_head != NULL){        
        run_make(dep_head->rule->target, rules, 0);
        if (!compare_mtime(check_rule->target, dep_head->rule->target)){
            printf("dependency is more recent than target\n");
            update = 1; 
        }
        dep_head = dep_head->next_dep;
    } 

    // Check for existence of *.c files should always return true
    // -> assumes that the *.c files already exist anyway, otherwise error

    if (update || access(check_rule->target, F_OK ) == -1){
        // execute actions 
        Action* action_head = check_rule->actions; 
        while (action_head != NULL){
            //fork and execvp 
            int ret_fork = fork(); 
            if (ret_fork == -1){
                perror("fork"); 
                exit(EXIT_FAILURE); 
            } else if(ret_fork == 0){
                //child
                char** act_args = action_head->args;
                // printf("Executing: %s ", act_args[0]);
                // act_args = &act_args[1];
                // int arg_count = 0;
                // while(act_args[arg_count] != NULL){
                //     printf("%s ", act_args[arg_count]);
                //     arg_count++;
                // }
                execvp(act_args[0], &act_args[1]);
                exit(EXIT_SUCCESS);                    
            } else {
                //parent
                int status = 0;  
                wait(NULL);            
            }
            action_head = action_head->next_act; 
        }        
    }
    return ;
}

