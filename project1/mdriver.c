#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
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

const int dir[DIRECTION][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

typedef struct MultiArgu
{
  int **matrix_pre;
  int **matrix_cur;
  int row;
  int col;
  int thread_row_start;
  int thread_row_end;
} MultiArgu;
void Reset_MultiArgu(MultiArgu *ptr, int **pre, int **cur, int row, int col, int start, int end)
{
  ptr->matrix_pre = pre;
  ptr->matrix_cur = cur;
  ptr->row = row;
  ptr->col = col;
  ptr->thread_row_start = start;
  ptr->thread_row_end = end;
  return;
}
void Print_matrix(int **matrix, int row, int col)
{
  for (int i = 1; i <= row; i++)
  {
    for (int j = 1; j <= col; j++)
      printf("%d ", *(*(matrix + i) + j));
    printf("\n");
  }
}
void MakingOutputFile(int **matrix, int row, int col, int gen, char *filename)
{
  // 결과값을 새로운 파일 "output.matrix"에 생성
  FILE *fp;
  char *file_name = (char *)malloc(sizeof(char) * _MAX_FILENAME_);
  if (filename == NULL)
    sprintf(file_name, "%s%d%s", "./gen_", gen, ".matrix");
  else
    strcpy(file_name, filename);
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
    // export result
    if (depth < gen)
      MakingOutputFile(matrix_pre, row, col, depth, NULL);

    if (depth == gen)
      MakingOutputFile(matrix_pre, row, col, depth, "output.matrix");
  }
  free(matrix_cur);
}
// Process병렬처리
void Parallel_pros(int **matrix_pre, int row, int col, int gen, int child_pros_num)
{
  /*
     IPC-shared memory 이용한 fork() code 수정 필요.
     
  */
  pid_t cur_pid;
  const int ZERO = 0;
  const int _COLS_PER_ROW_ = col + 2;
  int row_start, row_end;
  int cnt = 0;
  void *shmaddr;
  const int _SHMR_MEM_SIZE_ = sizeof(int) * (row + 2) * (col + 2);
  const int _SHMR_KEY_NUM_ = ftok("./mdriver.c", 1);
  // IPC - Shared memory
  // mtrix_pre 와 matrix_cur 를 위한 공유메모리 2 * _SHMR_MEM_SIZE_ 할당
  int shmid = shmget(_SHMR_KEY_NUM_, _SHMR_MEM_SIZE_ * 2, IPC_CREAT | 0666);
  if (shmid == -1)
  {
    perror("shmid failed");
    exit(1);
  }
  if ((shmaddr = shmat(shmid, (void *)0, 0)) == (void *)-1)
  {
    perror("shmat failed");
    exit(1);
  }
  // matrix_pre 를 공유 메모리에 올림.
  for (int i = 0; i <= row + 1; i++)
  {
    for (int j = 0; j <= col + 1; j++)
    {
      if (i == row + 1 || i == 0 || j == 0 || j == col + 1)
        memcpy((int *)shmaddr + i * _COLS_PER_ROW_ + j, &ZERO, sizeof(int));
      else
        memcpy((int *)shmaddr + i * _COLS_PER_ROW_ + j, *(matrix_pre + i) + j, sizeof(int));
    }
  }
  // matrix_cur 을 공유 메모리에 올림.
  for (int i = 0; i <= row + 1; i++)
  {
    for (int j = 0; j <= col + 1; j++)
    {
      memcpy((int *)shmaddr + (row + 2) * (col + 2) + i * _COLS_PER_ROW_ + j, &ZERO, sizeof(int));
    }
  }
  while (gen > cnt)
  {
    for (int i = 0; i < child_pros_num; i++)
    {
      if ((cur_pid = fork()) < 0)
      {
        fprintf(stderr, "Error - fork() error\n");
        exit(1);
      }
      else if (cur_pid > 0)
      {
        /* This is parent process */
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
            row_start = i * (row / child_pros_num) + (i + row % child_pros_num) % child_pros_num;
            row_end = row_start + 1;
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
              if (*(child_shmaddr + temp_x * _COLS_PER_ROW_ + temp_y) == 1)
                cnt++;
            }
            if (*(child_shmaddr + i * _COLS_PER_ROW_ + j) != 1)
            {
              /* 현재 if 문은 죽은 세포일 경우임 */
              if (cnt == 4)
              {
                *(child_shmaddr + (row + 2) * (col + 2) + i * _COLS_PER_ROW_ + j) = 1;
              }
              else
              {
                *(child_shmaddr + (row + 2) * (col + 2) + i * _COLS_PER_ROW_ + j) = 0;
              }
            }
            else
            {
              /* 현재 else 문은 살아있는 세포일 경우임 */
              if (2 < cnt && cnt < 7)
              {
                *(child_shmaddr + (row + 2) * (col + 2) + i * _COLS_PER_ROW_ + j) = 1;
              }
              else
              {
                *(child_shmaddr + (row + 2) * (col + 2) + i * _COLS_PER_ROW_ + j) = 0;
              }
            }
          }
        }
        // delete shm
        if (shmdt(shmaddr) == -1)
        {
          printf("shmdt failed\n");
          exit(1);
        }
        exit(0);
      }
    }
    cnt++;
    int status;
    int *parent_shmaddr = (int *)shmaddr;
    // 자식 프로세서가 모두 종료될때까지 wait
    while ((cur_pid = wait(&status) > 0))
      ;

    // matrix_pre갱신
    for (int i = 0; i < row + 2; i++)
    {
      for (int j = 0; j < col + 2; j++)
      {
        *(parent_shmaddr + i * _COLS_PER_ROW_ + j) = *(parent_shmaddr + (row + 2) * (col + 2) + i * _COLS_PER_ROW_ + j);
        *(parent_shmaddr + (row + 2) * (col + 2) + i * _COLS_PER_ROW_ + j) = 0;
      }
    }

    // 해당 세대수가 되면 해당 matrix를 file로 out.
    int **matrix_ptr = (int **)malloc(sizeof(int *) * (row + 2));
    for (int i = 0; i < row + 2; i++)
    {
      *(matrix_ptr + i) = (int *)malloc(sizeof(int *) * (col + 2));
      for (int j = 0; j < col + 2; j++)
      {
        *(*(matrix_ptr + i) + j) = *(parent_shmaddr + i * _COLS_PER_ROW_ + j);
      }
    }
    if (cnt < gen)
      MakingOutputFile(matrix_ptr, row, col, cnt, NULL);

    if (cnt == gen)
      MakingOutputFile(matrix_ptr, row, col, cnt, "output.matrix");
  }
  // delete shm
  if (shmdt(shmaddr) == -1)
  {
    printf("shmdt failed\n");
    exit(1);
  }
}

