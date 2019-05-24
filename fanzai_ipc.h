#include <map>

#define FANZAI_SIGNAL SIGUSR1
#define MAP_FILE_LOCATION "/home/FanzaiServiceMap.dat"

using namespace std;

typedef map<char*, pid_t> FanzaiServiceMap;

int create_shm_fd(char* name, int size);
void* create_shm_buf(int length, int fd);
