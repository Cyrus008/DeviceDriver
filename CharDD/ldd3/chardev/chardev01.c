#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "charheader.h"

int device_qset =   DEVICE_QSET;
int device_trim(struct device_dev *dev)
	{
		struct device_qset *next, *dptr;
		int qset = dev->qset;   /* "dev" is not-null */
		int i;

		for (dptr = dev->data; dptr; dptr = next) 
		{ 					/* all the list items */
			if (dptr->data)
		       	{
				for (i = 0; i < qset; i++)
					kfree(dptr->data[i]);
				kfree(dptr->data);
				dptr->data = NULL;
			}
			next = dptr->next;
			kfree(dptr);
		}
		dev->size = 0;
		//dev->quantum = scull_quantum;
		dev->qset = device_qset;
		dev->data = NULL;
		return 0;
	}

static struct file_operations dev_fops =
	{
        	.owner = THIS_MODULE,
        	.open  = device_open,
        	.read  = device_read,
        	.write = device_write,
	//	.llseek = device_llseek,
        	.release = device_release,
	};

static int chardevice_init(void)
  {
	if(alloc_chrdev_region(&dev,0,1,DEVICENAME)<0)
          {
          	printk(KERN_ALERT "Character Device Number allocation  is failed\n" );
	  	return -1;
	  }
	  printk(KERN_ALERT "Dynamic Major & Minor Number: %d %d",MAJOR(dev),MINOR(dev));
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
		printk(KERN_ALERT "Kernel Module deleting Successfully...:::\n");
		unregister_chrdev(device_num, DEVICENAME);
  }

static int device_open(struct inode *inode, struct file *file)
{
		struct device_dev *dev; /* device information */
		dev = container_of(inode->i_cdev, struct device_dev, cdev);
		file->private_data = dev; /*Storing device information in private data section for future used*/
		if((file->f_flags & O_ACCMODE) == O_WRONLY)  /* now trim to 0 the length of the device if open was write-only */
		{
			 if(down_interruptible(&dev->sem))
				return -ERESTARTSYS;
			device_trim(dev); /* ignore errors */
			up(&dev->sem);
		}
  		return 0;
}


struct device_qset *device_follow(struct device_dev *dev, int n)
{
	struct device_qset *qs = dev->data;

        /* Allocate first qset explicitly if need be */
	if (! qs) {
		qs = dev->data = kmalloc(sizeof(struct device_qset), GFP_KERNEL);
		if (qs == NULL)
			return NULL;  /* Never mind */
		memset(qs, 0, sizeof(struct device_qset));
	}

	/* Then follow the list */
	while (n--) {
		if (!qs->next) {
			qs->next = kmalloc(sizeof(struct device_qset), GFP_KERNEL);
			if (qs->next == NULL)
				return NULL;  /* Never mind */
			memset(qs->next, 0, sizeof(struct device_qset));
		}
		qs = qs->next;
		continue;
	}
	return qs;
}


static ssize_t device_read(struct file *file, char __user *buff, size_t len, loff_t * ppos)
{
		struct device_dev *dev = file->private_data;
		struct device_qset *dptr;
		int quantum = dev->quantum, qset = dev->qset;
		int itemsize = quantum * qset; /* how many bytes in the listitem */
		int item, s_pos, q_pos, rest;
		ssize_t retval = 0;
		if(down_interruptible(&dev->sem))
			return -ERESTARTSYS;
		if(*ppos >= dev->size)
			goto out;
		item = (long)*ppos/itemsize;
		rest = (long)*ppos%itemsize;
		s_pos = rest/quantum; q_pos = rest % quantum;
		dptr = device_follow(dev, item);

		if (dptr == NULL || !dptr->data || ! dptr->data[s_pos])
			goto out; /* don't fill holes */

		/* read only up to the end of this quantum */
		if(len>quantum - q_pos)
			len = quantum - q_pos;
		if (copy_to_user(buff, dptr->data[s_pos] + q_pos, len)) 
			{
				retval = -EFAULT;
				goto out;
			}
		*ppos += len;
		retval = len;
		out:
			up(&dev->sem);
			return retval;
}

static ssize_t device_write(struct file *file, const char __user *buff, size_t len, loff_t *ppos)
	{
		struct device_dev *dev = file->private_data;
		struct device_qset *dptr;
		int quantum = dev->quantum, qset = dev->qset;
		int itemsize = quantum * qset;
		int item, s_pos, q_pos, rest;
		ssize_t retval = -ENOMEM; /* value used in "goto out" statements */

		if(down_interruptible(&dev->sem))
			return -ERESTARTSYS;

		/* find listitem, qset index and offset in the quantum */
		item = (long)*ppos / itemsize;
		rest = (long)*ppos % itemsize;
		s_pos = rest / quantum; q_pos = rest % quantum;

		/* follow the list up to the right position */
		dptr = device_follow(dev, item);
		if(dptr == NULL)
			goto out;
		if(!dptr->data) 
			{
			dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
			if(!dptr->data)
				goto out;
			memset(dptr->data, 0, qset * sizeof(char *));
			}
		if(!dptr->data[s_pos]) 
			{
			dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
			if (!dptr->data[s_pos])
				goto out;
			}
		/* write only up to the end of this quantum */
		if(len > quantum - q_pos)
			len = quantum - q_pos;

		if(copy_from_user(dptr->data[s_pos]+q_pos, buff, len)) {
			retval = -EFAULT;
			goto out;
		}
		*ppos += len;
		retval = len;

        /* update the size */
		if(dev->size < *ppos)
			dev->size = *ppos;

	out:
		up(&dev->sem);
		return retval;
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

