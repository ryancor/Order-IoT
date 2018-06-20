#if __unix__
  #define PAGE_SIZE     4096
  #define KERNEL_FILE   "/sys/kernel/debug/query_vma"
  #define DRIVER_FILE   "/dev/query_driver"

  void read_sys();
  void set_items_to_kern(float, char *);
  void get_items_from_kern();
#endif
