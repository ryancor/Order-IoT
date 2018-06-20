#if __unix__
  #include "../driver/query_ioctl.h"

  #define PAGE_SIZE     4096
  #define KERNEL_FILE   "/sys/kernel/debug/query_vma"
  #define DRIVER_FILE   "/dev/query_driver"
  
  int qd_fd;
  query_arg_t q;

  void read_sys();
  void set_items_to_kern();
  void get_items_from_kern();
#endif
