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
    /*
        1. Traverse through each line 
        2. Check if empty line - if so, close current rule of actions 
        3. If starting with a word -> check if rule already exists, otherwise a new rule + target is created 
            - Extract dependencies -> for each dependency, construct dep node, construct rule and append to rule-LL 
            - Extract actions -> for each action, extract args 

        a. Traverse through each line and identify as ignore, target, or action 
        b. If target: 
            - Check if it already exists as a rule in rule linked list and populate that 
            - Otherwise create new rule 
        c. If not target, must be action: 
            - Extract actions and populate current rule 

    */    
   
   char inputline[MAXLINE];  
   Rule* head = NULL; 
   while(fgets(inputline, MAXLINE, fp) != NULL){

       // check if comment or \n 
        if (inputline[0] == '#' || inputline[0] == '\n') continue; 
        // check if empty
        if (is_comment_or_empty(inputline) == 1) continue; 

        if(inputline[0] != '\t'){
            // Processing target line here
            remove_comment(inputline); 
            printf("Processing this target line now: %s", inputline);
        } else {
            // Processing action line here
            printf("Processing this action line now: %s", inputline);
            
        }


        
            
        
        



    


   }
   


    return NULL;
}

