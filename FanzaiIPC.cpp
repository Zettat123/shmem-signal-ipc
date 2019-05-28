#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <utility>

#include "FanzaiIPC.h"

int FanzaiIPC::createShmemFd(string name, int size) {
  int fd = shm_open(name.data(), O_CREAT | O_RDWR | O_EXCL, 0777);

  if (fd < 0) {
    fd = shm_open(name.data(), O_RDWR, 0777);

    ftruncate(fd, size);
  }

  return fd;
}

char* FanzaiIPC::createShmemBuf(int length, int fd) {
  return (char*)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

FanzaiProcessMap FanzaiIPC::readMapFromFile(string mapFile) {
  FanzaiProcessMap resultMap;
  ifstream ins(mapFile);
  if (!ins) return resultMap;
  while (ins.peek() == EOF) {
    string key;
    pid_t value;
    ins >> key >> value;
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
  if (it != fsm.end()) {
    // Same name service has been running
    return -1;
  }
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
