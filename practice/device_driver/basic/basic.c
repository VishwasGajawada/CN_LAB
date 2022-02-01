
#include <linux/module.h>  /* needed for module_init and module_exit */
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VishwasGajawada");

int init_module(void){
    printk(KERN_INFO "Hello World\n");
    return 0;
}

void cleanup_module(void){
    printk(KERN_INFO "Goodbye World\n");
}
