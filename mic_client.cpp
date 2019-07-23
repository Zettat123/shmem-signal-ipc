#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "FanzaiIPCClient.h"
#include "sndplay.h"

#define BUFFER_SIZE 2*1024*1024  // 共享内存长度

string SERVICE_NAME = "CHARDEV_SERVICE";  // 要请求的服务名
int running = 0;  // 程序运行标识

FanzaiIPCClient* fic;  // FanzaiIPCClient 对象

//将字符数组转换为long long
long long charArr2LongLong(char *buf){
  long long ret = 0;
  for(int i=0; i<8; i++){
    ret = ret << 8;
    ret += (unsigned char)buf[i];
  }
  return ret;
}

// 信号处理函数  播放音频
void handler(char* rawBuf){
  char* buf = (char*)rawBuf;
  long long snd_size = charArr2LongLong(buf);
  printf("Sound size is: %lld bytes\n",snd_size);
  PlaySound(buf+8, snd_size);

  fic->closeConnection();
  delete fic;
  running = 1;
}

// 用于 sigaction 的 handler
void rawHandler(int signum, siginfo_t* info, void* context) {
  fic->wrappedServiceSignalHandler(signum, info, context);
}

int main(int argc, char* argv[]) {

  fic = new FanzaiIPCClient(SERVICE_NAME, getpid(), BUFFER_SIZE);

  fic->setRawHandler(rawHandler);
  fic->updateHandler(handler);

  char* params = fic->getShmemBuf();
  params[0] = 74;  // 供测试,无意义
  params[1] = 75;  // 供测试,无意义
  params[2] = 78;  // 供测试,无意义
  params[3] = 8;

  fic->establishConnection();
  
  fic->signalService();

  while(running == 0){
    sleep(1);
  }

  return 0;
}