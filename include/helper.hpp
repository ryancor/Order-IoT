#include <string>

void list_choices();
char *convert(const std::string);
std::string split_up_item(std::string);
char *split_up_money(std::string);
void receipt(unsigned long, char*, float, std::string);
float couponed_code(float);
void clear_cin();
unsigned long customer_id();
