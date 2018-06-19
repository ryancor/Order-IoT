#if __unix__
  #include <stdio.h>
  #include <stdlib.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <sys/mman.h>

  #define PAGE_SIZE     4096
  #define KERNEL_FILE   "/sys/kernel/debug/query_vma"

  void read_sys() {
    if(getuid() == 0) {
      int configfd;
      int *address;

      configfd = open(KERNEL_FILE, O_RDWR);
      if(configfd < 0) {
        perror("open");
      }

      address = (int*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, configfd, 0);
      if(address == MAP_FAILED) {
        perror("mmap");
      }

      printf("\n\nSending to kernel at map size of %d\n\n\n", address);

      close(configfd);
    }
  }
#endif
