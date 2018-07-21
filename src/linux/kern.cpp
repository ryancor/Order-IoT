#if __unix__
  #include <iostream>
  #include <stdio.h>
  #include <stdlib.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <sys/mman.h>
  #include <sys/types.h>
  #include <sys/ioctl.h>

  #include "../../driver/query_ioctl.h"
  #include "../../include/kern.hpp"

  using namespace std;

  int qd_fd;
  query_arg_t q;

  void read_sys() {
    if(getuid() == 0) {
      int configfd;
      int *virt_addr, offset;
      unsigned int *kern_addr;

      configfd = open(KERNEL_BUS_FILE, O_RDWR);
      if(configfd < 0) {
        perror("open");
      }

      // get virtual memory address in sys/kernel using drivers kernel bus to userland
      virt_addr = (int*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, configfd, 0);
      if(virt_addr == MAP_FAILED) {
        perror("mmap");
      }
      offset = ((*virt_addr % 4) + 3) << 9;

      printf("\n\nRetrieving virtual memory address from kernel bus at a mapping size of %d/kb\n\n\n", offset);

      // get kernel memory address
      kern_addr = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, configfd, PAGE_SIZE);
      if(kern_addr == MAP_FAILED) {
        perror("mmap");
      }

      if((kern_addr[0] != 0xdead0000) || (kern_addr[1] != 0xbeef0000)
        || (kern_addr[PAGE_SIZE/sizeof(int)-2] != (0xdead0000+PAGE_SIZE/sizeof(int)-2))
        || (kern_addr[PAGE_SIZE/sizeof(int)-1] != (0xbeef0000+PAGE_SIZE/sizeof(int)-1)))
      {
        printf("0x%x 0x%x\n", kern_addr[0], kern_addr[1]);
        printf("0x%x 0x%x\n", kern_addr[PAGE_SIZE/sizeof(int)-2], kern_addr[PAGE_SIZE/sizeof(int)-1]);
      }

      close(configfd);
    }
  }

  void set_items_to_kern(float total, char *ordered) {
    qd_fd = open(DRIVER_FILE, O_RDWR);

    if (qd_fd == -1) {
      perror("main query_driver open");
    }

    q.price = total;
    q.order = ordered;

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
