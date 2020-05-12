#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#define DEVICENAME    "alphaNode-dev"
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
	if(alloc_chrdev_region(&dev,0,1,DEVICENAME)<0)
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
		size_t datalen = strlen(data);
		printk("Reading device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));
		if(len > datalen)
			len = datalen;  
		if(copy_to_user(buff, data, len))
	     		return -EFAULT;
		printk(KERN_ALERT "device_read():%s",(uint8_t *)data);	
        	return len;
}
static ssize_t device_write(struct file *file, const char __user *buff, size_t len, loff_t *ppos)
{		 ssize_t  Maxdlen = 30,ncopied;
    		 uint8_t bufdata[Maxdlen];
		 if(len < Maxdlen)
			 Maxdlen=len ;
		 ncopied = copy_from_user(bufdata,buff,Maxdlen);
		 if(ncopied == 0)
			 printk("Copied %zd bytes from the user\n", Maxdlen);
		 else
			printk("Could't copy %zd bytes from the user\n", copy_from_user(bufdata,buff,Maxdlen));

	         bufdata[Maxdlen]=0;	
		 printk("Data from the user: %s\n", bufdata); 
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
 *      Here this demo code perform some basic operation like read,write.Using this code we able to read and write any string from the kernel space.
 *      This code does not required sysfs to communicate with the user space.So it did't create any link file under the /dev directory.
 *
 *      Here we manually create node of the file using the mknode and provide the permission for read,write and execute.Instruction is given below:
 *      $ sudo insmode demoxx.ko
 *      $ cat /proc/devices | tail -50   //This will show your driver name,major and minor number of device
 *      $ sudo mknod /dev/alphA0 c 240 0 //This will create a device node under /dev
 *      $ sudo chmod 665 /dev/alpha0     //This will create read,write,execute permission for device
 *      # echo "Universe is suffering from pendamic.. " > /dev/alphA0 //Write some on device
 *      # tail -n3 /var/log/kern.log     //This will show you log of your driver operation/
 *      $ head -c80 /dev/alphA0          //We reading some inforation from the device from the device head
 *      # tail -n3 /var/log/kern.log     //This will show you log of your driver operation/
 * 
 * NOTE:
 * 	Your driver must be able to access the user-space buffer in order to get its job done.The access data from the user space  must always
 * 	be performed by special, kernel-supplied functions, however, in order to be safe.The functions that introduced by in <asm/uaccess.h> header.
 * 	They use some special, architecture-dependent magic to ensure that data transfers between kernel and user space happen in a safe and correct
 * 	way.The function prototype is given bellow:
 *
 *			copy_to_user(void __user *to,const void *from,unsigned long count);
 *			copy_from_user(void *to,const void __user *from,unsigned long count);
 *
 *      The copy_to_user/copy_from_user used in read and write method to copy a whole segment of data to or from the user address space.These API
 *      copy an arbitrary array of bytes and sit at the heart of most read and write implementations:
 * */

