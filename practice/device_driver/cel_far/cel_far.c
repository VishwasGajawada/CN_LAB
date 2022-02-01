
#include <linux/module.h>  /* needed for module_init and module_exit */
#include <linux/kernel.h>
#include <linux/slab.h> /* kmalloc */
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VishwasGajawada");

#define dev_name "cel_far"

char *mybuf;
int in_temp, out_temp;

int atoi(char *str)
{
    int res = 0;
    int sign = 1;
    int i = 0;
    if (str[0] == '-')
    {
        sign = -1;
        i = 1;
    }
    for (; str[i] != '\0'; ++i)
    {
        res = res * 10 + str[i] - '0';
    }
    return sign * res;
}

int C_to_F(int temp){
  /* converts celsius to farenheit */
  return ( (temp * 9) / 5) + 32;
}
int F_to_C(int temp){
  /* converts farenheit to celsius */
  return ((temp - 32) * 5 )/ 9;
}


static int open_dev(struct inode *inode, struct file *filep) {
  printk("open_dev\n");
  mybuf = (char *)kmalloc(10, GFP_KERNEL);
  return 0;
}

static int release_dev(struct inode *inode, struct file *filep) {
  printk("release_dev\n");
  kfree(mybuf);
  return 0;
}

static ssize_t read_dev(struct file *filep, char __user *buff, size_t len, loff_t *off) {
  printk("read_dev\n");
  sprintf(mybuf, "%d", out_temp);
  if(copy_to_user(buff, mybuf, len)) {
    return -EFAULT;
  }
  return 10;
}

static ssize_t write_dev(struct file *filep, const char *buff, size_t len, loff_t *off) {
  printk("write_dev\n");
  if(copy_from_user(mybuf, buff, len)) {
    printk("copy_from_user failed\n");
    return -EFAULT;
  }
  int choice = (int)(mybuf[0] - '0');
  mybuf = mybuf + 2;
  in_temp = atoi(mybuf);


  if(choice == 1){
    out_temp = C_to_F(in_temp);
  }else{
    out_temp = F_to_C(in_temp);
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
  
  return 0;
}
static void __exit end_func(void){
  unregister_chrdev(major, dev_name);
  printk("%s: module unloaded\n", dev_name);
  
}

module_init(start_func);
module_exit(end_func);
