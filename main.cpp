#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include "include/helper.hpp"

using namespace std;

char food[4][100] = {"1. Hamburger : $ 3.25", "2. Grilled-Cheese : $ 2.50",
            "3. Fries : $ 2.00", "4. Soda : $ 1.00"};

typedef struct {
  string food_1;
  int price_1;
} split_items;

string pick_food(int x) {
  split_items f;
  f.food_1 = split_up_item(food[x]) + ", ";
  return f.food_1;
}

int pick_price(int x) {
  split_items f;
  f.price_1 = atof(split_up_money(food[x]));
  return f.price_1;
}

int main() {
  string menu_choice, ordered;
  int food_choice, remove_choice;
  float total = 0.00;

  while(1) {
    std::cout << "Total Price: $" << std::fixed <<
    std::setprecision(2) << total << endl;
    std::cout << "Ordered: " << ordered << endl << endl;

    list_choices();
    getline(cin, menu_choice);

    if(menu_choice == "1") {
      cout << "Food & Drink Choices:" << std::endl;

      for(int i = 0; i < 4; i++) {
        std::cout << food[i] << std::endl;
      }
      cout << std::endl;
    } else if(menu_choice == "2") {
      cout << std::endl << "food>> ";
      std::cin >> food_choice;

      if((food_choice-1) < sizeof(food)/100) {
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

        if((remove_choice-1) < sizeof(food)/100) {
          // 0000000100007148         cmp        esi, 0x0
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
      receipt(total, ordered);
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
