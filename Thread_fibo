#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void * thread1(void * arg);
void * thread2(void*);

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

int count = 0;
int input = 0;
int t1= 0 , t2 = 1;

int main(){
  pthread_t tid1, tid2;
  int status;

  if(pthread_create(&tid1,NULL,thread1,NULL) != 0){
    fprintf(stderr,"create");
    exit(1);
  }
  if(pthread_create(&tid2,NULL,thread2,NULL) != 0){
    fprintf(stderr,"create");
    exit(1);
  }

  while(1){
    printf("enter num\n");
    scanf("%d",&input);

    if(input > 1 ){
      pthread_cond_signal(&cond1);
      break;
    }
  }

  pthread_join(tid1, (void *) & status);
  pthread_join(tid2, (void *) & status);

  printf("completed\n");

  pthread_mutex_destroy(&mutex1);
  pthread_mutex_destroy(&mutex2);
  pthread_cond_destroy(&cond1);
  pthread_cond_destroy(&cond2);
}

void * thread1( void * arg){
  while(1){
    pthread_mutex_lock(&mutex1);

    if(input<2)
      pthread_cond_wait(&cond1,&mutex1);

    if(input == count){
      pthread_cond_signal(&cond2);
      break;
    }

    if(count == 0){
      count++;
      printf("Thread 1 : %d\n",t1);
    }
    else if( count % 2 == 0){
      t1 += t2;
      count++;
      printf("Thread1 : %d\n",t1);
    }

    pthread_cond_signal(&cond2);
    pthread_cond_wait(&cond1,&mutex1);
    pthread_mutex_unlock(&mutex1);
  }
  return NULL;
}
void * thread2( void * arg){
  while(1){
    pthread_mutex_lock(&mutex2);

    if(input<2)
      pthread_cond_wait(&cond2,&mutex2);

    if(input == count){
      pthread_cond_signal(&cond1);
      break;
    }

    if(count == 1){
      count++;
      printf("Thread 2 : %d\n",t2);
    }
    else if( count % 2 ==1){
      t2 += t1;
      count++;
      printf("Thread2 : %d\n",t2);
    }

    pthread_cond_signal(&cond1);
    pthread_cond_wait(&cond2,&mutex2);
    pthread_mutex_unlock(&mutex2);
  }

  return NULL;
}

