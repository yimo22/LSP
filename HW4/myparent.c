#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char ** argv){
  if(argc != 2){
    fprintf(stderr, "Error - Unexpected Input\n");
    exit(1);
  }
  int fork_index = atoi(argv[1]);
  int cpid_index = 1;

  pid_t cur_pid;
  pid_t pid[fork_index];
  
  printf("started\n");
  for(int i=0;i<fork_index;i++){
    if( (cur_pid = fork()) < 0){
      fprintf(stderr,"fork()");
      exit(1);
    }
    else if( cur_pid == 0){
      int creation_index = cpid_index;
      while(1){
        printf("> This is %d's child process\n",creation_index);
        sleep(5);
      }
    }
    else{
      pid[i] = cur_pid;
      cpid_index++;
    }
  }
  // fork() completed
  
  // Wait 5sec and deleted inversely
  sleep(5);
  for(int i=fork_index-1;i>=0;i--){
    kill(pid[i],SIGINT);
    sleep(1);
  }
  printf("All child Process was killed, Parent Process will be terminated\n");
  raise(SIGINT);



  printf("이것이 출력되면 잘못된 것임\n");
  return 0;
}