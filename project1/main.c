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

void Continue_to_next(){
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
int Print_Interface(){
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
        if (*(ptr + i) != '\n' && *(ptr + i) != ' ')
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
void Print_RunningTime(clock_t starttime, clock_t endtime){
  double diff = (double)(endtime-starttime);
  printf("Total time : %fms\n",diff);

  return ;
}
int main(int args, char ** argv){
  int branch_case = 0;
  int ** mtrx;
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

  while (1)
  {
    branch_case = Print_Interface();
    int gen;
    int child_pros_num=0;
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
      Seq_pros(mtrx,row,col,gen);
      end = clock();
      printf(" << Seq_process Completed! >>\n\n");
      Print_RunningTime(start,end);
      Continue_to_next();
      break;
    case 3: /* Case - Process 병렬 처리 */
      while (1)
      {
        printf("Enter #(Child Process) >> ");
        scanf("%d", &child_pros_num);
        if (child_pros_num <= 0){
          printf("Error - Unexpected child Process\n");
          printf("Retry it\n");
        }
        else{
          break;
        }
      }
      system("clear");
      Parallel_pros(mtrx,row,col,gen,child_pros_num);
      end = clock();
      printf(" << parallel_process Completed! >>\n\n");
      Print_RunningTime(start,end);
      Continue_to_next();
      break;
    case 4: /* Case - Thread 병렬 처리 */
      system("clear");
      //Parallel_Thread(mtrx,row,col,gen);
      break;
    default:
      break;
    }
  }




  return 0;
}