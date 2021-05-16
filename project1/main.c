
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define __MAX__BUFF__ 10000
#define MILISEC_PER_SEC 1000

/* function prototype in mdriver.c */
void Seq_pros(int **, int,int,int);
void Parallel_pros(int **,int,int, int,int);
void Parallel_Thread(int **,int,int,int,int);
void Print_matrix(int ** ,int,int);

int col=0,row=0;



int **Readmatrix(int **matrix, FILE *fp )
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
        if ( *(ptr + i) == '0' || *(ptr + i) ==  '1')
          col++;
      }
    }
  }
  matrix = (int **)malloc(sizeof(int *) * (row+2));
  rewind(fp);

  for (int i = 0; i < row+2; i++)
  {
    *(matrix + i) = (int *)malloc(sizeof(int) * (col+2));
    for (int j = 0; j < col+2; j++)
    {
      if(j==0 || j == col + 1 ||i == 0 || i == row + 1){
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

void Print_RunningTime(double time_rdmtrx,clock_t starttime, clock_t endtime){
  double diff = (double)(endtime-starttime) / CLOCKS_PER_SEC * MILISEC_PER_SEC;
  printf("Total time(readMatrix + operation) : %fms\n",diff + time_rdmtrx);

  return ;
}
void Continue_to_next(double time_rdmtrx,clock_t start, clock_t end, int type, int Thread_or_Process , int gen){
  switch (type)
  {
  case 2: /* seq_process */
    printf(" << Seq_process Completed! >>\n\n");
    printf(" Generation : %d\n\n",gen);
    break;
  case 3: /* parallel_process */
    printf(" << parallel_process Completed! >>\n\n");
    printf(" Number of Process : %d\n\n", Thread_or_Process);
    printf(" Generation : %d\n\n",gen);

    break;
  case 4: /* parallel_Thread */
    printf(" << parallel_Thread Completed! >>\n\n");
    printf(" Number of Thread : %d\n\n", Thread_or_Process);
    printf(" Generation : %d\n\n",gen);
    break;
  default:
    printf("Error - Undefined Type\n");
    exit(1);
    break;
  }
  Print_RunningTime(time_rdmtrx,start,end);
  printf("\n\n\n\nEnter \\n to continue...\n");
  fflush(stdin);
  int c;
  while (c = getchar() )
  {
    getchar();
    if (c == '\n')
      return;
    else
      printf("please, enter \\n to continue\n");
  }
}
int Menu_Interface(){
  int input=0;
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
    }
    else
    {
      return input;
    }
  }
}
void InterFace(int args, char ** argv){
  int branch_case = 0;
  int ** mtrx;
  double time_rdmtrx;
  clock_t start, end;
  FILE * fp;
  if(args != 2){
    fprintf(stderr,"Error - Unexpected argument\n");
    exit(1);
  }

  if( (fp = fopen(argv[1],"r")) == NULL){
    fprintf(stderr,"Open error");
    exit(1);
  }
  start = clock();
  mtrx = Readmatrix(mtrx, fp);
  end = clock();
  time_rdmtrx = (double) ( end - start) / CLOCKS_PER_SEC * MILISEC_PER_SEC;

  while (1)
  {
    branch_case = Menu_Interface();
    int gen;
    int cnt_num=0;
    if(branch_case != 1){
      printf("Enter Generation >> ");
      scanf("%d",&gen);
    }
    switch (branch_case)
    {
    case 1: /* Case - Exit */
      printf("Exiting Program...\n");
      exit(0);
      break;
    case 2: /* Case - 순차 처리 */
      system("clear");
      start = clock();
      Seq_pros(mtrx,row,col,gen);
      end = clock();
      Continue_to_next(time_rdmtrx, start,end,2,0,gen);
      break;
    case 3: /* Case - Process 병렬 처리 */
      while (1)
      {
        printf("Enter #(Child Process) >> ");
        scanf("%d", &cnt_num);
        if (cnt_num <= 0){
          printf("Error - Unexpected child Process\n");
          printf("Retry it\n");
        }
        else{
          break;
        }
      }
      system("clear");
      start = clock();
      Parallel_pros(mtrx,row,col,gen,cnt_num);
      end = clock();
      Continue_to_next(time_rdmtrx,start,end,3,cnt_num,gen);
      
      break;
    case 4: /* Case - Thread 병렬 처리 */
      while (1)
      {
        printf("Enter #(Thread) >> ");
        scanf("%d", &cnt_num);
        if (cnt_num <= 0){
          printf("Error - Unexpected child Process\n");
          printf("Retry it\n");
        }
        else{
          break;
        }
      }
      system("clear");
      start = clock();
      Parallel_Thread(mtrx,row,col,gen,cnt_num);
      end = clock();
      Continue_to_next(time_rdmtrx,start,end,4,cnt_num,gen);
      break;
    default:
      break;
    }
  }

}
int main(int args, char ** argv){

  InterFace(args,argv);

  return 0;
}

