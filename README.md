# Fanzai IPC

基于共享内存和信号的进程间通信机制

**使用之前需先执行以下命令**

```
sudo mkdir /home/fanzai
sudo chmod 777 /home/fanzai
```

## For Service - FanzaiIPCService

供服务端使用

## For Client - FanzaiIPCClient

供客户端使用

## Examples

### 从 service 中读取数据
`chardev_service.cpp`: 接收客户端请求,向提供的内存区域中写入指定数量的随机字符[a-z]

`read_client.cpp`: 从 chardev_service 中读取数据,接收 3 个命令行参数:第 1 个参数为读取次数,第 2 个参数为每次读取字符个数,第 3 个参数为是否打印接收的字符串


### 向 service 写入数据
`print_service.cpp`: 接收客户端发送的字符串并打印

`write_client.cpp`: 向 print_service 写入数据,接收2个命令行参数:第 1 个参数为写入次数,第 2 个参数为每次写入的字符串长度

## How to run examples?
```
make
./chardev_service &
./read_client 5 8 1
```
