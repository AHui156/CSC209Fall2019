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

    NOTE: Nodes are stored in a Last In, First Out Linked List (LL), the first added Node is thus at the tail. 

 */
Rule *parse_file(FILE *fp) {
    char *inputline = calloc(MAXLINE, sizeof(char));
   char *tofree = inputline;  
   Rule *rule_head = NULL; 
   Rule *curr_rule = NULL;  
   while(fgets(inputline, MAXLINE, fp) != NULL){

       // check if comment or \n or empty 
        if (inputline[0] == '#' || inputline[0] == '\n' || is_comment_or_empty(inputline) == 1) continue;

        // removing trailing comment and newline
        remove_trailing(inputline); 

        if(inputline[0] != '\t'){
            // Processing target line here
            // Extract target 
            char *target = strsep(&inputline, " ");
            // removes the semicolon
            strsep(&inputline, " "); 
            
            // Traverse the Rule linked list to check if Rule already exists
            Rule *check_rule = rule_head; 
            while (check_rule != NULL){
                if (strcmp(check_rule->target, target) == 0){
                    // rule found
                    // set current Rule to this Rule
                    curr_rule = check_rule;
                    break;
                } else {
                    check_rule = check_rule->next_rule;
                } 
            }
            
            // If Rule doesn't exist - initialize a new Rule and set as current Rule
            // Push new Rule to the Rule linked list
            if (check_rule == NULL){
                Rule *new_rule = NULL;
                new_rule = malloc(sizeof(Rule)); 
                new_rule->target = calloc(strlen(target) + 1, sizeof(char));
                strncpy(new_rule->target, target, strlen(target));  
                new_rule->dependencies = NULL; 
                new_rule->actions = NULL;
                curr_rule = new_rule;
                new_rule->next_rule = rule_head; 
                rule_head = new_rule;
            } 
            
            // Construct new dependencies. If dependency does not point to Rule that exists, 
            // construct new Rule and push to Rule linked list. 
            char *dependency = NULL;
            while((dependency = strsep(&inputline, " ")) != NULL){

                // Create a new dependency node
                Dependency* new_dep = calloc(1, sizeof(Dependency)); 

                // Add dependency node to the current Rule 
                new_dep->next_dep = curr_rule->dependencies;
                curr_rule->dependencies = new_dep;

                // Check that rule does not already exist 
                check_rule = rule_head; 
                while (check_rule != NULL){
                    if (strcmp(check_rule->target, dependency) == 0){
                        // rule found
                        // set current dependency->rule to this rule 
                        new_dep->rule = check_rule;
                        break;
                    } else {
                        check_rule = check_rule->next_rule;
                    } 
                }

                // Rule is not found
                // Initialize a new Rule with target
                // Set current dependency to point to this Rule
                
                if (check_rule == NULL){
                    Rule* new_rule = NULL;
                    new_rule = calloc(1, sizeof(Rule)); 
                    new_rule->target = calloc(strlen(dependency) + 1, sizeof(char));
                    // new_rule->target = malloc(sizeof(char) * strlen(dependency)+1); 
                    // memset(new_rule->target, 0, sizeof(char) * (strlen(target) + 1));
                    strncpy(new_rule->target, dependency, strlen(dependency));
                    new_rule->dependencies = NULL; 
                    new_rule->actions = NULL;
                    new_dep->rule = new_rule;
                    new_rule->next_rule = rule_head; 
                    rule_head = new_rule;                 
                }
            }
            inputline = tofree; 
        } else {  
            // Processing action line here
            if (curr_rule == NULL){
                fprintf(stderr, "Encountered standalone action line. Exiting program.\n"); 
                exit(1);
            }
            
            Action* new_action = calloc(1, sizeof(Action)); 
            new_action->args = build_args(inputline);
            new_action->next_act = NULL;
            if(curr_rule->actions == NULL){ curr_rule->actions = new_action; }
            else{
                Action *curr_action = curr_rule->actions; 
                while(curr_action->next_act != NULL){ curr_action = curr_action->next_act; }
                curr_action->next_act = new_action; 
            }

            // strsep() in build_args() modifies pointer to inputline
            // Need to reset it to the beginning of inputline.
            inputline = tofree;
        } 
   }
    
    
    // Reverse the Rule linked list
    Rule *current = rule_head;
    Rule *prev, *next;
    prev = next = NULL; 
    while(current != NULL){
        next = current->next_rule; 
        current->next_rule = prev; 
        prev = current; 
        current = next; 
    }
    rule_head = prev;

    free(tofree);
    return rule_head;
}
