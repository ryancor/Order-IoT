#include <iostream>
#include <string>
#include <ostream>
#include <ctime>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <sfml/Network.hpp>
#include <sfml/Network/TcpSocket.hpp>
#include <sfml/Network/IpAddress.hpp>
#include "include/ip.hpp"
#include "include/helper.hpp"

using namespace std;

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
void post_data(const time_t seconds, unsigned long c_id,  char *name, float total_price, string food) {
  if(is_host_up(IP, PORT)) {
    std::stringstream ss;
    ss << seconds;
    string content = "time=";
    content += ss.str();
    content += "&cust_id=";
    content += to_string(c_id);
    content += "&name=";
    content += name;
    content +="&total_price=";
    content += to_string(total_price);
    content += "&food=";
    content += food;

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

char *user_request() {
  char *hostname = new char[HOST_NAME_MAX];
  char *username = new char[LOGIN_NAME_MAX];
  int result;

  result = gethostname(hostname, HOST_NAME_MAX);
  if(result) {
    perror("gethostname");
    strcpy(hostname, "unknown");
  }

  result = getlogin_r(username, LOGIN_NAME_MAX);
  if(result) {
    perror("getlogin_r");
    strcpy(username, "unknown");
  }
  return username;
}
