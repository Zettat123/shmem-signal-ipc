#include <map>
#include <string>

#define FANZAI_SIGNAL SIGUSR1
#define MAP_FILE_LOCATION "/home/fanzai/FanzaiServiceMap.dat"

using namespace std;

typedef map<string, pid_t> FanzaiServiceMap;

int create_shm_fd(char* name, int size);
void* create_shm_buf(int length, int fd);
int remove_service_from_map(char* serviceName);
int update_service_map(string serviceName, pid_t pid);
