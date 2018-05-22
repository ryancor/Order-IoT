# Order-IoT
```
Welcome ryancor
Open: false
Customer ID: #117470808

Total Price: $0.00
Ordered:

1. List Food
2. Add item
3. Delete item
4. Place Order
5. Exit

menu>> 4
Storing receipt to text file..

Order sent to server: http://192.168.0.10/firmware-api/v1/place_order

Receipt Created Successfully.
```

# Install Dependencies
1. brew install boost
  - OR sudo apt-get install libboost-dev-all
2. brew install sfml
  - OR sudo apt-get install sfml-dev
3. MYSQL
  - Mac
    - brew install mysql
    - mysql.server start
  - Linux
    - sudo apt-get install libmysqlclient20
    - sudo apt-get install libmysqlclient-dev
    - sudo apt-get install mysql-client-core-5.7
    - sudo apt-get install mysql-server
    - sudo service mysql start
4. chmod +x install.sh
5. ./install.sh

# Create Mysql DB
1. For Mac & Linux:
  - mysql --user=root mysql -p
  - mysql> CREATE USER 'ryancor'@'localhost' IDENTIFIED BY 'qwerty';
  - mysql> GRANT ALL PRIVILEGES ON *.* TO 'ryancor'@'localhost' WITH GRANT OPTION;
  - mysql --user=ryancor mysql -p
  - CREATE DATABASE ordersdb;

# Side Fixes for Compiling on Linux
1. It seems as though, assembly files don't like the " _ " next to function names,
for example, if you look at src/call.s, you'll see the CALL on line 16. on Mac OSX, it
seems to compile fine, but on linux, the compiler may barf on it and see it as
undefined. So to fix this:
  - Remove underscore on src/call.s => Line 6, 7, 9, and 13
  - Remove underscore on src/open.s => Line 2, 3, 16, 33, and 44
