#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

void Print_matrix(int **matrix, int row, int col)
{
  for (int i = 1; i <= row; i++)
  {
    for (int j = 1; j <= col; j++)
      printf("%d ", *(*(matrix + i) + j));
    printf("\n");
  }
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
      fprintf(fp, "%d ", matrix_pre[i][j]);
    }
    fprintf(fp, "\n");
  }

  free(file_name);
  fclose(fp);
}
// Process병렬처리
void Parallel_pros(int **matrix, int row, int col, int gen)
{
}
// Thread병렬처리
void Parallel_Thread(int **matrix, int row, int col, int gen)
{
}