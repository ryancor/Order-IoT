#include <iostream>
#include <string>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __unix__
#include <string.h>
#endif

#include "include/ip.hpp"

std::string IPP::get_IP(void) {
  struct ifaddrs *ifAddrStruct = NULL;
  struct ifaddrs *ifa = NULL;
  void *tmpAddrPtr = NULL;
  char en0_ip[75];

  getifaddrs(&ifAddrStruct);

  for(ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if(ifa->ifa_addr->sa_family==AF_INET) {
      tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];

      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      // Compare first nums in addr
      if(strstr(ifa->ifa_name, "en0") != NULL || strstr(addressBuffer, "192") != NULL) {
        strcpy(en0_ip, addressBuffer);
      }
    }
  }

  if(ifAddrStruct != NULL) {
    freeifaddrs(ifAddrStruct);
  }

  return en0_ip;
}
