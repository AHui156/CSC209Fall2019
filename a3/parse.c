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
   char *inputline, *tofree;
   inputline = malloc(sizeof(char) * MAXLINE);
   tofree = inputline;  
   Rule *rule_head = NULL; 
   Rule *curr_rule = NULL;  
   while(fgets(inputline, MAXLINE, fp) != NULL){

       // check if comment or \n or empty 
        if (inputline[0] == '#' || inputline[0] == '\n' || is_comment_or_empty(inputline) == 1){
            continue;
        } 

        if(inputline[0] != '\t'){
            // Processing target line here
            remove_trailing(inputline); // removing trailing comment and newline 
            printf("Processing this target line now: %s\n", inputline);

            // Extract target 
            char *target = strsep(&inputline, " ");
            strsep(&inputline, " "); // removes the semicolon

            
            // Traverse the Rule LL to check if rule already exists
            Rule *check_rule = rule_head; 
            while (check_rule != NULL){
                if (strcmp(check_rule->target, target) == 0){
                    // rule found
                    // set current rule to this Rule
                    curr_rule = check_rule;
                    break;
                } else {
                    check_rule = check_rule->next_rule;
                } 
            }
            
            // If Rule doesn't exist - initialize a new Rule, set as current Rule
            // Append new Rule to the Rule linked list
            if (check_rule == NULL){
                Rule *new_rule;
                new_rule = malloc(sizeof(Rule)); 
                new_rule->target = malloc(sizeof(char) * (strlen(target) + 1)); 
                strncpy(new_rule->target, target, strlen(target));  
                new_rule->dependencies = NULL; 
                new_rule->actions = NULL;
                curr_rule = new_rule;
                new_rule->next_rule = rule_head; 
                rule_head = new_rule;
            } 
            
            // Construct new dependency and rule nodes for each dependency here
            // each new rule node is also appended to the rule LL 
            char *dependency;
            while((dependency = strsep(&inputline, " ")) != NULL){
                printf("dependency: %s\n", dependency);

                // Create a new dependency node
                Dependency* new_dep = malloc(sizeof(Dependency)); 

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
                // Initialize a new Rule and target
                // Set current dependency to point to this Rule
                
                if (check_rule == NULL){
                    Rule* new_rule;
                    new_rule = malloc(sizeof(Rule)); 
                    new_rule->target = malloc(sizeof(char) * MAXLINE); 
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
            // Each action node corresponds to a single action line
            printf("Processing this action line now: %s", inputline);

            // Check that the action line belongs to a rule 
            // Note: we assume that there are no empty lines between action lines
            // i.e. 
            if (curr_rule == NULL){
                fprintf(stderr, "Encountered standalone action line. Exiting program.\n"); 
                exit(1);
            }

            char** action_args = build_args(inputline); 
            Action* new_action = malloc(sizeof(Action)); 
            new_action->args = action_args;
            new_action->next_act = curr_rule->actions; 
            curr_rule->actions = new_action; 
        } 
   }
   free(tofree); 
   return rule_head;
}



