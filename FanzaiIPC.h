#include <signal.h>
#include <map>
#include <string>

#define FANZAI_SIGNAL SIGUSR1
#define SERVICE_MAP_FILE_LOCATION "/home/fanzai/FanzaiServiceMap.dat"

#define FANZAI_PARAMS_LENGTH 64 + 16

#define FANZAI_COMMUNICATION 0
#define FANZAI_CLOSE_CONNECTION -1

using namespace std;

typedef map<string, pid_t> FanzaiProcessMap;
typedef void (*RawSigactionHandler)(int, siginfo_t*, void*);

class FanzaiIPC {
 private:
 public:
  static int createShmemFd(string name, int length);
  static char* createShmemBuf(int fd, int length);
  static void munmapBuf(void* buf, int length);
  static void unlinkShmem(string name);
  static FanzaiProcessMap readMapFromFile(string mapFile);
  static int writeMapToFile(FanzaiProcessMap newMap, string mapFile);
  static int insertProcessToMap(string name, pid_t pid, string mapFile);
  static int removeProcessFromMap(string name, string mapFile);
  static pid_t getPidByName(string name, string mapFile);
};
