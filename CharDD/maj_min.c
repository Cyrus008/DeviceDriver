#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#define DEVICE_NAME    "parthu"
MODULE_LICENSE("GPL");
dev_t dev = MKDEV(255,0);
int result;
static int __init hello_init(void)
        {
         printk(KERN_ALERT "Major Number Number allocation:::\n");
         result=register_chrdev_region(dev, 1, DEVICE_NAME);
                 result=a
        if(result < 0){
                printk(KERN_ALERT "Major Number allocation is failed:%d\n" ,MAJOR(dev));
                return -1;
         }
         printk(KERN_ALERT "Major Number : %d : Minor Number:%d",MAJOR(dev),MINOR(dev));
       return 0;
        }
static void __exit hello_exit(void)
        {
        printk(KERN_ALERT "Goodbye, cruel world\n");
        unregister_chrdev_region(dev, 1);
        }
module_init(hello_init);
module_exit(hello_exit);

