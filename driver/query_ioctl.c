#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <asm/io.h>

#include "query_ioctl.h"

#define FIRST_MINOR   0
#define MINOR_CNT     1
#define IRQ_N         1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
struct dentry *sys_kernel_debugger_f;
static int price = 0;
static char *order;
static int numberOfOpens = 0;

// worker definitions
static void query_ioctl_work_handler(struct work_struct *w);
static struct workqueue_struct *wq = 0;
static DECLARE_DELAYED_WORK(query_ioctl_work, query_ioctl_work_handler);
static unsigned long onesec;

// worker queue
static void query_ioctl_work_handler(struct work_struct *w) {
  pr_info("query_ioctl worker is at %u jiffies\n", (unsigned)onesec);
}


// This is a keyboard interrupt handler => esc key gets pressed
irq_handler_t irq_handler(int irq, void *dev_id, struct pt_regs *regs) {
  static unsigned char scancode;

  // read keyboard status
  scancode = inb(0x60);

  if((scancode == 0x1) || (scancode == 0x81)) {
    printk("Orders-IOT: You caused an interrupt using `Esc` key!\n");
  }

  return (irq_handler_t) IRQ_HANDLED;
}

// VMA ops -> this only opens when mmap is opened at /sys/kernel/debug/
void vma_open(struct vm_area_struct *vma) {
  printk(KERN_NOTICE "Quey Driver: VMA Open -> virt %lx, phys %lx\n",
    vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}

void vma_close(struct vm_area_struct *vma) {
  printk(KERN_NOTICE "Query Driver: VMA Close.");
}

static struct vm_operations_struct remap_vm_ops = {
  .open = vma_open,
  .close = vma_close,
};

static int remap_mmap(struct file *filp, struct vm_area_struct *vma) {
  if(remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, vma->vm_end - vma->vm_start,
    vma->vm_page_prot)) {
      return -EAGAIN;
    }

  // initializing the vm_operations struct type
  vma->vm_ops = &remap_vm_ops;
  vma_open(vma);
  return 0;
}

// end of VMA

static int my_open(struct inode *i, struct file *f) {
  numberOfOpens++;
  printk(KERN_INFO "Query Driver: Has been updated %d time(s).\n", numberOfOpens);
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
      if(copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t))) {
        return -EACCES;
      }

      price = q.price;
      order = q.order;
      printk(KERN_INFO "Orders-IOT: Food = %s\nPrice = %d\n", order, price);

      break;
    case QUERY_GET_VARIABLES:
      q.price = price;
      q.order = order;

      if(order && price > 0) {
        q.size_of_all = price + (sizeof(order) >> 2);
      } else {
        q.size_of_all = 0;
      }

      if(copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t))) {
        return -EACCES;
      }

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
  .ioctl = my_ioctl,
#else
  .unlocked_ioctl = my_ioctl,
#endif
  .mmap = remap_mmap,
};

static int __init query_ioctl_init(void) {
  int ret, ret_irq;
  struct device *dev_ret;


  if((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0) {
    return ret;
  }

  printk(KERN_INFO "Query IOCTL: mod loaded!\n");

  cdev_init(&c_dev, &query_fops);

  if((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0) {
    return ret;
  }

  if(IS_ERR(cl = class_create(THIS_MODULE, "char"))) {
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    return PTR_ERR(cl);
  }

  if(IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "query_driver"))) {
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    return PTR_ERR(dev_ret);
  }

  // creates a kernel bus to userland to read virtual memory addresses
  if(IS_ERR(sys_kernel_debugger_f = debugfs_create_file("query_vma", 0644, NULL, NULL, &query_fops))) {
    return PTR_ERR(sys_kernel_debugger_f);
  }

  // register interrupt
  /* Request IRQ_N 1, the keyboard IRQ */
  ret_irq = request_irq(IRQ_N, (irq_handler_t) irq_handler, IRQF_SHARED, "query_ioctl", (void*)(irq_handler));
  if(ret_irq) {
    printk(KERN_INFO "Query Driver: Can't get shared interrupt for keyboard\n");
  }

  // get worker queue
  onesec = msecs_to_jiffies(1000);
  pr_info("query_ioctl loaded %u jiffies\n", (unsigned)onesec);

  if(!wq) {
    wq = create_singlethread_workqueue("query_ioctl");
  }

  if(wq) {
    queue_delayed_work(wq, &query_ioctl_work, onesec);
  }

  printk(KERN_INFO "Query VMA: Loaded successfully into /sys/kernel/debug/!\n");
  printk(KERN_INFO "Query Driver: Loaded successfully into /dev/!\n");

  return 0;
}

static void __exit query_ioctl_exit(void) {
  device_destroy(cl, dev);
  class_destroy(cl);
  cdev_del(&c_dev);
  unregister_chrdev_region(dev, MINOR_CNT);

  debugfs_remove(sys_kernel_debugger_f);

  // remove interrupt handler
  free_irq(IRQ_N, (void*)(irq_handler));

  // free worker queue
  if(wq) {
    destroy_workqueue(wq);
  }

  printk(KERN_ALERT "Query Driver: Removed from /dev/!\n");
}

module_init(query_ioctl_init);
module_exit(query_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ryan Cornateanu help@hackersclub.io");
MODULE_DESCRIPTION("Query ioctl() Char Driver");
