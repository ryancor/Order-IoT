#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H
#include <linux/ioctl.h>

typedef struct {
  int price;
} query_arg_t;

#define QUERY_SET_VARIABLES _IOW('q', 3, query_arg_t *)

#endif
