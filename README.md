# cpp-algorithms
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
