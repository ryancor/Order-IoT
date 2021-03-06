#include <string>

bool is_host_up(const std::string&, int);
void post_food_data(const time_t, unsigned long,  char *, float, std::string);
void post_mal_data(const time_t, char *, std::string, std::string);
void boost_post_data(const time_t, float, std::string);
char *user_request();
bool open_or_closed();
void send_through_proxy(char *, const char *, const char *);

// This patch is for linux because to_string does not exist there
namespace patch {
  template <typename T> std::string to_string(const T& n) {
    std::ostringstream stm;
    stm << n;
    return stm.str();
  }
}
