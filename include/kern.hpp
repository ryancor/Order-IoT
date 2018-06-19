#if __unix__
  #include "../driver/query_ioctl.h"
  
  char *file_name = "/dev/query_driver";
  int qd_fd;
  query_arg_t q;

  void read_sys();
#endif
