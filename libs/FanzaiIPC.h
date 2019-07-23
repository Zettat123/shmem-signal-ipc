#include <signal.h>
#include <map>
#include <string>

#define FANZAI_SIGNAL SIGUSR1
#define SERVICE_MAP_FILE_LOCATION \
  "/home/fanzai/FanzaiServiceMap.dat"  // 默认的服务名称-服务进程号映射文件保存位置

#define FANZAI_SHARED_MEMORY_KEY 8765  // 创建共享内存时的key
#define FANZAI_PARAMS_LENGTH 64 + 16  // 预留参数用长度,目前为 64 字节

#define FANZAI_SIGNAL_TYPE int          // 信号携带的参数
#define FANZAI_COMMUNICATION 0          // 正常通信
#define FANZAI_ESTABLISH_CONNECTION -1  // 连接建立
#define FANZAI_CLOSE_CONNECTION -2      // 连接关闭

using namespace std;

typedef map<string, pid_t> FanzaiProcessMap;  // 服务名称-服务进程号的 map 类型
typedef void (*RawSigactionHandler)(int, siginfo_t*,
                                    void*);  // 用于 sigaction 的 handler 类型

class FanzaiIPC {
 private:
 public:
  /**
   * @brief  创建共享内存
   * @note
   * @param  length: 共享内存的大小
   * @retval 共享内存标识符
   */
  static int createShmemID(int length);
  /**
   * @brief  将共享内存连接到当前进程空间
   * @note
   * @param  shmemID: 共享内存标识符
   * @retval 共享内存首地址
   */
  static char* createShmemBuf(int shmemID);
  /**
   * @brief  将共享内存从当前进程分离
   * @note
   * @param  buf: 共享内存首地址
   */
  static void removeShmemBuf(void* buf);
  /**
   * @brief  删除共享内存
   * @note
   * @param  shmemID: 共享内存标识符
   */
  static void removeShmem(int shmemID);
  /**
   * @brief  从文件中建立服务名称-服务进程号 map
   * @note
   * @param  mapFile: 保存 map 的文件
   * @retval 新的 map
   */
  static FanzaiProcessMap readMapFromFile(string mapFile);
  /**
   * @brief  将服务名称-服务进程号 map 写入文件
   * @note
   * @param  newMap: 要保存的 map
   * @param  mapFile: 要保存为的文件
   * @retval 成功返回 0
   */
  static int writeMapToFile(FanzaiProcessMap newMap, string mapFile);
  /**
   * @brief  将进程添加入 map 中
   * @note
   * @param  name: 服务名称
   * @param  pid: 服务进程号
   * @param  mapFile: 保存 map 的文件
   * @retval 成功返回 0
   */
  static int insertProcessToMap(string name, pid_t pid, string mapFile);
  /**
   * @brief  将进程从 map 中移除
   * @note
   * @param  name: 服务名称
   * @param  mapFile: 保存 map 的文件
   * @retval 成功返回 0
   */
  static int removeProcessFromMap(string name, string mapFile);
  /**
   * @brief  根据服务名称获取服务 PID
   * @note
   * @param  name: 服务名称
   * @param  mapFile: 保存 map 的文件
   * @retval 该服务 PID
   */
  static pid_t getPidByName(string name, string mapFile);
};
