
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>

#define DEBUG 1
#define PID_MAX 32768
#define __MAX__BUFF__ 10000
#define MILISEC_PER_SEC 1000

/* function prototype in mdriver.c */
void Seq_pros(int **, int, int, int);
void Parallel_pros(int **, int, int, int, int, void *);
void Parallel_Thread(int **, int, int, int, int, void *);
void Print_matrix(int **, int, int);


int ** Copy2Dmatrix( int ** , int ** , int , int);



int col = 0, row = 0;

int **Readmatrix(int **matrix, FILE *fp)
{
  char strBuff[__MAX__BUFF__];
  char *ptr;
  // get row
  while (!feof(fp))
  {
    ptr = fgets(strBuff, sizeof(strBuff), fp);
    row++;
    if (col == 0)
    {
      for (int i = 0; i < strlen(ptr); i++)
      {
        if (*(ptr + i) == '0' || *(ptr + i) == '1')
          col++;
      }
    }
  }
  matrix = (int **)malloc(sizeof(int *) * (row + 2));
  rewind(fp);

  for (int i = 0; i < row + 2; i++)
  {
    *(matrix + i) = (int *)malloc(sizeof(int) * (col + 2));
    for (int j = 0; j < col + 2; j++)
    {
      if (j == 0 || j == col + 1 || i == 0 || i == row + 1)
      {
        *(*(matrix + i) + j) = 0;
        continue;
      }

      char ch = ' ';
      while (ch == ' ' || ch == '\n')
        ch = fgetc(fp);

      *(*(matrix + i) + j) = ch - '0';
    }
  }
  return matrix;
}

