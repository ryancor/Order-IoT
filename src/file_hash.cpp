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
#include <openssl/sha.h>

#include "../include/requests.hpp"

using namespace std;

unsigned char md5_result[MD5_DIGEST_LENGTH];
unsigned char sha_result[SHA256_DIGEST_LENGTH];

string hash_print(unsigned char *md, size_t length) {
  size_t i;
  string res;

  printf("[+] ");

  for(i = 0; i < length; i++) {
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

string return_md5_hash(char *file) {
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
  MD5((unsigned char*)file_buffer, file_size, md5_result);
  munmap(file_buffer, file_size);

  printf("------------------------\n");
  printf("MD5 & SHA256 Hash Below\n");
  printf("------------------------\n");

  return hash_print(md5_result, MD5_DIGEST_LENGTH);
}

string return_sha_hash(char *file) {
  int file_descript;
  unsigned long file_size;
  void* file_buffer;

  file_descript = open(file, O_RDONLY);
  if(file_descript < 0) {
    std::cout << "Can't open file for hash." << std::endl;
  }

  file_size = get_size_by_fd(file_descript);

  file_buffer = mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0);
  SHA256((unsigned char*)file_buffer, file_size, sha_result);
  munmap(file_buffer, file_size);

  return hash_print(sha_result, SHA256_DIGEST_LENGTH);
}
