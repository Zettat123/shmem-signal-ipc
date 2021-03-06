#include "FanzaiIPC.h"

/**
 * @brief  客户端的信号 handler 函数类型
 * @note   1 个参数,指向共享内存区段的指针
 * @retval
 */
typedef void (*ClientSignalHandler)(char *);

class FanzaiIPCClient {
 private:
  string serviceName;                       // 服务名称
  pid_t clientPid;                          // 客户端 PID
  pid_t servicePid;                         // 服务 PID
  int bufferLength;                         // 共享内存长度
  int shmemID;                              // 共享内存标识符
  char *shmemBuf;                           // 指向共享内存区段的指针
  ClientSignalHandler clientSignalHandler;  // 信号 handler
  RawSigactionHandler rawHandler;           // 用于 sigaction 的 handler
  int removeShmem();

 public:
  /**
   * @brief  构造函数
   * @note
   * @param  serviceName: 服务名称
   * @param  clientPid: 客户端 PID
   * @param  bufferLength: 共享内存长度
   * @retval
   */
  FanzaiIPCClient(string serviceName, pid_t clientPid, int bufferLength);
  /**
   * @brief  获取共享内存
   * @note
   * @retval 指向共享内存区段的指针
   */
  char *getShmemBuf();
  /**
   * @brief  更新信号 handler
   * @note
   * @param  newHandler: 新的信号 handler
   * @retval None
   */
  void updateHandler(ClientSignalHandler newHandler);
  /**
   * @brief  更新用于 sigaction 的 handler
   * @note
   * @param  handler: 新的 handler
   * @retval None
   */
  void setRawHandler(RawSigactionHandler handler);
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
   * @brief  建立连接
   * @note
   * @retval None
   */
  void establishConnection();
  /**
   * @brief  向服务发送 signal 通知服务处理数据
   * @note
   * @retval None
   */
  void signalService();
  /**
   * @brief  关闭连接
   * @note
   * @retval None
   */
  void closeConnection();

  ~FanzaiIPCClient();
};
