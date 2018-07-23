#include <asm/uaccess.h> /* copy_from_user */
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h> /* min */
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

static const char *filename = "orderIOT_mmap";

enum { BUFFER_SIZE = 4 };

struct mmap_info {
  char *data;
};

/* After unmap. */
static void vm_close(struct vm_area_struct *vma) {
  pr_info("orderIOT_mmap: vm_close\n");
}

/* First page access. */
static int vm_fault(struct vm_area_struct *vma, struct vm_fault *vmf) {
  struct page *page;
  struct mmap_info *info;

  pr_info("orderIOT_mmap: vm_fault\n");

  info = (struct mmap_info *)vma->vm_private_data;
  if (info->data) {
    page = virt_to_page(info->data);
    get_page(page);
    vmf->page = page;
  }
  return 0;
}

static void vm_open(struct vm_area_struct *vma) {
  pr_info("orderIOT_mmap: vm_open\n");
}

static struct vm_operations_struct vm_ops =
{
  .close = vm_close,
  .fault = vm_fault,
  .open = vm_open,
};

static int mmapdrv_mmap(struct file *filp, struct vm_area_struct *vma) {
  pr_info("orderIOT_mmap: mmap\n");

  vma->vm_ops = &vm_ops;
  vma->vm_flags |= VM_DONTEXPAND | VM_DONTDUMP;
  vma->vm_private_data = filp->private_data;

  vm_open(vma);
  return 0;
}

static int mmapdrv_open(struct inode *inode, struct file *filp) {
  struct mmap_info *info;

  pr_info("orderIOT_mmap: open\n");

  info = kmalloc(sizeof(struct mmap_info), GFP_KERNEL);
  pr_info("orderIOT_mmap: virt_to_phys = 0x%llx\n", (unsigned long long)virt_to_phys((void *)info));

  info->data = (char *)get_zeroed_page(GFP_KERNEL);
  memcpy(info->data, "dead", BUFFER_SIZE);
  filp->private_data = info;

  return 0;
}

static ssize_t mmapdrv_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
  struct mmap_info *info;
  int ret;

  pr_info("orderIOT_mmap: read\n");

  info = filp->private_data;
  ret = min(len, (size_t)BUFFER_SIZE);
  if (copy_to_user(buf, info->data, ret)) {
    ret = -EFAULT;
  }
  return ret;
}

static ssize_t mmapdrv_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
  struct mmap_info *info;

  pr_info("orderIOT_mmap: write\n");

  info = filp->private_data;
  if(copy_from_user(info->data, buf, min(len, (size_t)BUFFER_SIZE))) {
    return -EFAULT;
  } else {
    return len;
  }
}

static int mmapdrv_release(struct inode *inode, struct file *filp) {
  struct mmap_info *info;

  pr_info("orderIOT_mmap: release\n");
  info = filp->private_data;
  free_page((unsigned long)info->data);
  kfree(info);
  filp->private_data = NULL;
  return 0;
}

static const struct file_operations fops = {
  .mmap = mmapdrv_mmap,
  .open = mmapdrv_open,
  .release = mmapdrv_release,
  .read = mmapdrv_read,
  .write = mmapdrv_write,
};

static int orderIOT_init(void) {
  // creates location at /proc/orderIOT_mmap
  proc_create(filename, 0, NULL, &fops);
  printk(KERN_INFO "orderIOT_mmap kernel bus initialized\n");
  return 0;
}

static void orderIOT_exit(void) {
  remove_proc_entry(filename, NULL);
  printk(KERN_ALERT "orderIOT_mmap kernel bus removed\n");
}

module_init(orderIOT_init);
module_exit(orderIOT_exit);
MODULE_LICENSE("GPL");
