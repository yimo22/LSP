#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

char input[1024];
int input_Pos = 0;
char buff[1024];
int buff_Pos = 0;

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
int perm(char * buff){
  int result=0;
  // case - 문자로 지정하는 경우
  if(strlen(buff)==9){
    for(int i=0;i<3;i++){
    if(buff[3*i] == 'r'){
      result |= (S_IRUSR >> i*3);
    }
    if(buff[3*i+1] == 'w'){
      result |= (S_IWUSR >> i*3);
    }
    if(buff[3*i+2] == 'x'){
      result |= (S_IXUSR >> i*3);
    }
    }
  }
  // case - 8진법으로 지정하는 경우
  else if(strlen(buff)==3){
    result = strtol(buff,NULL,8);
  }
  return result;
}

int main(){
  struct stat st;
  int mode = 0;
  gets(input);
  lex();

  if(!strcmp(buff,"mychmod")){
    // get next token
    lex();
    mode = perm(buff);

    lex();
  if(lstat(buff,&st) < 0){
    perror(buff);
    exit(1);
  }
    chmod(buff,mode);
  }


  return 0;
}