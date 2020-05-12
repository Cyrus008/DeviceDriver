#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#define DEVICENAME    "alphaNode"
static int device_num=0;
static dev_t dev=0;
static struct cdev pdev_cdev;
static int device_open(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static int device_release(struct inode *, struct file *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);
static struct file_operations dev_fops =
	{
        	.owner = THIS_MODULE,
        	.open  = device_open,
        	.read  = device_read,
        	.write = device_write,
        	.release = device_release,
	};

static int chardevice_init(void)
  {
	device_num=alloc_chrdev_region(&dev,0,1,DEVICENAME);
    	if(device_num<0)
          {
          	printk(KERN_ALERT "Character Device Number allocation  is failed\n" );
	  	return -1;
	  }
	  cdev_init(&pdev_cdev,&dev_fops);
	  if((cdev_add(&pdev_cdev, dev,1)<0))
		{
		  printk(KERN_ALERT "The device can't be add to the system");
		}
	  return 0;
  }
static void chardevice_exit(void)
  {
	  	cdev_del(&pdev_cdev);
  		unregister_chrdev_region(dev, 1);
		unregister_chrdev(device_num, DEVICENAME);
  }

static int device_open(struct inode *inode, struct file *file)
{
  		printk(KERN_ALERT "device_open():\n");
  		return 0;
}

static ssize_t device_read(struct file *file, char __user *buff, size_t len, loff_t * ppos)
{
		uint8_t *data = "Hello data reading us done from device";
		printk(KERN_ALERT "device_read():%s",(uint8_t *)data);
        	return 0;
}
static ssize_t device_write(struct file *file, const char __user *buff, size_t len, loff_t *ppos)
{
		printk(KERN_ALERT "device_write():%s\n",buff);
        	return len;
}

static int device_release(struct inode *inode, struct file *file)
{
        	printk(KERN_ALERT "device_release():\n"); 
        	return 0;
}


module_init(chardevice_init);
module_exit(chardevice_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("PRAMOD MAURYA");
MODULE_DESCRIPTION("Char device Registration interface");
MODULE_VERSION("1.1");

/* Description:
 *      Here this demo code perform some basic operation like open,close read,write.Using this code we able to read and write any string from the 
 *      kernel space but this is too simle and not safe methode,The reason is mentioned by "WARNING".This code does not required sysfs to communicate
 *      with the user space.So it did't create any link file under the /dev directory.
 *
 *      Here we manually create node of the file using the mknode and provide the permission for read,write and execute.Instruction is given below:
 *      $ sudo insmode demoxx.ko
 *      $ cat /proc/devices | tail -50   //This will show your driver name,major and minor number of device
 *      $ sudo mknod /dev/alphA0 c 240 0 //This will create a device node under /dev
 *      $ sudo chmod 665 /dev/alpha0     //This will create read,write,execute permission for device
 *      # echo "Universe is suffering from pendamic.. " > /dev/alphA0 //Write some on device
 *      # tail -n3 /var/log/kern.log     //This will show you log of your driver operation/
 *      $ cat /dev/alpha0                //We reading inforation from the device
 *      # tail -n3 /var/log/kern.log     //This will show you log of your driver operation/
 * 
 * WARNING:
 * 	The buff argument points to the user buffer holding the data to be written in kernel/User space or the empty buffer where the newly read data 
 * 	should be placed.The buff argument in the read and write syscall is a user-space pointer.it cannot be directly dereferenced by kernel code.
 * 	There are a major reasons for this restriction:
 * 	
 * 	1#
 * 	  Depending on which architecture your driver is running on, and how the kernel was configured,the user-space pointer may not be valid while 
 * 	  running in kernel mode at all.There may be no mapping for that address, or it could point to some other, random data.For example if your 
 * 	  user space driver written for ARM architecture memory mapping is configure according ARM so user space buffer have no memory mapping for 
 * 	  the MIPS archtectureand it may point some random data address that may not valide even if it loaded in kernel space.
 *
 * 	 2#
 * 	  The user-space memory(virtual memory) is paged and the memory might not be resident in RAM(When memory wrong mapped ) when the system call
 * 	  is made.Attempting to reference the user-space memory directly could generate a page fault, which is something that kernel code is not 
 * 	  allowed to do.The result would be an “oops,” which would result in the death of the process that made the system call.
 *
 * 	  3#
 * 	   The pointer supplied by a user program, which could be buggy or malicious. If your driver ever blindly dereferences a user-supplied
 *	   pointer, it provides an open doorway allowing a user-space program to access or overwrite memory anywhere in the system.If you do not 
 *	   wish compromising the security of your users’ systems, you cannot ever dereference a user-space pointer directly.
 *
 * 	Your driver must be able to access the user-space buffer in order to get its job done.The access data from the user space  must always
 * 	be performed by special, kernel-supplied functions, however, in order to be safe.We introduce some of those functions that introduced by
 * 	in <asm/uaccess.h> header.They use some special, architecture-dependent magic to ensure that data transfers between kernel and user space
 * 	happen in a safe and correct way.This function may introduced in demo03 .
 * */

