
#include <linux/module.h>  /* needed for module_init and module_exit */
#include <linux/kernel.h>
#include <linux/slab.h> /* kmalloc */
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VishwasGajawada");

#define dev_name "read_write"

char *mybuf;

static int open_dev(struct inode *inode, struct file *filep) {
  printk("open_dev\n");
  return 0;
}

static int release_dev(struct inode *inode, struct file *filep) {
  printk("release_dev\n");
  return 0;
}

static ssize_t read_dev(struct file *filep, char *buff, size_t len, loff_t *off) {
  printk("read_dev\n");
  if(copy_to_user(buff, mybuf, len)) {
    return -EFAULT;
  }
  return sizeof(mybuf);
}

static ssize_t write_dev(struct file *filep, const char *buff, size_t len, loff_t *off) {
  printk("write_dev\n");
  if(copy_from_user(mybuf, buff, len)) {
    return -EFAULT;
  }
  return len;
}

int major;
static struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = open_dev,
  .release = release_dev,
  .read = read_dev,
  .write = write_dev,
};

static int __init start_func(void){
  major = register_chrdev(0, dev_name, &fops);
  if(major < 0){
    printk("%s: register_chrdev failed with %d\n", dev_name, major);
    return major;
  }
  printk("%s: registered with major number %d\n", dev_name, major);
  mybuf = (char *)kmalloc(1024, GFP_KERNEL);
  strcpy(mybuf, "Hello World");
  return 0;
}
static void __exit end_func(void){
  unregister_chrdev(major, dev_name);
  printk("%s: module unloaded\n", dev_name);
}

module_init(start_func);
module_exit(end_func);
