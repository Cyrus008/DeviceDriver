#ifndef _CHARHEADER_H
#define _CHARHEADER_H
#define DEVICENAME    "char_dev0"

#ifndef DEVICE_QSET
#define DEVICE_QSET    1000
#endif


static int device_num=0;
static dev_t dev=0;
static struct cdev pdev_cdev;
static int device_open(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static int device_release(struct inode *, struct file *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);
//int  device_trim(struct device_dev *dev);
struct device_qset {
	void **data;
	struct device_qset *next;
};

struct device_dev {
	struct device_qset *data;  /* Pointer to first quantum set */
	int quantum;              /* the current quantum size */
	int qset;                 /* the current array size */
	unsigned long size;       /* amount of data stored here */
	unsigned int access_key;  /* used by sculluid and scullpriv */
	struct semaphore sem;     /* mutual exclusion semaphore     */
	struct cdev cdev;	  /* Char device structure		*/
};
#endif
