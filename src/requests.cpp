#include <iostream>
#include <string>
#include <ostream>
#include <ctime>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <netdb.h>
#include <sys/socket.h>

#ifdef __APPLE__
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#endif

#ifdef __unix__
#include <sstream>
#include <string.h>
#include <QApplication>
#endif

#include <SFML/Network.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include "../include/ip.hpp"
#include "../include/helper.hpp"
#include "../include/requests.hpp"
#include "../include/clipIt.hpp"

using namespace std;

#ifdef __APPLE__
  // Copy content over to clip func
  #define useWxWidget copyRecToClip(content);
#else
  // converting string to QString so it can be put to QClipboard
  #define useWxWidget QtOnChar(QString::fromStdString(content));
#endif

IPP ip1;
#define IP               ip1.get_IP()
#define PORT             5000
#define HOST_NAME_MAX    64
#define LOGIN_NAME_MAX   84

bool is_host_up(const string& address, int port) {
  sf::TcpSocket socket;
  bool open = (socket.connect(sf::IpAddress(address), port) == sf::Socket::Done);
  socket.disconnect();
  return open;
}

// We use sfml
void post_food_data(const time_t seconds, unsigned long c_id,  char *name, float total_price, string food) {
  std::stringstream ss;
  ss << seconds;
  string content = "time=";
  content += ss.str();
  content += "&cust_id=";
  content += patch::to_string(c_id);
  content += "&name=";
  content += name;
  content +="&total_price=";
  content += patch::to_string(total_price);
  content += "&food=";
  content += food;

  if(is_host_up(IP, PORT)) {
    // prepare the request
    sf::Http::Request request("/firmware-api/v1/place_order", sf::Http::Request::Post);

    // encode the parameters in the request body
    std::ostringstream stream;
    stream << content;
    request.setBody(stream.str());

    // send the request
    sf::Http server(IP, 5000);
    sf::Http::Response response = server.sendRequest(request);

    if (response.getStatus() == sf::Http::Response::NoContent) {
        // check the contents of the response
        std::cout << "Order sent to server." << std::endl << std::endl;
    }
    else {
        std::cout << "Request failed." <<  std::endl << std::endl;
    }
  } else {
    std::cout << "Can't connect to API endpoint: " << IP << std::endl << std::endl;
  }
  useWxWidget;
}

void post_mal_data(const time_t seconds, char *file, string md5_hash, string sha_hash) {
  if(is_host_up(IP, PORT)) {
    std::stringstream ss;
    ss << seconds;
    string content = "time=";
    content += ss.str();
    content += "&malicious_file=";
    content += patch::to_string(file);
    content += "&md5_hash=";
    content += md5_hash;
    content += "&sha_hash=";
    content += sha_hash;

    // prepare the request
    sf::Http::Request request("/firmware-api/v1/malicious_activity", sf::Http::Request::Post);

    // encode the parameters in the request body
    std::ostringstream stream;
    stream << content;
    request.setBody(stream.str());

    // send the request
    sf::Http server(IP, 5000);
    sf::Http::Response response = server.sendRequest(request);

    if (response.getStatus() == sf::Http::Response::NoContent) {
        // check the contents of the response
        std::cout << "+1" << std::endl << std::endl;
    }
  } else {
    std::cout << std::endl;
  }
}

bool open_or_closed() {
  if(is_host_up(IP, PORT)) {
    // prepare the request
    sf::Http::Request request("/firmware-api/v1/hours", sf::Http::Request::Get);

    // send the request
    sf::Http server(IP, 5000);
    sf::Http::Response response = server.sendRequest(request);

    if (response.getStatus() == sf::Http::Response::Ok) {
      char bl[4];
      strncpy(bl, split_up_char(response.getBody(), 2), 4);

      if(strncmp(bl, "true", 4) == 0) {
        return true;
      }
    }
  }
  return false;
}

