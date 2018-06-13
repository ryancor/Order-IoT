#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H
#include <linux/ioctl.h>

typedef struct {
  int size_of_all;
  char *order;
  int price;
} query_arg_t;

// _IOW(int type, int number, data_type) - similar to _IOR (read),
// but used to write data to the driver.
#define QUERY_GET_VARIABLES _IOR('q', 1, query_arg_t *)
#define QUERY_SET_VARIABLES _IOW('q', 3, query_arg_t *)

#endif
