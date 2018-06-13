#ifndef MAIN_HPP
#define MAIN_HPP

extern "C" void show();

typedef struct {
  std::string food_1;
  int price_1;
} split_items;

std::string pick_food(int);
double pick_price(int);

#if __unix__
  #include "../driver/query_ioctl.h"

  char *file_name = "/dev/query_driver";
  int fd;
  query_arg_t q;
#endif

#endif
