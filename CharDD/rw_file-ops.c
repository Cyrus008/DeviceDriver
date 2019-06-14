/* This sample programe demonstrate the simple file operation when the module is inserted into the kernel .It just print the read/open/write/close statment.The all openration is done via the classical old method if can used this or adop 
 * new method.This method only create entry in /proc/devices but did't make any entry in the /dev .For that we need to do manuall given bellow :

   Step 01:  grep -nr "<device_name in module>" /proc/devices
   Step 02:  mknod /dev/Raspberry_xxxx c <Major Number> <Minor>
   Step 03:  echo 1 > /dev/Raspberry_xxxx c  {This will show open,write,close funtion}
   Step 04: dmesg {This will show all operation} 
   Step 05: cat /dev/Raspberry_xxxx c  {This will show open,read,write,close funtion}
   Usefull Funtion name:
   1->: alloc_chrdev_region(dev_t *dev, unsigned int firstminor,unsigned int count, char *name);
 * */


#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#define DEVICE_NAME "parth_maurya"

MODULE_LICENSE("Dual BSD/GPL");

dev_t dev = 0;
int device_num;

struct cdev pcdev;
struct class *pclass;

static int device_open(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);
static int device_release(struct inode *, struct file *);

static struct file_operations fops =
    {
        .owner = THIS_MODULE,
        .open = device_open,
        .read = device_read,
        .write = device_write,
        .release= device_release,

};

static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device is open function is called....$$$ :");
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buff, size_t len, loff_t *ppos)
{
    printk(KERN_INFO "Device is read funtion called....$$$");
    return 0;
}

static ssize_t device_write(struct file *file, const char __user *buff, size_t len , loff_t *ppos)
{
    printk(KERN_INFO "Device write function called......$$$");
    return len;
}

static int device_release(struct inode *inode, struct file * file)
{
    printk(KERN_INFO "Device release function called......$$$:");
    return 0;
}


static int __init device_init(void)
{
    /*Device number registration allocation dynamically !!!*/
       if((alloc_chrdev_region(&dev,0,1,DEVICE_NAME) < 0))
       {
        printk(KERN_ALERT "Device Registration is failed ");
        return -1;
       }
        printk(KERN_ALERT "Device major number:%d minor number :%d ",MAJOR(dev),MINOR(dev)) ;

        /*Character Device initalization function */
            cdev_init(&pcdev,&fops);
        if(cdev_add(&pcdev,dev,1) < 0)
        {
        printk(KERN_INFO "Cannot add the device to the system...\n");
        return -1;
        }
          if((pclass = class_create(THIS_MODULE,DEVICE_NAME)) == NULL){
              printk(KERN_INFO "Cannot create the struct class\n");
              return -1;
        }

        /*Creating device*/
        if((device_create(pclass,NULL,dev,NULL,DEVICE_NAME)) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            return -1;
        }
        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
        return 0;
}
static void device_exit(void)
{
    device_destroy(pclass,dev);
    class_destroy(pclass);
    cdev_del(&pcdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_ALERT "Goodbye, Device Driver removing is done .......\n !!");
}

module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("PRAMOD MAURYA");
MODULE_DESCRIPTION("A simple open/read/write/close operation example");
MODULE_VERSION("1.1");
