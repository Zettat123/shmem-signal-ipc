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

FanzaiServiceMap FanzaiIPC::readMapFromFile() {
  FanzaiServiceMap resultMap;
  ifstream ins(MAP_FILE_LOCATION);
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

int FanzaiIPC::writeMapToFile(FanzaiServiceMap newMap) {
  ofstream outs(MAP_FILE_LOCATION);
  FanzaiServiceMap::iterator it = newMap.begin();
  for (it; it != newMap.end(); it++) {
    outs << it->first << " " << it->second << endl;
  }
  outs.close();

  return 0;
}