void Thread_processing(void *Multi_arg)
{
  MultiArgu *ptr = (MultiArgu *)Multi_arg;
  int cnt = 0;
  for (int i = ptr->thread_row_start + 1; i <= ptr->thread_row_end + 1; i++)
  {
    for (int j = 1; j <= (ptr->col); j++)
    {
      cnt = 0;
      for (int k = 0; k < DIRECTION; k++)
      {
        int temp_x = i + dir[k][VALUE_OF_X];
        int temp_y = j + dir[k][VALUE_OF_Y];
        // Find alived cell around neighbor cell
        if (ptr->matrix_pre[temp_x][temp_y] == 1)
          cnt++;
      }
      if (ptr->matrix_pre[i][j] != 1)
      {
        /* 현재 if 문은 죽은 세포일 경우임 */
        if (cnt == 4)
        {
          ptr->matrix_cur[i][j] = 1;
        }
        else
        {
          ptr->matrix_cur[i][j] = 0;
        }
      }
      else
      {
        /* 현재 else 문은 살아있는 세포일 경우임 */
        if (2 < cnt && cnt < 7)
        {
          ptr->matrix_cur[i][j] = 1;
        }
        else
        {
          ptr->matrix_cur[i][j] = 0;
        }
      }
    }
  }
}
// Thread병렬처리
void Parallel_Thread(int **matrix_pre, int row, int col, int gen, int Thread_pros_num)
{
  pthread_t tid[Thread_pros_num];
  MultiArgu *ptr[Thread_pros_num];
  int remainder = row % Thread_pros_num;
  int row_start = 0, row_end = 0;
  int cnt = 0;
  int **matrix_cur = (int **)malloc(sizeof(int *) * (row + 2));
  for (int i = 0; i < row + 2; i++)
  {
    *(matrix_cur + i) = (int *)malloc(sizeof(int) * (col + 2));
  }
  // reset matrix_cur
  for (int i = 0; i < row + 2; i++)
    for (int j = 0; j < col + 2; j++)
      matrix_cur[i][j] = 0;
  while (cnt < gen)
  {
    for (int i = 0; i < Thread_pros_num; i++)
    {
      ptr[i] = (MultiArgu *)malloc(sizeof(MultiArgu));
      if (remainder != 0)
      {
        // 균등하게 분배되지 않는 경우
        if (i + row % Thread_pros_num >= Thread_pros_num)
        {
          row_start = i * (row / Thread_pros_num) + (i + row % Thread_pros_num) % Thread_pros_num;
          row_end = row_start + 1;
        }
        else
        {
          row_start = i * (row / Thread_pros_num);
          row_end = (i + 1) * (row / Thread_pros_num) - 1;
        }
      }
      else
      { // 균등하게 분배되는 경우
        row_start = i * (row / Thread_pros_num);
        row_end = (i + 1) * (row / Thread_pros_num) - 1;
      }
      Reset_MultiArgu(ptr[i], matrix_pre, matrix_cur, row, col, row_start, row_end);
      int state = pthread_create(&tid[i], NULL, (void *)Thread_processing, (void *)ptr[i]);
      if (state != 0)
      {
        perror("pthread_create error");
        exit(1);
      }
    }
    // Thread가 끝날때까지 대기
    for (int i = 0; i < Thread_pros_num; i++)
      pthread_join(tid[i], NULL);
    // 실행한 gen 증가
    cnt++;
    // Main Thread - update matrix_cur && matirx_pre
    for (int i = 0; i <= row + 1; i++)
    {
      for (int j = 0; j <= col + 1; j++)
      {
        matrix_pre[i][j] = matrix_cur[i][j];
        matrix_cur[i][j] = 0;
      }
    }
    // Exprot file
    if (cnt < gen)
      MakingOutputFile(matrix_pre, row, col, cnt, NULL);

    if (cnt == gen)
      MakingOutputFile(matrix_pre, row, col, cnt, "output.matrix");
  }
}


