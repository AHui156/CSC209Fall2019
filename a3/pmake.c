#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"

/* This program reads and evaluates a Makefile very much like the program make
   but with some simplifications, and the option to evaluate dependencies in 
   parallel.

   Options:
     "-o"  - print the rules data structure as if it were a Makefile
     "-p"  - create a new process to evaluate each dependency of a rule
     "-f <filename>" - Use "filename" as the input Makefile.  If this option is
             not provided then pmake will use a file called "Makefile"
     "target" - specify the rule in the file to evaluate. If no target is
             provided, pmake will evaluate the first rule.
 */

int main(int argc, char **argv) {
    FILE *fp;
    char *filename = "Makefile";
    char *target = NULL;
    int output = 0;
    int parallel = 0;
    char opt;

    while ((opt = getopt(argc, argv, "f:op")) != -1) {
        switch (opt) {
        case 'o':
            output = 1;
            break;
        case 'p':
            parallel = 1;
            break;
        case 'f':
            filename = optarg;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-op][-f <filename>] [target]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if(optind < argc) {
        target = argv[optind];
    }
    if((fp = fopen(filename, "r")) == NULL) {
        perror("fopen");
    }

    Rule *rules = parse_file(fp);
    if(output) {
        print_rules(rules);
    }

    run_make(target, rules, parallel);


    // // Freeing up all malloced space 
    Rule *next_rule = NULL; 
    while(rules != NULL){
        next_rule = rules->next_rule;

        // free target 
        free(rules->target);

        // free dependency nodes
        Dependency* dep_head = rules->dependencies; 
        Dependency *next_head = NULL;
        while(dep_head != NULL){
            next_head = dep_head->next_dep;
            free(dep_head);
            dep_head = next_head;
        }

        //free action nodes
        Action *action_head = rules->actions;
        Action *next_action = NULL; 
        while(action_head != NULL){
            next_action = action_head->next_act; 
            int arg_count = 0; 
            char** action_args = action_head->args; 
            while(action_args[arg_count] != NULL){
                // free individual arg strings
                free(action_args[arg_count]);
                arg_count++;
            }
            free(action_head->args);
            free(action_head); 
            action_head = next_action;
        }

        //free the Rule 
        free(rules); 
        rules = next_rule;
    }
    
    fclose(fp);

    return 0;
}
