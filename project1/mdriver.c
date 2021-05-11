#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

/*
  alive 세포가
  - 주위에 세포가 3~6개 alived 일경우, 다음세대도 alive.
  - 이외의 경우 다음세대에 세포는 die
  dead 세포가
  - 주위에 세포가 4개 인 경우, 죽은 세포가 다음세대에 alive.
  - 이외의 경우 다음세대에 세포는 die
*/
#define VALUE_OF_X 0
#define VALUE_OF_Y 1
#define DIRECTION 8
#define _MAX_FILENAME_ 1024
#define _SHMR_KEY_NUM_ 20172612

const int dir[DIRECTION][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

void Print_matrix(int **matrix, int row, int col)
{
  for (int i = 1; i <= row; i++)
  {
    for (int j = 1; j <= col; j++)
      printf("%d ", *(*(matrix + i) + j));
    printf("\n");
  }
}
void MakingOutputFile(int ** matrix, int row, int col, int gen){
  // 결과값을 새로운 파일 "output.matrix"에 생성
  FILE *fp;
  char *file_name = (char *)malloc(sizeof(char) * _MAX_FILENAME_);
  sprintf(file_name, "%s%d%s", "./gen_", gen, ".matrix");
  if ((fp = fopen(file_name, "w+")) == NULL)
  {
    fprintf(stderr, "fopen error\n");
    exit(1);
  }
  for (int i = 1; i <= row; i++)
  {
    for (int j = 1; j <= col; j++)
    {
      fprintf(fp, "%d ", matrix[i][j]);
    }
    fprintf(fp, "\n");
  }

  free(file_name);
  fclose(fp);
}
// 순차처리
void Seq_pros(int **matrix_pre, int row, int col, int gen)
{
  int depth = 0;
  int **matrix_cur = (int **)malloc(sizeof(int *) * (row + 2));
  for (int i = 0; i < row + 2; i++)
  {
    *(matrix_cur + i) = (int *)malloc(sizeof(int) * (col + 2));
  }
  // reset matrix_cur
  for (int i = 0; i < row + 2; i++)
    for (int j = 0; j < col + 2; j++)
      matrix_cur[i][j] = 0;

  while (depth < gen)
  {
    int cnt = 0;
    for (int i = 1; i <= row; i++)
    {
      for (int j = 1; j <= col; j++)
      {
        cnt = 0;
        for (int k = 0; k < DIRECTION; k++)
        {
          int temp_x = i + dir[k][VALUE_OF_X];
          int temp_y = j + dir[k][VALUE_OF_Y];
          // Find alived cell around neighbor cell
          if (matrix_pre[temp_x][temp_y] == 1)
            cnt++;
        }
        if (matrix_pre[i][j] != 1)
        {
          /* 현재 if 문은 죽은 세포일 경우임 */
          if (cnt == 4)
            matrix_cur[i][j] = 1;
          else
            matrix_cur[i][j] = 0;
        }
        else
        {
          /* 현재 else 문은 살아있는 세포일 경우임 */
          if (2 < cnt && cnt < 7)
            matrix_cur[i][j] = 1;
          else
            matrix_cur[i][j] = 0;
        }
      }
    }
    // 다음 반복문을 위해 matrix_pre 갱신
    for (int i = 1; i <= row; i++)
    {
      for (int j = 1; j <= col; j++)
      {
        matrix_pre[i][j] = matrix_cur[i][j];
      }
    }
    // depth값 갱신
    depth++;
  }
  free(matrix_cur);
  // export result
  MakingOutputFile(matrix_pre,row,col,gen);
}
// Process병렬처리
void Parallel_pros(int **matrix_pre, int row, int col, int gen, int child_pros_num){
  /*
     IPC-shared memory 이용한 fork() code 수정 필요.
     
  */
  pid_t cur_pid;
  const int ZERO = 0;
  const int _COLS_PER_ROW_ = col + 2;
  int row_start, row_end;
  void * shmaddr;
  const int _SHMR_MEM_SIZE_ = sizeof(int) * (row+2) * (col +2);
  // IPC - Shared memory
  // mtrix_pre 와 matrix_cur 를 위한 공유메모리 2 * _SHMR_MEM_SIZE_ 할당
  int shmid = shmget( _SHMR_KEY_NUM_,_SHMR_MEM_SIZE_ * 2,IPC_CREAT | 0666);
  if(shmid == -1){
    perror("shmid failed");
    exit(1);
  }
  if ((shmaddr = shmat(shmid, (void *)0, 0)) == (void *)-1)
  {
      perror("shmat failed");
      exit(1);
  }
  // matrix_pre 를 공유 메모리에 올림.
  for(int i=0;i<= row + 1;i++){
    for(int j=0;j<=col+1;j++){
      if( i == row+1 || i == 0 || j==0 || j==col+1)
        memcpy((int *) shmaddr + i * _COLS_PER_ROW_ + j, &ZERO, sizeof(int));
      else
        memcpy((int *) shmaddr + i * _COLS_PER_ROW_ + j, *(matrix_pre+i) + j, sizeof(int));
    }
  }  
  // matrix_cur 을 공유 메모리에 올림.
  for(int i=0;i<= row + 1;i++){
    for(int j=0;j<=col+1;j++){
        memcpy((int *) shmaddr + (row+2)*(col+2)  +i * _COLS_PER_ROW_ + j, &ZERO, sizeof(int));
    }
  }  
  
  for (int i = 0; i < child_pros_num; i++)
  {
    if ((cur_pid = fork()) < 0)
    {
      fprintf(stderr, "Error - fork() error\n");
      exit(1);
    }
    else if ( cur_pid > 0){
      /* This is parent process */
      int status;
      int cnt = 0;
      /*
      while(cnt == child_pros_num){
        pid_t temp;
        temp = wait(&status);
        if()
      }*/
      printf("End-Parent");
    }
    else
    {
      /* This is child process */
      // i번째 child가 처리하는 행은 [row_start,row_end] 행임.
      // 이때 0<= row_start, row_end <= row-1 임.
      if ((shmaddr = shmat(shmid, (void *)0, 0)) == (void *)-1)
      {
        perror("shmat failed");
        exit(1);
      }
      int *child_shmaddr = (int *)shmaddr;

      if (row % child_pros_num != 0)
      {
        // 균등하게 분배되지 않는 경우
        if (i + row % child_pros_num >= child_pros_num)
        {
          row_start = i * (row / child_pros_num) + (child_pros_num - row % child_pros_num);
          row_end = row_start + row / child_pros_num;
        }
        else
        {
          row_start = i * (row / child_pros_num);
          row_end = (i + 1) * (row / child_pros_num) - 1;
        }
      }
      else
      { // 균등하게 분배되는 경우
        row_start = i * (row / child_pros_num);
        row_end = (i + 1) * (row / child_pros_num) - 1;
      }
      // child process 는 row_start, row_end를 통해 해당 행을 처리.
      int cnt = 0;
      for (int i = row_start + 1; i <= row_end + 1; i++)
      {
        for (int j = 1; j <= col; j++)
        {
          cnt = 0;
          for (int k = 0; k < DIRECTION; k++)
          {
            int temp_x = i + dir[k][VALUE_OF_X];
            int temp_y = j + dir[k][VALUE_OF_Y];
            // Find alived cell around neighbor cell
            if ( *(child_shmaddr + i * _COLS_PER_ROW_ + j) == 1)
              cnt++;
          }
          if ( *(child_shmaddr + i * _COLS_PER_ROW_ + j) != 1)
          {
            /* 현재 if 문은 죽은 세포일 경우임 */
            if (cnt == 4)
              *(child_shmaddr + (row+2) * (col+2) +i * _COLS_PER_ROW_ + j) = 1;
            else
              *(child_shmaddr + (row+2) * (col+2) +i * _COLS_PER_ROW_ + j) = 0;
          }
          else
          {
            /* 현재 else 문은 살아있는 세포일 경우임 */
            if (2 < cnt && cnt < 7)
              *(child_shmaddr + (row+2) * (col+2) +i * _COLS_PER_ROW_ + j) = 1;
            else
              *(child_shmaddr + (row+2) * (col+2) +i * _COLS_PER_ROW_ + j) = 0;
          }
        }
      }

      // Test for matrix_cur
      for (int i = row_start + 1; i <= row_end + 1; i++){
        for(int j=1;j<=col;j++)
          printf("%d ",*(child_shmaddr + (row+2) * (col+2) +i * _COLS_PER_ROW_ + j));
        printf("\n");
      }

      printf("ChildProcess 종료\n");
      exit(0);
    }
  }

}
// Thread병렬처리
void Parallel_Thread(int **matrix_pre, int row, int col, int gen, int Thread_pros_num)
{

}