#include <iostream>
#include <string>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

std::string get_IP() {
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
      if(strncmp(addressBuffer, "192", 3) == 0) {
        strcpy(en0_ip, addressBuffer);
      } else {
        strcpy(en0_ip, "192.168.0.8");
      }
    }
  }

  if(ifAddrStruct != NULL) {
    freeifaddrs(ifAddrStruct);
  }

  return en0_ip;
}
