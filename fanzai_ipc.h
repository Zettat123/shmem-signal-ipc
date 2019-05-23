int create_shm_fd(char* name, int size);
void* create_shm_buf(int length, int fd);

#define FANZAI_SIGNAL SIGUSR1

/**
 * type:
 * 1: char
 * 2: int
 */
typedef struct fanzai_signal_data {
  unsigned int length;
  char type;
  void* buf;
} FanzaiBuffer;