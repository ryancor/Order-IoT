#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#ifdef __unix__
#include <stdlib.h>
#endif

#include "../include/helper.hpp"
#include "../include/requests.hpp"
#include "../include/sql.hpp"

using namespace std;

extern "C" void show();

typedef struct {
  string food_1;
  int price_1;
} split_items;

string pick_food(int x) {
  split_items f;
  f.food_1 = split_up_item(DataGet()[x]) + ", ";
  return f.food_1;
}

double pick_price(int x) {
  split_items f;
  f.price_1 = atof(split_up_char(DataGet()[x], 2));
  return f.price_1;
}

int main() {
  string menu_choice, ordered;
  unsigned long cust_id = customer_id();
  int food_choice, remove_choice;
  float total = 0.00;

  #ifdef __unix__
  #include "../lib/pal.hpp"

  if(isPalindrome(user_request()) == 1) {
    std::cout << "You have a very special name ;)" << std::endl;
  }

  #endif

  show();
  std::cout << user_request() << std::endl;

  if(open_or_closed() == 1) {
    std::cout << "Open: " << "true" << std::endl;
  } else {
    std::cout << "Open: " << "false" << std::endl;
  }

  std::cout << "Customer ID: #" << cust_id << std::endl << std::endl;

  while(1) {
    std::cout << "Total Price: $" << std::fixed <<
    std::setprecision(2) << total << endl;
    std::cout << "Ordered: " << ordered << endl << endl;

    list_choices();
    getline(cin, menu_choice);

    if(menu_choice == "1") {
      cout << "Food & Drink Choices:" << std::endl;

      for(int i = 0; i < DataGet().size(); i++) {
        std::cout << DataGet()[i] << std::endl;
      }
      cout << std::endl;
    } else if(menu_choice == "2") {
      cout << std::endl << "food>> ";
      std::cin >> food_choice;

      if((food_choice-1) < DataGet().size()) {
        std::cout << "Adding " << pick_food(food_choice-1) << " to your order." << endl;
        total += pick_price(food_choice-1);
        ordered += pick_food(food_choice-1);
        clear_cin();
      } else {
        std::cout << "Invalid Choice." << std::endl;
        clear_cin();
      }
    } else if(menu_choice == "3") {
      if(total > 0.01) {
        cout << std::endl << "remove>> ";
        cin >> remove_choice;

        if((remove_choice-1) < DataGet().size()) {
          if(ordered.find(pick_food(remove_choice-1)) != string::npos) {
            std::cout << "Removing " << pick_food(remove_choice-1) << " from your order." << endl;
            total -= pick_price(remove_choice-1);

            size_t pos = ordered.find(pick_food(remove_choice-1));
            ordered.erase(pos, pick_food(remove_choice-1).length());
            clear_cin();
          } else {
            std::cout << "You haven't ordered this item." << std::endl;
            clear_cin();
          }
        } else {
          std::cout << "Invalid Choice." << std::endl;
          clear_cin();
        }
      } else {
        std::cout << "You haven't ordered anything." << std::endl;
      }
    } else if(menu_choice == "4") {
      std::cout << "Storing receipt to text file.." << std::endl << std::endl;
      receipt(cust_id, user_request(), total, ordered);
    } else if(menu_choice == "5") {
      exit(-1);
    } else if(menu_choice == "6") {
      total = couponed_code(total);
    } else {
      std::cout << "Invalid Choice." << std::endl;
    }
  }
  return 0;
}
