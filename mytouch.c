#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
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
int main(){
  gets(input);
  // Get Token
  lex();
  if( !strcmp(buff,"mytouch")){
    // get next Token
    lex();
    if(access(buff,F_OK) < 0){
      // create Newfile
      FILE * fp = fopen(buff,"w");
      free(fp);
    }
    
    //existed file
    if( access(buff,F_OK) == 0){
      utime(buff,NULL);
    }
  }
  return 0;
}
