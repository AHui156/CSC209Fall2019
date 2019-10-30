#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"


/* Print the list of actions */
void print_actions(Action *act) {
    while(act != NULL) {
        if(act->args == NULL) {
            fprintf(stderr, "ERROR: action with NULL args\n");
            act = act->next_act;
            continue;
        }
        printf("\t");

        int i = 0;
        while(act->args[i] != NULL) {
            printf("%s ", act->args[i]) ;
            i++;
        }
        printf("\n");
        act = act->next_act;
    }    
}

/* Print the list of rules to stdout in makefile format. If the output
   of print_rules is saved to a file, it should be possible to use it to 
   run make correctly.
 */
void print_rules(Rule *rules){
    Rule *cur = rules;
    
    while(cur != NULL) {
        if(cur->dependencies || cur->actions) {
            // Print target
            printf("%s : ", cur->target);
            
            // Print dependencies
            Dependency *dep = cur->dependencies;
            while(dep != NULL){
                if(dep->rule->target == NULL) {
                    fprintf(stderr, "ERROR: dependency with NULL rule\n");
                }
                printf("%s ", dep->rule->target);
                dep = dep->next_dep;
            }
            printf("\n");
            
            // Print actions
            print_actions(cur->actions);
        }
        cur = cur->next_rule;
    }
}

/* Create the rules data structure and return it.
   Figure out what to do with each line from the open file fp
     - If a line starts with a tab it is an action line for the current rule
     - If a line starts with a word character it is a target line, and we will
       create a new rule
     - If a line starts with a '#' or '\n' it is a comment or a blank line 
       and should be ignored. 
     - If a line only has space characters ('', '\t', '\n') in it, it should be
       ignored.
 */
Rule *parse_file(FILE *fp) {

    // TODO

    return NULL;
}

