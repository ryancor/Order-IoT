#if __unix__
  #include <iostream>
  #include <stdio.h>
  #include <stdlib.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <assert.h>
  #include <string.h>
  #include <sys/mman.h>
  #include <sys/types.h>
  #include <sys/ioctl.h>

  #include "common.hpp"
  #include "../../driver/query_ioctl.h"
  #include "../../include/kern.hpp"

  using namespace std;

  int qd_fd;
  query_arg_t q;

  enum { BUFFER_SIZE = 4 };

  void read_write_proc() {
    // for orderIOT_mmap proc
    // get kernel memory address
    // get virtual memory address in /proc using drivers kernel bus to userland
    int proc_fd;
    long page_size;
    char *address1, *address2;
    char buf[BUFFER_SIZE];
    uintptr_t paddr;

    page_size = sysconf(_SC_PAGE_SIZE);

    proc_fd = open(KERNEL_BRIDGE_FILE, O_RDWR | O_SYNC);
    if(proc_fd < 0) {
      perror("open");
      assert(0);
    }

    /* mmap twice for double fun. */
    address1 = (char*)mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, proc_fd, 0);
    if(address1 == MAP_FAILED) {
      perror("mmap");
      assert(0);
    }

    address2 = (char*)mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, proc_fd, 0);
    if(address2 == MAP_FAILED) {
      perror("mmap");
      assert(0);
    }

    assert(address1 != address2);

    /* Read and modify memory. */
    assert(!strcmp(address1, "dead"));
    /* vm_fault */
    assert(!strcmp(address2, "dead"));
    /* vm_fault */
    strcpy(address1, "beef");
    /* Also modified. So both virtual addresses point to the same physical address. */
    assert(!strcmp(address2, "beef"));

    // once address1 changes, address2 automatically changes
    assert(!virt_to_phys_user(&paddr, getpid(), (uintptr_t)address1));
    printf("paddr1 = 0x%jx : 0x", (uintmax_t)paddr);
    // convert virtual addrss to hex value
    for(int j = 0; j < 4; j++) {
      /*
        [0x00415714]> x @ r13
        - offset -       0 1  2 3  4 5  6 7  8 9  A B  C D  E F  0123456789ABCDEF
        0x7f4e5d109000  6265 6566 0000 0000 0000 0000 0000 0000  beef.
      */
      printf("%x", (unsigned int)(unsigned char)*(address1+j));
    }
    // char pointer to hex literal
    unsigned int out;
    sscanf(address1, "%x", &out);
    /*
      [0x00415714]> pd 2
      0x0041571e    e87deffeff   call sym.imp.sscanf
      sym.imp.sscanf()
      0x00415723  b  8b542404     mov edx, [rsp+0x4]
      [0x00415714]> dr
      rdx = 0x0000beef
    */
    printf(" == 0x%x0000\n", out);

    /* Check that modifications made from userland are also visible from the kernel. */
    ssize_t r = read(proc_fd, buf, BUFFER_SIZE);
    if(r == -1) {
      assert(0);
    } else {
      assert(!memcmp(buf, "beef", BUFFER_SIZE));
    }

    /* Modify the data from the kernel, and check that the change is visible from userland. */
    ssize_t w = write(proc_fd, "c0de", 4);
    if(w == -1) {
      assert(0);
    } else {
      assert(!strcmp(address1, "c0de"));
      assert(!strcmp(address2, "c0de"));
    }

    /* Cleanup. */
    if (munmap(address1, page_size)) {
      perror("munmap");
      assert(0);
    }

    if (munmap(address2, page_size)) {
      perror("munmap");
      assert(0);
    }
    printf("\n\n");

    close(proc_fd);
  }

  void read_sys() {
    if(getuid() == 0) {
      // for query_vma proc
      int configfd;
      int *virt_addr, offset;

      configfd = open(KERNEL_BUS_FILE, O_RDWR);
      if(configfd < 0) {
        perror("open");
      }

      // get virtual memory address in sys/kernel using drivers kernel bus to userland
      virt_addr = (int *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, configfd, 0);
      if(virt_addr == MAP_FAILED) {
        perror("mmap");
      }
      offset = ((*virt_addr % 4) + 3) << 9;

      printf("\n\nRetrieving virtual memory address from kernel bus at a mapping");
      printf(" size of %d/kb @ 0x%p\n", offset, virt_addr);

      read_write_proc();

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
