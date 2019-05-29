all: client chardev_service

FanzaiIPC.o: FanzaiIPC.cpp FanzaiIPC.h
	g++ --std=c++11 -c FanzaiIPC.cpp -lrt

FanzaiIPCClient.o: FanzaiIPCClient.cpp FanzaiIPCClient.h FanzaiIPC.h
	g++ --std=c++11 -c FanzaiIPCClient.cpp -lrt

FanzaiIPCService.o: FanzaiIPCService.cpp FanzaiIPCService.h FanzaiIPC.h
	g++ --std=c++11 -c FanzaiIPCService.cpp

client: client.cpp FanzaiIPC.o FanzaiIPCClient.o
	g++ --std=c++11 -o $@ client.cpp FanzaiIPC.o FanzaiIPCClient.o -lrt

chardev_service: chardev_service.cpp FanzaiIPC.o FanzaiIPCService.o
	g++ --std=c++11 -o $@ chardev_service.cpp FanzaiIPC.o FanzaiIPCService.o -lrt


clean:
	rm client chardev_service *.o
