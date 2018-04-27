#include <string>

bool is_host_up(const std::string&, int);
void post_data(const time_t, unsigned long,  char *, float, std::string);
void boost_post_data(const time_t, float, std::string);
char *user_request();
