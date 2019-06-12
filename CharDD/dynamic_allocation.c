#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#define DEVICE_NAME    "parthu"

dev_t dev ;
int result;

static int __init hello_init(void)
	{
	 printk(KERN_ALERT "Major Number Number allocation:::\n");
         result=alloc_chrdev_region(&dev,0,1, DEVICE_NAME);
		 result=reg
	if(result < 0){
		printk(KERN_ALERT "Major Number allocation is failed:%d\n" ,MAJOR(dev));
		return -1;
	 }
	 printk(KERN_ALERT "Major Number : %d : Minor Number:%d",MAJOR(dev),MINOR(dev));
	 printk(KERN_INFO "Kernel Module Inserted Successfully...:::\n");
       return 0;
	}
static void __exit hello_exit(void)
	{
	printk(KERN_ALERT "Kernel Module deleting Successfully...:::\n");
	unregister_chrdev_region(dev, 1);
	}
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("PRAMOD MAURYA");
MODULE_DESCRIPTION("A simple Maor and Minor Number allocation example");
MODULE_VERSION("1.1");
