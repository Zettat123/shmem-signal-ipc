SUBDIRS = libs
MAKE = make
CC = g++ --std=c++11
CLIBS = -L./libs -lfanzaiipc -lrt
INCLUDE_DIRS = -I./libs
TARGETS = read_client chardev_service write_client print_service

all: $(TARGETS) 

ECHO:
	@echo $(SUBDIRS)
	@echo begin compile

$(SUBDIRS): ECHO
	$(MAKE) -C $@

$(TARGETS): %: %.cpp $(SUBDIRS)
	$(CC) -o $@ $< $(INCLUDE_DIRS) $(CLIBS) 

clean:
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir clean || exit 1;\
	done
	rm *_client *_service
