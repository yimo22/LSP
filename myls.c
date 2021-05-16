#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>

int MYLS_OPT_I = 0, MYLS_OPT_L = 0, MYLS_OPT_T = 0;

char input[1024];
int input_Pos = 0;
char buff[1024];
int buff_Pos = 0;
/* 파일 타입을 리턴 */
char type(mode_t mode)
{
  if (S_ISREG(mode))
    return ('-');
  if (S_ISDIR(mode))
    return ('d');
  if (S_ISCHR(mode))
    return ('c');
  if (S_ISBLK(mode))
    return ('b');
  if (S_ISLNK(mode))
    return ('l');
  if (S_ISFIFO(mode))
    return ('p');
  if (S_ISSOCK(mode))
    return ('s');
}
char *perm(mode_t mode)
{
  int i;
  static char perms[10] = "---------";
  for (i = 0; i < 3; i++)
  {
    if (mode & (S_IRUSR >> i * 3))
      perms[i * 3] = 'r';
    if (mode & (S_IWUSR >> i * 3))
      perms[i * 3 + 1] = 'w';
    if (mode & (S_IXUSR >> i * 3))
      perms[i * 3 + 2] = 'x';
  }
  return perms;
}
void printStat(char *file, char *path, struct stat *st)
{
  printf("%c%s ", type(st->st_mode), perm(st->st_mode));
  printf("%1ld ", st->st_nlink);
  printf("%10s %10s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
  printf("%9ld ", st->st_size);
  printf("%.12s ", ctime(&st->st_mtime) + 4);
}
void Print_list(DIR *dir, struct dirent *ptr, char *path)
{

  struct stat st;
  char *temp_path = (char *)malloc(sizeof(char) * 1024);
  strcpy(temp_path, path);
  strcat(temp_path, "/");
  strcat(temp_path, ptr->d_name);
  if (lstat(temp_path, &st) < 0)
  {
    perror(temp_path);
  }
  // 숨겨진 파일은 표시하지 않음.
  if ((ptr->d_name)[0] == '.')
    return;
  if (MYLS_OPT_I)
    printf("%ld ", ptr->d_ino);

  if (MYLS_OPT_L)
  {
    // 파일경로명 만들기
    printStat(ptr->d_name, temp_path, &st);
  }
  printf("%s\n", ptr->d_name);
}
void list(DIR *dir, struct dirent *ptr, char *path)
{

  if (MYLS_OPT_T)
  {
    // -t 옵션이 지정된 경우
    int NumOfFile = 0;
    struct dirent *arr[1024];
    while ((ptr = readdir(dir)) != NULL)
    {
      arr[NumOfFile] = ptr;
      NumOfFile++;
    }
    // 최근 수정순으로 정렬
    struct stat i_stat, j_stat;
    for (int i = 0; i <= NumOfFile - 1; i++)
    {
      for (int j = i + 1; j < NumOfFile; j++)
      {
        lstat(arr[i]->d_name, &i_stat);
        lstat(arr[j]->d_name, &j_stat);
        if (i_stat.st_mtime < j_stat.st_mtime)
        {
          struct dirent *temp = arr[j];
          arr[j] = arr[i];
          arr[i] = temp;
        }
      }
    }
    for (int i = 0; i < NumOfFile; i++)
      Print_list(dir, arr[i], path);
  }
  else
  {
    while ((ptr = readdir(dir)) != NULL)
    {
      Print_list(dir, ptr, path);
    }
  }
}
void lex()
{ // Token 을 짤라서 buff[] 에 넣어주는 함수
  memset(buff, 0, sizeof(buff));
  buff_Pos = 0;
  while (isspace(input[input_Pos]))
  {
    input_Pos++;
  }
  while (!isspace(input[input_Pos]) && input[input_Pos] != 0)
  {
    buff[buff_Pos] = input[input_Pos];
    buff_Pos++;
    input_Pos++;
  }
}
int main()
{
  char *path = (char *)malloc(sizeof(char) * 1024);
  struct dirent *en = NULL;
  struct stat statbuf;
  DIR *dir = NULL;
  // myls 명령어 확인
  gets(input);
  lex();
  if (!strcmp(buff, "myls")){
    // get current working directory.
    getcwd(path, 1024);
    if ((dir = opendir(path)) == NULL){
      printf("Error - current dir error\n");
      exit(1);
    }

    // Get next Token
    lex();

    // Option 
    // ls -i : inode 출력
    // ls -l : 자세히 출력
    // ls -t : 최근 수정순으로 출력
    if (buff[0] == '-'){
      for (int i = 1; i < buff_Pos; i++){
        switch (buff[i]){
        case 'i':
          MYLS_OPT_I = 1;
          break;
        case 'l':
          MYLS_OPT_L = 1;
          break;
        case 't':
          MYLS_OPT_T = 1;
          break;
        default:
          break;
        }
      }
    }
    // pathname or Filename
    else
    {
      stat(buff, &statbuf);
      // Regular File
      if (S_ISREG(statbuf.st_mode)){
        en = readdir(dir);
        // 해당 파일 이름을 출력
        printf("%s\n",en->d_name);
        return 0;
      }
      else if (S_ISDIR(statbuf.st_mode)){
        // 현재 dir 주소 + 해당 DIR 를 DIR 로  넘겨준다.
        dir = opendir(buff);
        strcat(path, "/");
        strcat(path, buff);
        list(dir, en, path);
        return 0;
      }
    }
    // ls
    list(dir, en, ".");
  }
  free(path);
  closedir(dir);

  return 0;
}
