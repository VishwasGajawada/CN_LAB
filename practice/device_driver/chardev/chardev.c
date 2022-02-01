
#include <linux/module.h>  /* needed for module_init and module_exit */
#include <linux/kernel.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

#define dev_name "chardev"


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
  return 0;
}

static ssize_t write_dev(struct file *filep, const char *buff, size_t len, loff_t *off) {
  printk("write_dev\n");
  return 0;
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
  
  return 0;
}
static void __exit end_func(void){
  unregister_chrdev(major, dev_name);
  printk("%s: module unloaded\n", dev_name);
}

module_init(start_func);
module_exit(end_func);
