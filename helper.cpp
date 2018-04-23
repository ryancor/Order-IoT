#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>

using namespace std;

#define MAX_PATH        4096
#define FILE_PATH       "/tmp/coupon"

void list_choices() {
  std::cout << "1. List Food" << std::endl <<
  "2. Add item" << std::endl <<
  "3. Delete item" << std::endl <<
  "4. Place Order" << std::endl <<
  "5. Exit" << std::endl <<
  std::endl << "menu>> ";
}

char *convert(const std::string & s)
{
   char *pc = new char[s.size()+1];
   std::strcpy(pc, s.c_str());
   return pc;
}

string split_up_item(string s) {
  istringstream iss(s, istringstream::in);
  string buf;
  std::vector<std::string> item;
  // converting vector to char array
  std::vector<char*>  r_item;
  string food_item;

  while(iss >> buf) {
      iss >> buf;
      // put split strings into an array
      item.push_back(buf);
  }

  // storing first index to string
  std::transform(item.begin(), item.end(), std::back_inserter(r_item), convert);
  for(int i = 0 ; i < r_item.size() ; i++) {
    food_item = r_item[0];
  }
  return food_item;
}

char *split_up_money(string s) {
  istringstream iss(s, istringstream::in);
  string buf;
  std::vector<std::string> item;
  // converting vector to char array
  std::vector<char*>  m_item;
  char *money_item;

  while(iss >> buf) {
      iss >> buf;
      item.push_back(buf);
  }

  // storing third index to string to be converted to integer
  std::transform(item.begin(), item.end(), std::back_inserter(m_item), convert);
  for(int i = 0 ; i < m_item.size() ; i++) {
    money_item = m_item[2];
  }
  return money_item;
}

void receipt(float total_price, string food_array) {
  FILE *fp;
  fp = fopen("receipt.txt", "w+");
  fprintf(fp, "Total Price $%f\n\n", total_price);
  fprintf(fp, "Food Ordered: %s\n", food_array.c_str());
  fclose(fp);
}

int get_file_size(string filename) {
  FILE *p_file = NULL;
  p_file = fopen(filename.c_str(), "rb");

  fseek(p_file, 0, SEEK_END);
  int size = ftell(p_file);

  fclose(p_file);

  return size;
}

float couponed_code(float total_price) {
  ifstream cc(FILE_PATH);
  char buffer[MAX_PATH+1], new_buf[MAX_PATH+1];

  if(cc.is_open()) {
    int fsize = get_file_size(FILE_PATH);

    std::cout << "Secret Coupon Code Area!" << std::endl;

    if(fsize >= 2001) {
      std::cout << "Your file size is too large: " << fsize << std::endl;
      exit(-1);
    }

    while(cc >> buffer) {
      // copy only the memory from file, so anything in hex
      std::memcpy(new_buf, buffer, sizeof(buffer));
    }

    if(!memcmp(new_buf, "\xde\xad\xbe\xef", 4)) {
      std::cout << "You got 15 percent off your order." << std::endl;
      total_price = total_price - (total_price * 0.15); // 15 percent off
      std::cout << "Your new total: " << total_price << std::endl << std::endl;

      // Delete coupon
      remove(FILE_PATH);
    } else {
      std::cout << "Better luck next time." << std::endl << std::endl;
    }
  } else {
    std::cout << "Can't file for coupon in /tmp directory!" << std::endl << std::endl;
  }
  return total_price;
}
