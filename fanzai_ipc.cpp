#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
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

int read_map_from_file() {
  ifstream ins(MAP_FILE_LOCATION);
  FanzaiServiceMap resultMap;
  while (!ins.eof()) {
    char* key;
    pid_t value;
    ins >> key >> value;
    resultMap.insert(make_pair(key, value));
  }

  ins.close();

  return 0;
}

int write_map_to_file(FanzaiServiceMap newMap) {
  ofstream outs(MAP_FILE_LOCATION);
  FanzaiServiceMap::iterator it = newMap.begin();
  for (it; it != newMap.end(); it++) {
    outs << it->first << " " << it->second << endl;
  }

  outs.close();

  return 0;
}

int update_service_map(char* serviceName, pid_t pid) {
  FanzaiServiceMap fsm = read_map_from_file();

  return 0;
}