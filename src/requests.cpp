#include <iostream>
#include <string>
#include <ostream>
#include <ctime>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#ifdef __APPLE__
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#endif

#ifdef __unix__
#include <sstream>
#include <string.h>
#endif

#include <SFML/Network.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <wx/clipbrd.h>

#include "../include/ip.hpp"
#include "../include/helper.hpp"
#include "../include/requests.hpp"

using namespace std;

IPP ip1;
#define IP               "192.168.0.9"
#define PORT             5000
#define HOST_NAME_MAX    64
#define LOGIN_NAME_MAX   84

bool is_host_up(const string& address, int port) {
  sf::TcpSocket socket;
  bool open = (socket.connect(sf::IpAddress(address), port) == sf::Socket::Done);
  socket.disconnect();
  return open;
}

void copyRecToClip(string clip) {
  // get text to copy to clip
  if(wxTheClipboard->Open()) {
    wxTheClipboard->SetData(new wxTextDataObject(clip));
    wxTheClipboard->Close();
  }

  // read it
  if(wxTheClipboard->Open()) {
    if(wxTheClipboard->IsSupported(wxDF_TEXT)) {
      wxTextDataObject data;
      wxTheClipboard->GetData(data);

      const char *new_text = data.GetText();
      //wxInfoMessageBox((wxWindow*)new_text);
      std::cout << "Copied to Clipboard." << std::endl;
    }
    wxTheClipboard->Close();
  }
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
  // Copy content over to clip func
  copyRecToClip(content);
}

void post_mal_data(const time_t seconds, char *file) {
  if(is_host_up(IP, PORT)) {
    std::stringstream ss;
    ss << seconds;
    string content = "time=";
    content += ss.str();
    content += "&malicious_file=";
    content += patch::to_string(file);

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
