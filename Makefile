all: client chardev_service

FanzaiIPC.o: FanzaiIPC.cpp FanzaiIPC.h
	g++ --std=c++11 -c FanzaiIPC.cpp -o $@

FanzaiIPCClient.o: FanzaiIPCClient.cpp FanzaiIPCClient.h FanzaiIPC.o
	g++ --std=c++11 -c FanzaiIPCClient.cpp FanzaiIPC.o -o $@

FanzaiIPCService.o: FanzaiIPCService.cpp FanzaiIPCService.h FanzaiIPC.o
	g++ --std=c++11 -c FanzaiIPCService.cpp FanzaiIPC.o -o $@	

client: client.cpp FanzaiIPCClient.o
	g++ --std=c++11 -o $@ client.cpp FanzaiIPCClient.o -lrt

chardev_service: chardev_service.cpp FanzaiIPCService.o
	g++ --std=c++11 -o $@ chardev_service.cpp FanzaiIPCService.o -lrt


clean:
	rm client chardev_service *.o
