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
//     if (ret1.st_mtim.tv_sec < ret2.st_mtim.tv_sec){ return 0; } 
//     else if(ret1.st_mtim.tv_sec > ret2.st_mtim.tv_sec){ return 1; } 
//     else {
//         // seconds are equal  
//         if (ret1.st_mtim.tv_nsec < ret2.st_mtim.tv_nsec){ return 0; }
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

void run_make(char *target, Rule *rules, int pflag) {
    Rule *check_rule = rules; 
    if (target != NULL){
        while (check_rule != NULL){
            if(strcmp(check_rule->target, target) == 0) break;
            else check_rule = check_rule->next_rule; 
        }
    }
    printf("Running target: %s\n", check_rule->target); 

    Dependency* dep_head = check_rule->dependencies;
    int dep_count = 0;
    int dep_fork;      
    while(dep_head != NULL){        
        if (pflag == 1){ 
            dep_fork = fork();
            if(dep_fork == -1){
                perror("Dependency fork"); 
                exit(EXIT_FAILURE);
            } else if(dep_fork == 0){
                //child 
                // should terminate after running it's dependency?
                // the purpose of the child is to fork out the dep
                // It's task is just to run the dep
                run_make(dep_head->rule->target, rules, 0);
                exit(EXIT_SUCCESS);
                
            }
        } else { run_make(dep_head->rule->target, rules, 0); }
        dep_head = dep_head->next_dep;
        dep_count++;
    }

    // Call the corresponding number of waits in the parent
    if(pflag == 1 && dep_fork > 0){
        for(int i = 0; i < dep_count; i++){
            int dep_status = 0;
            if(wait(&dep_status) == -1){
                perror("wait failure");
                exit(EXIT_FAILURE); 
            } else {
                puts("Child terminated successfully");
            }
        }
    } 

    // Check if any dependency is more recent than the current target
    int update = 0;
    dep_head = check_rule->dependencies; 
    while(dep_head != NULL){
        // need to check that file exists first 

        if (!compare_mtime(check_rule->target, dep_head->rule->target)){
            printf("%s is more recent than %s\n", dep_head->rule->target, check_rule->target);            
            update = 1; 
        }
        dep_head = dep_head->next_dep;
    }

    // If dependency is more recent, or target file doesn't exist -> execute action
    // We assume here that *.c file must exist 

    if (update || access(check_rule->target, F_OK ) == -1){
        // execute actions 
        Action* action_head = check_rule->actions;
        // int arg_count = 0;
        // printf("Executing: ");
        // while(act_args[arg_count] != NULL){
        //     printf("%s ", act_args[arg_count]);
        //     arg_count++;
        // }
        // printf("\n");
        while (action_head != NULL){
            char **act_args = action_head->args;
            int arg_count = 0;
            printf("Executing: ");
            while(act_args[arg_count] != NULL){
                printf("%s ", act_args[arg_count]);
                arg_count++;
            }
            printf("\n");
            //fork and execvp 
            int exec_fork = fork(); 
            if (exec_fork == -1){
                perror("fork"); 
                exit(EXIT_FAILURE); 
            } else if(exec_fork == 0){
                //child                
                if (execvp(act_args[0], act_args) == -1){
                    perror("execvp"); 
                    exit(EXIT_FAILURE);
                } else { exit(EXIT_SUCCESS); }
            } else {
                //parent
                int status = 0;  
                if (wait(&status) == -1){
                    perror("wait"); 
                    exit(EXIT_FAILURE);
                }
                if (WEXITSTATUS(status) == EXIT_FAILURE){
                    exit(EXIT_FAILURE);                      
                }
            }
            action_head = action_head->next_act; 
        }        
    }
}

