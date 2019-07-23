objects = FanzaiIPC.o FanzaiIPCService.o FanzaiIPCClient.o sndcapture.o sndplay.o
all: libfanzaiipc.a

libfanzaiipc.a: $(objects)
	ar -rc $@ $^

$(objects): %.o: %.cpp
	g++ --std=c++11 -c $^ -o $@ -lrt -lasound

clean:
	rm -f *.o *.a