void send_through_proxy(char *web_addr) {
  addrinfo host_info;
  addrinfo *host_info_list;
  int socketfd, status, len;
  char *msg = NULL;
  char *msg2 = NULL;

  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo("127.0.0.1", "8080", &host_info, &host_info_list);

  socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
    host_info_list->ai_protocol);
  if(socketfd == -1) {
    std::cout << "ERROR: socket error" << std::endl;
    exit(-1);
  }

  std::cout << "Connect()ing..."  << std::endl;

  status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if(status == -1) {
    std::cout << "ERROR: connect error" << std::endl;
    exit(-1);
  }

  // connect to proxy
  msg = new char[200];
  strcpy(msg, "CONNECT ");
  strcat(msg, web_addr);
  strcat(msg, " HTTP/1.0\r\n");
  strcat(msg, "\r\n");

  ssize_t bytes_sent;
  len = strlen(msg);
  bytes_sent = send(socketfd, msg, len, 0);

  ssize_t bytes_recieved = 0;
  std::cout << "Waiting to recieve data..."  << std::endl;

  char *incoming_data_buffer = new char[200];
  bytes_recieved = recv(socketfd, incoming_data_buffer, 200, 0);
  if (bytes_recieved == 0) { std::cout << "host shut down." << std::endl; exit(-1); }
  if (bytes_recieved == -1) { std::cout << "ERROR: receive error!" << std::endl; exit(-1); }
  std::cout << bytes_recieved << " bytes recieved.." << std::endl;
  std::cout << "Proxy Receive Status: "
    << "HTTP/1.1 "
    << split_up_char(incoming_data_buffer, 0)
    << " OK"
    << std::endl;

  // message that needs to send to proxy
  msg2 = new char[300];
  strcpy(msg2, "GET ");
  strcat(msg2, web_addr);
  strcat(msg2, " HTTP/1.0\r\n\r\n");

  std::cout << "Message sent to proxy: " << msg2 << std::endl;
  len = strlen(msg2);
  bytes_sent = send(socketfd, msg2, len, 0);
  if(bytes_sent >= 300) {
    std::cout << "ERROR: Web address too large." << std::endl;
    exit(-1);
  }
  std::cout << "bytes_sent: " << bytes_sent << std::endl;

  bytes_recieved = 0;
  std::cout << "Waiting to recieve bytes.." << std::endl;
  char* incoming_data_buffer2 = new char[1000];

  bytes_recieved = recv(socketfd, incoming_data_buffer2, 1000, 0);

  if (bytes_recieved == 0) { std::cout << "host shut down." << std::endl; exit(-1); }
  if (bytes_recieved == -1) { std::cout << "ERROR: recieve error!" << std::endl; exit(-1); }
  std::cout << bytes_recieved << " bytes recieved" << std::endl << std::endl;
  //std::cout << incoming_data_buffer2 << std::endl << std::endl;
}

#ifdef __APPLE__
// we aren't using boost yet
void boost_post_data(const time_t seconds, float total_price, string food) {
  boost::asio::ip::tcp::iostream stream;

  string content = "data=";

  stream.connect(IP, to_string(PORT));
  stream << "POST /firmware-api/v1/place_order?" << content << " HTTP/1.1\r\n";
  stream << "Host: " << IP << ":" << to_string(PORT) << "\r\n";
  stream << "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n";
  stream << "Connection: close\r\n\r\n";
  stream << "Content-Length: 13\r\n";

  stream.flush();
  std::cout << stream.rdbuf();
}
#endif


char *user_request() {
  char *hostname = new char[HOST_NAME_MAX];
  char *username = new char[LOGIN_NAME_MAX];
  int result;

  register struct passwd *pw;
  register uid_t uid;

  result = gethostname(hostname, HOST_NAME_MAX);
  if(result) {
    perror("gethostname");
    strcpy(hostname, "unknown");
  }

  result = getlogin_r(username, LOGIN_NAME_MAX);
  if(result) {
    uid = geteuid();
    pw = getpwuid(uid);

    if(pw) {
      strcpy(username, pw->pw_name);
    } else {
      perror("getlogin_r");
      strcpy(username, "unknown");
    }
  }
  return username;
}
