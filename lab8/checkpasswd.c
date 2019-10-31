/*
Lab summary - This lab is meant to demonstrate use of fork() to create child processes 
and wait(&status) to get signals from the child, as well as using pipe() to 
send data to the child. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void)
{
  char user_id[MAXLINE];
  char password[MAXLINE];

  /* The user will type in a user name on one line followed by a password 
     on the next.
     DO NOT add any prompts.  The only output of this program will be one 
	 of the messages defined above.
   */

  if (fgets(user_id, MAXLINE, stdin) == NULL)
  {
    perror("fgets");
    exit(1);
  }
  if (fgets(password, MAXLINE, stdin) == NULL)
  {
    perror("fgets");
    exit(1);
  }

  // Create the pipe
  int pipefd[2];
  if (pipe(pipefd) == -1){
    perror("pipe failure"); 
    exit(EXIT_FAILURE); 
  } 

  // Fork
  int status;
  int result = fork();
  if (result == -1)
  {
    perror("fork failure");
    exit(1);
  }
  else if (result > 0)
  {
    // This is the parent
    close(pipefd[0]); // close the read pipe 
    write(pipefd[1], user_id, 10);
    write(pipefd[1], password, 10); // Why 10? 
    close(pipefd[1]); // close the write pipe 
    if (wait(&status) == -1)
    {
      perror("wait failure");
      exit(1);
    }
    if (WIFEXITED(status))
    {
      switch (WEXITSTATUS(status))
      {
      case 0 :
        write(STDOUT_FILENO, SUCCESS, strlen(SUCCESS));
        break;
      case 2 :
        write(STDOUT_FILENO, INVALID, strlen(INVALID));
        break;
      case 3 :
        write(STDOUT_FILENO, NO_USER, strlen(NO_USER));
        break;
      }
    }
  }
  else if (result == 0)
  {
    // this is the child
    close(pipefd[1]); // close the write pipe 
    dup2(pipefd[0], STDIN_FILENO); // dup stdin to read pipe
    close(pipefd[0]); // close the read pipe

    if (execl("./validate", "validate", NULL) == -1)
    {
      perror("execl failure");
      exit(1);
    }
  }

  return 0;
}