void Print_RunningTime(double time_rdmtrx, struct timeval starttime, struct timeval endtime)
{
  double diff = (double)(endtime.tv_sec - starttime.tv_sec) * MILISEC_PER_SEC + (double)(endtime.tv_usec - starttime.tv_usec) / 1000;
  printf("\nreading time : %f\n",time_rdmtrx);
  printf("\n\nTotal time(readMatrix + operation) : %fms\n", diff + time_rdmtrx);

  return;
}
void Continue_to_next(double time_rdmtrx, struct timeval start, struct timeval end, int type, int Thread_or_Process, int gen, void *input_ptr)
{

  switch (type)
  {
  case 2: /* seq_process */
    printf(" << Seq_process Completed! >>\n\n");
    printf(" Generation : %d\n\n", gen);
    break;
  case 3: /* parallel_process */
    printf(" << parallel_process Completed! >>\n\n");
    printf(" Number of Process : %d\n\n", Thread_or_Process);
    printf(" Generation : %d\n\n", gen);
    pid_t *pid_ptr = (pid_t *)input_ptr;
    for (int j = 0; j < gen; j++)
    {
      printf("\nIn calculating <<gen%d.matrix>>......\n", j + 1);
      for (int i = 0; i < Thread_or_Process; i++)
      {
        if ( 0 < *(pid_ptr + i + j * Thread_or_Process) &&  *(pid_ptr + i + j * Thread_or_Process) <= PID_MAX)
          printf(" > %d child process was used. \n", *(pid_ptr + i + j * Thread_or_Process));
        else
        {
          printf(" > not used proscess - same value as the previous generation \n");
          break;
        }
      }
    }
    break;
  case 4: /* parallel_Thread */
    printf(" << parallel_Thread Completed! >>\n\n");
    printf(" Number of Thread : %d\n\n", Thread_or_Process);
    printf(" Generation : %d\n\n", gen);
    pthread_t *pthread_ptr = (pthread_t *)input_ptr;
    for (int j = 0; j < gen; j++)
    {
      printf("\nIn calculating <<gen%d.matrix>>......\n", j + 1);
      for (int i = 0; i < Thread_or_Process; i++)
      {
        if (*(pthread_ptr + i + j * Thread_or_Process) != 0)
          printf(" > %lu thread was used. \n", *(pthread_ptr + i + j * Thread_or_Process));
        else
        {
          printf(" > not used thread - same value as the previous generation \n");
          break;
        }
      }
    }
    break;
  default:
    printf("Error - Undefined Type\n");
    exit(1);
    break;
  }
  Print_RunningTime(time_rdmtrx, start, end);
  printf("\n\n\n\nEnter \\n to continue...\n");
  fflush(stdin);
  int c;
  while (c = getchar())
  {
    getchar();
    if (c == '\n')
      return;
    else
      printf("please, enter \\n to continue\n");
  }
}
int Menu_Interface()
{
  int input = 0;
  while (1)
  {
    system("clear");
    printf("==============MENU==================\n");
    printf("(1) Exit Program\n");
    printf("(2) 순차처리\n");
    printf("(3) Process 병렬처리\n");
    printf("(4) Thread 병렬처리\n\n");
    printf(">> Choose NUM : ");
    scanf("%d", &input);
    printf("\n");
    if (input < 1 || input > 4)
    {
      printf("Error - Unexpected input\n\n\n");
      sleep(3);
    }
    else
    {
      return input;
    }
  }
}
int ** Copy2Dmatrix( int ** source , int ** copy, int row , int col){
  copy =  (int **) malloc (sizeof(int *) * (row+2));
  for(int i=0;i<=row+1;i++){
    *(copy + i) = (int *)malloc(sizeof(int) * (col+2) );
    for(int j=0;j<=col+1;j++){
      *(*(copy + i) + j) =  *(*(source + i) + j) ;
    }
  }
  return copy;
}
void InterFace(int args, char **argv)
{
  int branch_case = 0;
  int **mtrx;
  int ** copy_mtrx;
  double time_rdmtrx;
  struct timeval start_e , end_e;
  FILE *fp;
  if (args != 2)
  {
    fprintf(stderr, "Error - Unexpected argument\n");
    exit(1);
  }

  if ((fp = fopen(argv[1], "r")) == NULL)
  {
    fprintf(stderr, "Open error");
    exit(1);
  }

  gettimeofday(&start_e, NULL);
  mtrx = Readmatrix(mtrx, fp);
  gettimeofday(&end_e, NULL);
  time_rdmtrx = (double)(end_e.tv_sec - start_e.tv_sec) * 1000 + (double)(end_e.tv_usec - start_e.tv_usec);

  while (1)
  {
    copy_mtrx = Copy2Dmatrix(mtrx,copy_mtrx,row,col);
    branch_case = Menu_Interface();
    int gen;
    int cnt_num = 0;
    if (branch_case != 1)
    {
      printf("Enter Generation >> ");
      scanf("%d", &gen);
    }
    switch (branch_case)
    {
    case 1: /* Case - Exit */
      printf("Exiting Program...\n");
      exit(0);
      break;
    case 2: /* Case - 순차 처리 */
      system("clear");
  gettimeofday(&start_e, NULL);
      Seq_pros(copy_mtrx, row, col, gen);
  gettimeofday(&end_e, NULL);
      
      Continue_to_next(time_rdmtrx, start_e, end_e, 2, 0, gen, NULL);
      break;
    case 3: /* Case - Process 병렬 처리 */
      while (1)
      {
        printf("Enter #(Child Process) >> ");
        scanf("%d", &cnt_num);
        if (cnt_num <= 0)
        {
          printf("Error - Unexpected child Process\n");
          printf("Retry it\n");
        }
        else
        {
          break;
        }
      }
      pid_t *ptr = (pid_t *)malloc(sizeof(pid_t) * cnt_num * gen);
      system("clear");
  gettimeofday(&start_e, NULL);
      Parallel_pros(copy_mtrx, row, col, gen, cnt_num, ptr);
  gettimeofday(&end_e, NULL);
      Continue_to_next(time_rdmtrx, start_e, end_e, 3, cnt_num, gen, (void *)ptr);

      break;
    case 4: /* Case - Thread 병렬 처리 */
      while (1)
      {
        printf("Enter #(Thread) >> ");
        scanf("%d", &cnt_num);
        if (cnt_num <= 0)
        {
          printf("Error - Unexpected child Process\n");
          printf("Retry it\n");
        }
        else
        {
          break;
        }
      }
      pthread_t *tid_ptr = (pthread_t *)malloc(sizeof(pthread_t) * cnt_num * gen);
      system("clear");
  gettimeofday(&start_e, NULL);
      Parallel_Thread(copy_mtrx, row, col, gen, cnt_num, tid_ptr);
  gettimeofday(&end_e, NULL);
      Continue_to_next(time_rdmtrx, start_e, end_e, 4, cnt_num, gen, (void *)tid_ptr);
      break;
    default:
      break;
    }
  }
}
int main(int args, char **argv)
{

  InterFace(args, argv);

  return 0;
}
