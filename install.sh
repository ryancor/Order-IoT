OS=`uname`
if [ "${OS}" = "Linux" ] ; then
  cd driver
  make
  # Remove drive (if exists) before installing
  sudo rmmod query_ioctl
  sudo rmmod kernel_bus
  sudo insmod query_ioctl.ko
  sudo insmod kernel_bus.ko
  # change ryancor to your local username
  sudo chown ryancor:ryancor /dev/query_driver
  cd ../
fi
cd lib
make
make clean
cd ../
make
make clean
