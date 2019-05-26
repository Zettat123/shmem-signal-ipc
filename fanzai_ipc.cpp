#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <utility>

#include "fanzai_ipc.h"

int create_shm_fd(char* name, int size) {
  int fd = shm_open(name, O_CREAT | O_RDWR | O_EXCL, 0777);

  if (fd < 0) {
    fd = shm_open(name, O_RDWR, 0777);

    ftruncate(fd, size);
  }

  return fd;
}

void* create_shm_buf(int length, int fd) {
  return mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void printMap(FanzaiServiceMap fsm) {
  FanzaiServiceMap::iterator it = fsm.begin();
  cout << "Print map start" << endl;
  for (it; it != fsm.end(); it++) {
    cout << it->first << " " << it->second << endl;
  }
  cout << "Print map finish" << endl;
}

FanzaiServiceMap read_map_from_file() {
  FanzaiServiceMap resultMap;
  ifstream ins(MAP_FILE_LOCATION);
  if (!ins) return resultMap;
  int i = 0;
  while (ins.peek() == EOF) {
    string key;
    pid_t value;
    ins >> key >> value;
    cout << i++ << " " << key << " " << value << endl;
    resultMap.insert(make_pair(key, value));
  }

  ins.close();

  return resultMap;
}

int write_map_to_file(FanzaiServiceMap newMap) {
  ofstream outs(MAP_FILE_LOCATION);
  FanzaiServiceMap::iterator it = newMap.begin();
  int i = 0;
  for (it; it != newMap.end(); it++) {
    cout << i++ << " " << it->first << " " << it->second << endl;
    outs << it->first << " " << it->second << endl;
  }

  outs.close();

  return 0;
}

int update_service_map(string serviceName, pid_t pid) {
  FanzaiServiceMap fsm = read_map_from_file();
  printMap(fsm);
  fsm[serviceName] = pid;
  printMap(fsm);
  write_map_to_file(fsm);

  return 0;
}

int remove_service_from_map(string serviceName) {
  FanzaiServiceMap fsm = read_map_from_file();
  fsm.erase(serviceName);
  printMap(fsm);
  write_map_to_file(fsm);

  return 0;
}
