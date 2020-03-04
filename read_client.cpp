#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "FanzaiIPCClient.h"

#define BUFFER_SIZE 65536 + 10  // 共享内存长度

string SERVICE_NAME = "CHARDEV_SERVICE";  // 要请求的服务名

FanzaiIPCClient* fic;  // FanzaiIPCClient 对象
int times, size, count;  // 请求次数,每次请求要读取的字符个数,计数器
int printOrNot;          // 是否打印, 1 Yes; 0 No

// 信号处理函数
void handler(char* rawBuf) {
  count++;
  sleep(10);
  if (printOrNot == 1) {
    char* buf = (char*)rawBuf;
    printf("%s\n", buf);
  }
  if (count == times) {
    printf("count == times\n");
    fic->closeConnection();
    delete fic;
  }
}

// 用于 sigaction 的 handler
void rawHandler(int signum, siginfo_t* info, void* context) {
  fic->wrappedServiceSignalHandler(signum, info, context);
}

int main(int argc, char* argv[]) {
  times = atoi(argv[1]);
  size = atoi(argv[2]);
  printOrNot = atoi(argv[3]);
  count = 0;

  fic = new FanzaiIPCClient(SERVICE_NAME, getpid(), BUFFER_SIZE);

  fic->setRawHandler(rawHandler);
  fic->updateHandler(handler);

  char* params = fic->getShmemBuf();
  params[0] = 74;  // 供测试,无意义
  params[1] = 75;  // 供测试,无意义
  params[2] = 78;  // 供测试,无意义
  params[3] =
      size;  // 把每次请求要读取的字符个数放置于共享内存中,服务在建立连接时读取
  fic->establishConnection();

  int i = 0;
  for (i; i < times; i++) {
    fic->signalService();
    sleep(1);
  }

  return 0;
}