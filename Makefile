all: client chardev_service

client: client.cpp
	g++ --std=c++11 -o $@ -I./libs -L./libs client.cpp -lfanzaiipc -lrt 

chardev_service: chardev_service.cpp
	g++ --std=c++11 -o $@ -I./libs -L./libs chardev_service.cpp -lfanzaiipc -lrt 

clean:
	rm client chardev_service
