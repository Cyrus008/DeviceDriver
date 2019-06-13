/*This sample programe demonstrate the simple file operation when the module is inserted into the kernel .It just print the read/open/write/close statment
   The all openration is done via the classical old method if can used this or adop new method.This method only create entry in /proc/devices but did't make any entry
   in the /dev .For that we need to do manuall given bellow :
   Step 01:  grep -nr "<device_name in module>" /proc/devices
   Step 02:  mknod /dev/Raspberry_xxxx c <Major Number> <Minor>
   Step 03:  echo 1 > /dev/Raspberry_xxxx c
   Step 04: dmesg {This will show all operation} 
 * */

#include<linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/fs.h>
#include <linux/types.h>
#define DEVICE_NAME    "Raspberry_xxxx"

int majornum;
ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);
int device_release(struct inode *, struct file *);
int device_open(struct inode *, struct file *);

struct file_operations dev_fops = {
                       .owner  =   THIS_MODULE,
                       .open   =   device_open,
                       .read   =   device_read,
                       .write   =  device_write,
                       .release  = device_release,
                  } ;

static int __init  device_init(void)
{
   majornum=register_chrdev(0,DEVICE_NAME,&dev_fops);
   	if(majornum < 0)
   		{
    			printk(KERN_ALERT "Device registration is failed 02 : %d",majornum);
    			return  majornum;
   		}
  printk(KERN_ALERT "Major Number:%d\n",majornum);

	return 0;
}

static void __exit device_exit(void)
{
	unregister_chrdev(majornum, DEVICE_NAME);
	printk(KERN_ALERT "Kernel Module deleting Successfully...:::\n");

}

ssize_t device_read(struct file *file, char __user *buff, size_t len, loff_t * ppos)
{
	 printk(KERN_ALERT "Device reading is done :\n");
 	return 0;
}

ssize_t device_write(struct file *file, const char __user *buff, size_t len, loff_t *ppos)
{
 	printk(KERN_ALERT "Device relase is done :\n");
	return 0;
}

int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT "Device relase is done :\n");
	return 0;
}

int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT "Device open is done :\n");
	return 0;
}


module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("PRAMOD MAURYA");
MODULE_DESCRIPTION("A simple Maor and Minor Number allocation example");
MODULE_VERSION("1.1");

