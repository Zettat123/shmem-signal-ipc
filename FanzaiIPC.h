#include <map>
#include <string>

#define FANZAI_SIGNAL SIGUSR1
#define MAP_FILE_LOCATION "/home/fanzai/FanzaiServiceMap.dat"

using namespace std;

typedef map<string, pid_t> FanzaiServiceMap;

class FanzaiIPC {
 private:
 public:
  static FanzaiServiceMap readMapFromFile();
  static int writeMapToFile(FanzaiServiceMap newMap);
}
