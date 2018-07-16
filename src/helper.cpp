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
#include <elfio/elfio.hpp>

using namespace ELFIO;
#endif

#include "../include/requests.hpp"
#include "../include/file_hash.hpp"
#include "../include/cpu_rec.hpp"

using namespace std;

#define MAX_PATH          4096
#define FILE_PATH         "/tmp/coupon"
#define WHITE_LIST_FILE   "/var/whitelist.log"

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
    post_food_data(ctt, c_id, name, total_price, food_array.c_str());
  } catch(std::exception& e) {
    std::cout << "Couldn't make a successful request." << std::endl;
    std::cerr << e.what() << std::endl;
  }
  verify();
}

static int get_file_size(string filename) {
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
      remove(FILE_PATH);
      goto better_luck;
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
      better_luck:
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
  const time_t ctt = time(0);
  DIR *dir;
  struct dirent *ent;
  char path[MAX_PATH];
  const string ext[] = { ".bin", ".sh", ".py", ".exe", ".rb", ".elf", ".ps", ".dylib",
    ".dll", ".so", ".la", ".ko", ".php", ".html", ".js", ".sys", ".corpus"};
  // open select white-list channels
  ifstream w_file(WHITE_LIST_FILE);
  string w_file_line;

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
            // check to see if white-list config file exists
            if(!w_file.fail()) {
              while (getline(w_file, w_file_line, '\n')) {
                if(ent->d_name == w_file_line) {
                  std::cout << std::endl << "[*] " << ent->d_name
                            << " is a whitelisted file." << std::endl << std::endl;
                  // jump past the malicious file check if whitelisted
                  goto end_of_loop;
                }
              }
            } else {
              std::cout << "You can create a whitelist config file at "
                        << WHITE_LIST_FILE << std::endl;
            }

            if(get_file_size(ent->d_name) > 1) {
              std::cout << std::endl << "[!] Found malicious file: " << ent->d_name <<
              std::endl;

              // check cpu_recs for each file
              list_cpu_rec_after_check(ent->d_name);
            } else {
              std::cout << std::endl << "[!] Found empty foreign file: " << ent->d_name <<
              std::endl;
            }

            // send data
            try {
              post_mal_data(ctt, ent->d_name, return_md5_hash(ent->d_name), return_sha_hash(ent->d_name));
            } catch(std::exception& e) {
              std::cout << "-1" << std::endl;
              std::cerr << e.what() << std::endl;
            }
          }
          end_of_loop:
            continue;
        }
      }
    }
    w_file.close();
    closedir(dir);
  } else {
    /* could not open directory */
    perror ("");
  }
}

#ifdef __unix__
void check_file(char *file) {
  elfio reader;

  if(!reader.load(file)) {
    std::cout << "Can't find or process ELF file." << std::endl;
    exit(2);
  }
  std::cout << "~~~~ Anatomy of file ~~~~~" << std::endl;

  std::cout << "ELF file class: ";
  if(reader.get_class() == ELFCLASS32) {
    std::cout << "ELF32" << std::endl;
  } else {
    std::cout << "ELF64" << std::endl;
  }

  std::cout << "ELF file encoding: ";
  if(reader.get_encoding() == ELFDATA2LSB) {
    std::cout << "Little Endian" << std::endl << std::endl;
  } else {
    std::cout << "Big Endian" << std::endl << std::endl;
  }

  Elf_Half sec_num = reader.sections.size();
  std::cout << "Number of sections: " << sec_num << std::endl;
  for(int i = 0; i < sec_num; i++) {
    const section *psec = reader.sections[i];
    if(psec->get_size() >= 10 && psec->get_size() <= 2000) {
      std::cout << " [" << i << "] "
                << psec->get_name()
                << "\t"
                << psec->get_size()
                << std::endl;
    }
    //const char *p = reader.sections[i]->get_data();

    if(psec->get_type() == SHT_SYMTAB) {
      const symbol_section_accessor symbols(reader, (ELFIO::section*)psec);
      for(unsigned int j = 0; j < symbols.get_symbols_num(); j++) {
        string          name;
        Elf64_Addr      value;
        Elf_Xword       size;
        unsigned char   bind;
        unsigned char   type;
        Elf_Half        section_index;
        unsigned char   other;

        symbols.get_symbol(j, name, value, size, bind, type, section_index, other);
        if(name.size() >= 2) {
          if(name.find("strlen") == string::npos &&
             name.find("memcpy") == string::npos) {
            std::cout << j << " " << name << std::endl << std::endl;
          } else {
            std::cout << j << " " << name << "  [!] Vulnerable Function!"
                      << std::endl << std::endl;
          }
        }
      }
    }
  }
  printf("\n\n");
}
#endif
