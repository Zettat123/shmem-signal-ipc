#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <utility>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#include "FanzaiIPC.h"

// 调试用打印 map
void printMap(FanzaiProcessMap fsm) {
  FanzaiProcessMap::iterator it = fsm.begin();
  cout << "Print map start" << endl;
  for (it; it != fsm.end(); it++) {
    cout << it->first << " " << it->second << endl;
  }
  cout << "Print map finish" << endl;
}


int FanzaiIPC::createShmemID(int length){
  int shmid;
  size_t size = FANZAI_PARAMS_LENGTH + length;
  shmid = shmget((key_t)FANZAI_SHARED_MEMORY_KEY, size, 0666|IPC_CREAT);
  if (shmid < 0) {
      printf("Shmget failed: %d\n",errno);
  }
  //printf("shmid: %d\n",shmid);
  return shmid;
}

char* FanzaiIPC::createShmemBuf(int shmemID){
  char *shm;

  shm = (char*)shmat(shmemID, NULL, 0);
  if ((int)(*shm) == -1) {
      printf("Shmat failed\n");
  }

  return shm;
}

void FanzaiIPC::removeShmemBuf(void* buf){
  if(shmdt(buf) == -1){
    printf("Shmdt failed\n");
  }
}

void FanzaiIPC::removeShmem(int shmemID){
  if(shmctl(shmemID, IPC_RMID, NULL) == -1)
	{
    printf("Shmctl(IPC_RMID) failed: %d\n", errno);
	}
}

FanzaiProcessMap FanzaiIPC::readMapFromFile(string mapFile) {
  FanzaiProcessMap resultMap;
  ifstream ins(mapFile);

  if (!ins) return resultMap;

  string key;
  pid_t value;
  while (ins >> key >> value) {
    resultMap.insert(make_pair(key, value));
  }
  ins.close();

  return resultMap;
}

int FanzaiIPC::writeMapToFile(FanzaiProcessMap newMap, string mapFile) {
  ofstream outs(mapFile);
  FanzaiProcessMap::iterator it = newMap.begin();
  for (it; it != newMap.end(); it++) {
    outs << it->first << " " << it->second << endl;
  }
  outs.close();

  return 0;
}

int FanzaiIPC::insertProcessToMap(string name, pid_t pid, string mapFile) {
  FanzaiProcessMap fsm = FanzaiIPC::readMapFromFile(mapFile);

  FanzaiProcessMap::iterator it = fsm.find(name);
  // TODO: 处理重名服务的情况
  fsm[name] = pid;

  FanzaiIPC::writeMapToFile(fsm, mapFile);

  return 0;
}

int FanzaiIPC::removeProcessFromMap(string name, string mapFile) {
  FanzaiProcessMap fsm = FanzaiIPC::readMapFromFile(mapFile);
  fsm.erase(name);
  FanzaiIPC::writeMapToFile(fsm, mapFile);

  return 0;
}

pid_t FanzaiIPC::getPidByName(string name, string mapFile) {
  FanzaiProcessMap fsm = FanzaiIPC::readMapFromFile(mapFile);

  FanzaiProcessMap::iterator it = fsm.find(name);
  if (it == fsm.end()) {
    throw "No this service";
  }

  return it->second;
}
