#include <cstdlib>
#include <sstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <mysql/mysql.h>
#include <mysql/my_global.h>
#include <string>

#include "../include/encode.hpp"

using namespace std;

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void insert_data(MYSQL *con, int id, string query) {
  ostringstream strstr;
  strstr << "INSERT INTO Orders VALUES(" << id << ",'" << query << "')";
  string str = strstr.str();

  if (mysql_query(con, str.c_str())) {
      finish_with_error(con);
  }
}

void start_tbl(MYSQL *con, const char * HOST, const char * USER, const char * PASSWORD, const char * DATABASE) {
  if (mysql_real_connect(con, HOST, USER, PASSWORD, DATABASE, 0, NULL, 0) == NULL)  {
      std::cout << "Please start your MySQL DB." << std::endl <<
      "View README.md for instructions." << std::endl;
  }

  if (mysql_query(con, "DROP TABLE IF EXISTS Orders")) {
      finish_with_error(con);
  }

  if (mysql_query(con, "CREATE TABLE Orders(Id INT, Name TEXT)")) {
      finish_with_error(con);
  }

  const char* hb = ". Hamburger : $ 3.25";
  const char* gc = ". Grilled-Cheese : $ 2.50";
  const char* fs = ". Fries : $ 2.00";
  const char* sd = ". Soda : $ 1.00";

  insert_data(con, 1, base64_encode((const unsigned char*)hb, strlen(hb)));
  insert_data(con, 2, base64_encode((const unsigned char*)gc, strlen(gc)));
  insert_data(con, 3, base64_encode((const unsigned char*)fs, strlen(fs)));
  insert_data(con, 4, base64_encode((const unsigned char*)sd, strlen(sd)));
}

vector<string> DataGet() {
  MYSQL *con = mysql_init(NULL);
  vector<string> arr;
  string new_str;

  if (con == NULL) {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
  }

  start_tbl(con, "localhost", "ryancor", "qwerty", "ordersdb");

  if (mysql_query(con, "SELECT Name FROM Orders")) {
      finish_with_error(con);
  }

  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL) {
      finish_with_error(con);
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;

  while((row = mysql_fetch_row(result))) {
      for(int i = 0; i < num_fields; i++) {
          // Decode each row of data coming back up to console
          arr.push_back(row[i] ? base64_decode(row[i]) : "NULL");
          new_str += row[i] ? base64_decode(row[i]) : "NULL";
          new_str += " ";
      }
      new_str += " ";
  }

  mysql_free_result(result);
  mysql_close(con);

  return arr;
}
