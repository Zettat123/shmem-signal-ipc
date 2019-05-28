#include <map>
#include <string>

#define FANZAI_SIGNAL SIGUSR1
#define SERVICE_MAP_FILE_LOCATION "/home/fanzai/FanzaiServiceMap.dat"
#define CLIENT_MAP_FILE_LOCATION "/home/fanzai/FanzaiClientMap.dat"

using namespace std;

typedef map<string, pid_t> FanzaiProcessMap;

/**
 * type:
 * 0: No params, params pointer can be ignored.
 * 1: Request needs params.
 */
typedef struct ipc_metadata {
  string clientName;
  int bufferSize;
  char type;
  void* params;
}
IPCMetadata;

class FanzaiIPC {
 private:
 public:
  static int createShmemFd(string name, int size);
  static char* createShmemBuf(int length, int fd);
  static FanzaiProcessMap readMapFromFile(string mapFile);
  static int writeMapToFile(FanzaiProcessMap newMap, string mapFile);
  static int insertProcessToMap(string name, pid_t pid, string mapFile);
  static int removeProcessFromMap(string name, string mapFile);
  static pid_t getPidByName(string name, string mapFile);
};
