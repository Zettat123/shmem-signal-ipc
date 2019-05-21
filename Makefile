all: client server

fanzai_ipc.o: fanzai_ipc.c fanzai_ipc.h
	gcc -c fanzai_ipc.c -o $@

client: client.c fanzai_ipc.o
	gcc -o $@ client.c fanzai_ipc.o -lrt

server: server.c fanzai_ipc.o
	gcc -o $@ server.c fanzai_ipc.o -lrt


clean:
	rm client server *.o
