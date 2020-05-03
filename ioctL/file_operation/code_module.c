/* Description:
 * 	Here this is demo code that perform basic operation like open,close read,write.This code does not required sysfs to communicate
 * 	with the user space.So it did't create any link file under the /dev directory.Here we manually create node of the file using the 
 * 	mknode and provide the permission for read,write and execute.Instruction is given below:
 * 	$ cat /proc/devices | tail -50   //This will show your driver name,major and minor number of device
 *	$ sudo mknod /dev/alphA0 c 240 0 //This will create a device node under /dev 
 *	$ sudo chmod 665 /dev/alpha0     //This will create read,write,execute permission for device
 *	# echo "Hello Universe" > /dev/alphA0 //Write some on device 
 *	# tail -f /var/log/kern.log     //This will show you log of your driver operation
 *	
 * */


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
static char message[256] = {0};
static short  size_of_message;
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
	printk(KERN_ALERT "********************************************************\n" );
	device_num=alloc_chrdev_region(&dev,0,1,DEVICENAME);
    	if(device_num<0)
          {
          	printk(KERN_ALERT "Character Device Number allocation  is failed\n" );
	  	return -1;
	  }
	  printk(KERN_ALERT "Dynamic Major & Minor Number: %d %d",MAJOR(dev),MINOR(dev));
	  printk(KERN_ALERT "The device's major and minor is generateed \n");
	  cdev_init(&pdev_cdev,&dev_fops);
	  printk(KERN_ALERT "The device initalization is done successfully\n");
	  if((cdev_add(&pdev_cdev, dev,1)<0))
		{
		  printk(KERN_ALERT "The device can't be add to the system");
		  //unregister_chrdev_region(dev,1);
		}
	  printk(KERN_ALERT "The device addeding is done successfully\n");
	  printk(KERN_ALERT "********************************************************\n" );
	  return 0;
  }
static void chardevice_exit(void)
  {
	  	printk(KERN_ALERT "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n" );
	  	cdev_del(&pdev_cdev);
  		unregister_chrdev_region(dev, 1);
		printk(KERN_ALERT "Kernel Module deleting Successfully...:::\n");
		unregister_chrdev(device_num, DEVICENAME);
		printk(KERN_ALERT "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n" );
  }

static int device_open(struct inode *inode, struct file *file)
{
  		printk(KERN_ALERT "Device is opend...");
  		return 0;
}

static ssize_t device_read(struct file *file, char __user *buff, size_t len, loff_t * ppos)
{
		printk(KERN_ALERT "Device reading is done :\n");
        	return 0;
}

static ssize_t device_write(struct file *file, const char __user *buff, size_t len, loff_t *ppos)
{
		printk(KERN_ALERT "Device write  is done :\n");
		sprintf(message, "%s", buff, len);
		size_of_message = strlen(message);
		printk(KERN_INFO "EBBChar: Received %zu characters from the user\n", len);
        	return len;
}

static int device_release(struct inode *inode, struct file *file)
{
        	printk(KERN_ALERT "Device relase is done :\n");
        	return 0;
}


module_init(chardevice_init);
module_exit(chardevice_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("PRAMOD MAURYA");
MODULE_DESCRIPTION("Char device Registration interface");
MODULE_VERSION("1.1");

