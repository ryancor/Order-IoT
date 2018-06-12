#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>

#include "query_ioctl.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
static int price = 0;
static char *order;

static int my_open(struct inode *i, struct file *f) {
  return 0;
}

static int my_close(struct inode *i, struct file *f) {
  return 0;
}

/*
  Type name    32-bit type    64-bit type

  SInt32       long           int

  UInt32       unsigned long  unsigned int

  Checking to see if kernel is 32bit
*/

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
  query_arg_t q;

  switch (cmd) {
    case QUERY_SET_VARIABLES:
      if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t))) {
          return -EACCES;
      }

      price = q.price;
      order = q.order;
      printk(KERN_INFO "Orders-IOT: Food = %s\nPrice = %d\n", order, price);

      break;
    default:
      return -EINVAL;
  }

  return 0;
}

static struct file_operations query_fops = {
  .owner = THIS_MODULE,
  .open = my_open,
  .release = my_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
  .ioctl = my_ioctl
#else
  .unlocked_ioctl = my_ioctl
#endif
};

static int __init query_ioctl_init(void) {
  int ret;
  struct device *dev_ret;


  if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0) {
    return ret;
  }

  printk(KERN_INFO "Query IOCTL: mod loaded!\n");

  cdev_init(&c_dev, &query_fops);

  if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0) {
    return ret;
  }

  if (IS_ERR(cl = class_create(THIS_MODULE, "char"))) {
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    return PTR_ERR(cl);
  }

  if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "query_driver"))) {
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    return PTR_ERR(dev_ret);
  }

  printk(KERN_INFO "Query Driver: Loaded successfully into /dev/!\n");

  return 0;
}

static void __exit query_ioctl_exit(void) {
  device_destroy(cl, dev);
  class_destroy(cl);
  cdev_del(&c_dev);
  unregister_chrdev_region(dev, MINOR_CNT);

  printk(KERN_ALERT "Query Driver: Removed from /dev/!\n");
}

module_init(query_ioctl_init);
module_exit(query_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ryan Cornateanu help@hackersclub.io");
MODULE_DESCRIPTION("Query ioctl() Char Driver");
