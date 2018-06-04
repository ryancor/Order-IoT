#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>

#include <openssl/md5.h>

#include "../include/requests.hpp"

using namespace std;

unsigned char result[MD5_DIGEST_LENGTH];

string md5_print(unsigned char *md) {
  int i;
  string res;

  for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
    printf("%02x", md[i]);
    // short converts 0xA to 41 as string
    res.append(patch::to_string((short)md[i]));
  }
  printf("\n");
  return res;
}

unsigned long get_size_by_fd(int fd) {
  struct stat statbuf;

  if(fstat(fd, &statbuf) < 0) {
    exit(-1);
  }
  return statbuf.st_size;
}

string return_hash(char *file) {
  int file_descript;
  unsigned long file_size;
  void* file_buffer;

  file_descript = open(file, O_RDONLY);
  if(file_descript < 0) {
    std::cout << "Can't open file for hash." << std::endl;
  }

  file_size = get_size_by_fd(file_descript);
  std::cout << "File size: " << file_size << "kb" << std::endl;

  file_buffer = mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0);
  MD5((unsigned char*)file_buffer, file_size, result);
  munmap(file_buffer, file_size);

  return md5_print(result);
}
