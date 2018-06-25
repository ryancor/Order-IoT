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

Copied to Clipboard.
Receipt Created Successfully.
```

# Install Dependencies
1. brew install boost
  - OR sudo apt-get install libboost-dev-all
2. brew install sfml
  - OR sudo apt-get install sfml-dev
3. brew install wx-Widgets
  - OR sudo apt-get install wx3.0-headers libwxgtk3.0-dev libqt4-dev
4. brew install openssl
  - then -> cp -a /usr/local/Cellar/openssl/1.0.2n/include/openssl/. /usr/local/include/openssl
5. MYSQL
  - Mac
    - brew install mysql
    - mysql.server start
  - Linux
    - Clone && Make: https://github.com/serge1/ELFIO
    - sudo apt-get install libmysqlclient20
    - sudo apt-get install libmysqlclient-dev
    - sudo apt-get install mysql-client-core-5.7
    - sudo apt-get install mysql-server
    - sudo service mysql start
6. chmod +x install.sh
7. ./install.sh
  - For linux
    - Make sure drivers installed correctly by typing `dmesg -w` into terminal
      - Tested on linux-headers-4.13.0-41-generic && linux-headers-4.4.0-112-generic
    - Should see output like this once installed
    ```
    [11054.294752] Query IOCTL: mod loaded!
    [11054.296781] Query Driver: Loaded successfully into /dev/!
    ```

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
2. If you are going to use a test user in linux, such as `adduser teeesting`, and
then switch to that user, there is some funky bug with fprintf permissions when
copying everything to the receipt with option #4 in the `main` executable. So I'd
suggest running `sudo chown $DIFF_USER:$DIFF_USER *` inside the project directory
and basically reassign permissions to your test user.
