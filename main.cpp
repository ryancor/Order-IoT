#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include "helper.hpp"

using namespace std;

typedef struct {
  string food_1;
  string food_2;
  string food_3;
  string food_4;
  int price_1;
  int price_2;
  int price_3;
  int price_4;
} split_items;

int main() {
  string menu_choice, food_choice, remove_choice;
  float total = 0.00;
  string ordered;
  char food[4][100] = {"1. Hamburger : $ 3.25", "2. Grilled-Cheese : $ 2.50",
              "3. Fries : $ 2.00", "4. Soda : $ 1.00"};

  split_items f;
  f.food_1 = split_up_item(food[0]) + ", ";
  f.food_2 = split_up_item(food[1]) + ", ";
  f.food_3 = split_up_item(food[2]) + ", ";
  f.food_4 = split_up_item(food[3]) + ", ";
  f.price_1 = atof(split_up_money(food[0]));
  f.price_2 = atof(split_up_money(food[1]));
  f.price_3 = atof(split_up_money(food[2]));
  f.price_4 = atof(split_up_money(food[3]));

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
      getline(cin, food_choice);

      if(food_choice == "1") {
        std::cout << "Adding " << f.food_1 << " to your order." << endl;
        total += f.price_1;
        ordered += f.food_1;
      } else if(food_choice == "2") {
        std::cout << "Adding " << f.food_2 << " to your order." << endl;
        total += f.price_2;
        ordered += f.food_2;
      } else if(food_choice == "3") {
        std::cout << "Adding " << f.food_3 << " to your order." << endl;
        total += f.price_3;
        ordered += f.food_3;
      } else if(food_choice == "4") {
        std::cout << "Adding " << f.food_4 << " to your order." << endl;
        total += f.price_4;
        ordered += f.food_4;
      } else {
        std::cout << "Invalid Choice." << std::endl;
      }
    } else if(menu_choice == "3") {
      if(total > 0.01) {
        cout << std::endl << "remove>> ";
        getline(cin, remove_choice);

        // 0000000100007148         cmp        esi, 0x0
        if(remove_choice == "1" && ordered.find(f.food_1) != string::npos) {
          std::cout << "Removing " << f.food_1 << " from your order." << endl;
          total -= f.price_1;

          size_t pos = ordered.find(f.food_1);
          ordered.erase(pos, f.food_1.length());
        } else if(remove_choice == "2" && ordered.find(f.food_2) != string::npos) {
          std::cout << "Removing " << f.food_2 << " from your order." << endl;
          total -= f.price_2;

          size_t pos = ordered.find(f.food_2);
          ordered.erase(pos, f.food_2.length());
        } else if(remove_choice == "3" && ordered.find(f.food_3) != string::npos) {
          std::cout << "Removing " << f.food_3 << " from your order." << endl;
          total -= f.price_3;

          size_t pos = ordered.find(f.food_3);
          ordered.erase(pos, f.food_3.length());
        } else if(remove_choice == "4" && ordered.find(f.food_4) != string::npos) {
          std::cout << "Removing " << f.food_4 << " from your order." << endl;
          total -= f.price_4;

          size_t pos = ordered.find(f.food_4);
          ordered.erase(pos, f.food_4.length());
        } else {
          std::cout << "Invalid Choice." << std::endl;
        }
      } else {
        std::cout << "You haven't ordered anything." << std::endl;
      }
    } else if(menu_choice == "4") {
      std::cout << "Storing receipt to text file.." << std::endl;
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
