#include <signal.h>
#include "FanzaiIPC.h"

/**
 * @brief  服务的信号 handler 函数类型
 * @note   3 个参数: 指向共享内存区段的指针, 客户端 pid, 信号类型
 * @retval
 */
typedef int (*ServiceSignalHandler)(char *, pid_t, FANZAI_SIGNAL_TYPE);

// 与某一客户端通信用的共享内存结构
typedef struct sm {
  int id;            // 共享内存文件描述符
  char *buf;         // 指向共享内存区段的指针
  int bufferLength;  // 共享内存长度
} Shmem;

// 保存共享内存文件与对应客户端 Shmem 的 map 类型
typedef map<string, Shmem> ServiceShmemMap;

class FanzaiIPCService {
 private:
  string serviceName;  // 服务名称
  pid_t servicePid;    // 服务 PID
  ServiceShmemMap ssm;  //保存共享内存文件与对应客户端 Shmem 的 map
  ServiceSignalHandler serviceSignalHandler;  // 信号 handler
  RawSigactionHandler rawHandler;             // 用于 sigaction 的 handler
  /**
   * @brief  关闭连接
   * @note
   * @param  clientPid: 客户端 PID
   * @retval
   */
  int closeConnection(pid_t clientPid);

 public:
  /**
   * @brief  构造函数
   * @note
   * @param  serviceName: 服务名称
   * @param  servicePid: 服务 PID
   * @retval
   */
  FanzaiIPCService(string serviceName, pid_t servicePid);
  /**
   * @brief  更新信号 handler
   * @note
   * @param  newHandler: 新的信号 handler
   * @retval None
   */
  void updateHandler(ServiceSignalHandler newHandler);
  /**
   * @brief  更新用于 sigaction 的 handler
   * @note
   * @param  handler: 新的 handler
   * @retval None
   */
  void setRawHandler(RawSigactionHandler rsh);
  /**
   * @brief  包装后的信号处理函数
   * @note   与用于 sigaction 的 handler 参数相同
   * @param  signum:
   * @param  *info:
   * @param  *context:
   * @retval None
   */
  void wrappedServiceSignalHandler(int signum, siginfo_t *info, void *context);
  /**
   * @brief  向客户端发送 signal 通知客户端处理数据
   * @note
   * @param  clientPid: 客户端 PID
   * @param  signalType: 信号类型
   * @retval None
   */
  void signalClient(pid_t clientPid, FANZAI_SIGNAL_TYPE signalType);

  ~FanzaiIPCService();
};
