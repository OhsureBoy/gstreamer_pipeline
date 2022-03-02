#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_client.h"
#include "../util/util.h"
#include <time.h>

void *firstThreadRun();

int main() {
  pthread_t firstThread;
  int threadErr;

  // 쓰레드를 만들고 쓰레드 함수 실행
    if(threadErr = pthread_create(&firstThread,NULL,firstThreadRun,NULL))
    {
        // 에러시 에러 출력
        printf("Thread Err = %d",threadErr);
    }
    
    while(1);

}

// 쓰레드 동작시 실행될 함수
void *firstThreadRun()
{
  char app_version[518] = {};
  char get_update_version[518] = {};
  char real_token[518] = {};

  http_get_token("aaa","bbb",real_token);

    while(1)
    {
      read_device_profile("appversion.cfg",app_version);
      http_get_app_version(real_token,"tlc-bond",get_update_version);
      
      printf("\n\n app_version : %s  get_update_version : %s  \n\n",app_version,get_update_version);
      if(strcmp(app_version,get_update_version) < 0) {
        printf("\n\n Update Start \n\n");

        
        http_get_app_update_file(real_token,"tlc-bond");
        sleep(10);
        system("/home/nvidia/shellscript/update_shell.sh");
      }

      sleep(600);
    }
}
 