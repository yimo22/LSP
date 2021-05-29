#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int count = 5;

int main(int argc, char **argv)
{
  pid_t pid;

  if( (pid = fork()) < 0){
    fprintf(stderr,"fork");
    exit(1);
  }
  else if (pid > 0)
  {
    if (argc == 2)
    {
      int timelimit = atoi(argv[1]);
      while (1)
      {
        if (timelimit == 0){
          kill(pid, SIGINT);
          wait(NULL);
          break;
        }
        if (0 < timelimit && timelimit <= 5)
          printf("> %d sec left to Shutdown... \n", count--);

        sleep(1);
        timelimit--;
      }
    }
    raise(SIGINT);
  }
  else{
    while(1){
      system("date");
      sleep(1);
    }
  }

  printf("이것이 출력되면 잘못된 것임.\n");
  return 0;
}