# cpp-algorithms
1. brew install boost
  - OR sudo apt-get install libboost-dev-all
2. brew install sfml
  - OR sudo apt-get install sfml-dev
3. chmod +x install.sh
4. ./install.sh

# Mysql DB (optional)
1. For Mac:
  - mysql.server start
  - mysql --user=root mysql
  - mysql> CREATE USER 'ryancor'@'localhost' IDENTIFIED BY 'qwerty';
  - mysql> GRANT ALL PRIVILEGES ON *.* TO 'ryancor'@'localhost' WITH GRANT OPTION;
  - mysql --user=ryancor mysql -p
  - CREATE DATABASE ordersdb;
