all: client chardev_service

fanzai_ipc.o: fanzai_ipc.cpp fanzai_ipc.h
	g++ --std=c++11 -c fanzai_ipc.cpp -o $@

client: client.cpp fanzai_ipc.o
	g++ --std=c++11 -o $@ client.cpp fanzai_ipc.o -lrt

chardev_service: chardev_service.cpp fanzai_ipc.o
	g++ --std=c++11 -o $@ chardev_service.cpp fanzai_ipc.o -lrt


clean:
	rm client chardev_service *.o
