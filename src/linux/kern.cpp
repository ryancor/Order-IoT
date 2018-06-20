#if __unix__
  #include <iostream>
  #include <stdio.h>
  #include <stdlib.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <sys/mman.h>

  #include "../../driver/query_ioctl.h"
  #include "../../include/kern.hpp"

  using namespace std;

  void read_sys() {
    if(getuid() == 0) {
      int configfd;
      int *address, offset;

      configfd = open(KERNEL_FILE, O_RDWR);
      if(configfd < 0) {
        perror("open");
      }

      // get address from vma in sys/kernel
      address = (int*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, configfd, 0);
      if(address == MAP_FAILED) {
        perror("mmap");
      }
      offset = ((*address % 4) + 3) << 9;
      printf("\n\nSending string to kernel at a mapping size of %d/kb\n\n\n", offset);

      close(configfd);
    }
  }

  void set_items_to_kern() {
    qd_fd = open(DRIVER_FILE, O_RDWR);

    if (qd_fd == -1) {
      perror("main query_driver open");
    }

    q.price = total;
    q.order = &ordered[0u]; // converts std::string to char*

    if (ioctl(qd_fd, QUERY_SET_VARIABLES, &q) == -1) {
      perror("main ioctl set");
    }
    close(qd_fd);
  }

  void get_items_from_kern() {
    qd_fd = open(DRIVER_FILE, O_RDONLY);

    if (qd_fd == -1) {
      perror("main query_driver open");
    }

    if(ioctl(qd_fd, QUERY_GET_VARIABLES, &q) == -1) {
      perror("main ioctl get");
    } else {
      std::cout << "Your last order in size: " << q.size_of_all << std::endl;
    }
    close(qd_fd);
  }
#endif
