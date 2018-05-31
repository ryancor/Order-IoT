#ifndef MAIN_HPP
#define MAIN_HPP

extern "C" void show();

typedef struct {
  std::string food_1;
  int price_1;
} split_items;

std::string pick_food(int);
double pick_price(int);

#endif
