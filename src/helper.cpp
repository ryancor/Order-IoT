#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <istream>
#include <ostream>
#include <ctime>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#ifdef __unix__
#include <string.h>
#include <cstdlib>
#include <algorithm>
#include <stdint.h>
#endif

#include "../include/requests.hpp"

using namespace std;

#define MAX_PATH        4096
#define FILE_PATH       "/tmp/coupon"

extern "C" void verify();

void clear_cin() {
  for(int i = 0; i < 1; i++) {
    std::cin.clear();
    std::cin.ignore();
    continue;
  }
}

unsigned long customer_id() {
  uint32_t num;

  /* initialize random seed: */
  srand(time(NULL));

  /*
    generate secret number in asm between 1 and 0x80400000:
    num = rand() % 2151678196 + 1
    mov %1, %0 -> add $num, %0
  */
  __asm__ __volatile__ (
        "mov %%edx, %%ecx\n\t"
        "add $1, %%ecx"
        : "=r" (num)
        : "r" (rand() % 0x804061f4 + 0x1)
  );

  return num;
}

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
   strcpy(pc, s.c_str());
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
  transform(item.begin(), item.end(), std::back_inserter(r_item), convert);
  for(int i = 0 ; i < r_item.size() ; i++) {
    food_item = r_item[0];
  }
  return food_item;
}

char *split_up_char(string s, int num) {
  istringstream iss(s, istringstream::in);
  string buf;
  std::vector<std::string> item;
  // converting vector to char array
  std::vector<char*>  c_item;
  char *char_item;

  while(iss >> buf) {
      iss >> buf;
      item.push_back(buf);
  }

  // storing third index to string to be converted to integer
  transform(item.begin(), item.end(), std::back_inserter(c_item), convert);
  for(int i = 0 ; i < c_item.size() ; i++) {
    char_item = c_item[num];
  }
  return char_item;
}

void receipt(unsigned long c_id, char *name, float total_price, string food_array) {
  FILE *fp;
  const time_t ctt = time(0);

  // query name from computer for customer name
  fp = fopen("receipt.txt", "w+");
  fprintf(fp, "Customer ID: #%lu\n", c_id);
  fprintf(fp, "Customer: %s\n", name);
  fprintf(fp, "Date of Order: %s", asctime(localtime(&ctt)));
  fprintf(fp, "Total Price $%f\n\n", total_price);
  fprintf(fp, "Food Ordered: %s\n", food_array.c_str());

  fclose(fp);

  try {
    post_data(ctt, c_id, name, total_price, food_array.c_str());
  } catch(std::exception& e) {
    std::cout << "Couldn't make a successful request." << std::endl;
    std::cerr << e.what() << std::endl;
  }
  verify();
}

int get_file_size(string filename) {
  FILE *p_file = NULL;
  p_file = fopen(filename.c_str(), "rb");

  // measure file in bytes
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
      memcpy(new_buf, buffer, sizeof(buffer));
    }

    if(!memcmp(new_buf, "\xde\xad\xbe\xef", 4)) {
      std::cout << "You got 15 percent off your order." << std::endl;
      total_price = total_price - (total_price * 0.15); // 15 percent off
      std::cout << "Your new total: $" << total_price << std::endl << std::endl;

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

// Functions for finding malicious files
bool has_suffix(const string& s, const string& suffix) {
  return (s.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

void findForeignFiles() {
  DIR *dir;
  struct dirent *ent;
  char path[256];
  const string ext[] = { ".bin", ".sh", ".py", ".exe", ".rb", ".elf", ".ps", ".dylib",
    ".dll", ".so", ".la", ".ko", ".php", ".html", ".js", ".sys"};

  // get current directory path
  if (getcwd(path, sizeof(path)) == NULL) {
    perror("getcwd() error");
  }

  if((dir = opendir(path)) != NULL) {
    while((ent = readdir(dir)) != NULL) {
        // inner loop to compare each file to each file ext
      for(int i = 0; i < sizeof(ext)/sizeof(ext[0]); i++) {
        // file.py -comp-> .sh
        if(has_suffix(ent->d_name, ext[i])) {
          // exclude our install script
          if(strncmp(ent->d_name, "install.sh", 10)) {
            std::cout << "Found malicious file: " << ent->d_name << std::endl;
          }
        }
      }
    }
    closedir(dir);
  } else {
    /* could not open directory */
    perror ("");
  }
}
