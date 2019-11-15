/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed. 
 */
long num_reads, seconds;

// signal handler function 
void sig_handler(int signal){
  printf(MESSAGE, num_reads, seconds); 
  exit(1);
}


/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);
    num_reads = 0;

    FILE *fp;
    if ((fp = fopen(argv[2], "rb")) == NULL) {
      perror("fopen");
      exit(1);
    }

    //construct new sigaction struct
    struct sigaction new_sig; 
    new_sig.sa_handler = sig_handler; 
    new_sig.sa_flags = 0; 
    sigemptyset(&new_sig.sa_mask); 

    sigaction(SIGPROF, &new_sig, NULL);  

    struct itimerval new_time, old_time; 
    new_time.it_interval.tv_sec = 0; 
    new_time.it_interval.tv_usec = 0; 
    new_time.it_value.tv_sec = seconds; 
    new_time.it_value.tv_usec = 0; 
    if(setitimer(ITIMER_PROF, &new_time, &old_time) < 0){
      perror("stitemer"); 
      exit(1); 
    }    

    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    for (;;) {
      int num = random() % 99; 
      fseek(fp, sizeof(int) * num, SEEK_SET);
      int output = 0; 
      fread(&output, sizeof(int), 1, fp);
      printf("Count %ld: %d\n", num_reads, output); 
      sleep(1); 
      num_reads++; 
      getitimer(ITIMER_PROF, &old_time); 
      printf("Current time: %ld\n", old_time.it_value.tv_sec);
    }
    return 1; // something is wrong if we ever get here!
}
