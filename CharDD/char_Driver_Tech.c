										Char Drivers
[Introduction] :
			A character device  can be accessed as a stream of bytes (Sequence of byte). The char driver is in charge of implementing this behavior.
			The char driver usually implemented to access open, close, read, and write system calls.The text console,serial port are the best examples
			of char devices, as they are well represented by the stream abstraction.Char devices are accessed by means of filesystem nodes, such as /dev/tty1
		       	and /dev/lp0.The char devices are just data channels, which you can only access sequentially.

[About the scull]:
 			In the LDD3 we used the scull template.The scull represent small code extracted from a real device driver.The scull is a char driver that acts on
		       	a memory area as though it was a device.Here they are treating the word "device" interchangeably with “the memory area used by scull.”

			The scull[Real device driver] is the hardware dependent.So scull just acts on some memory,allocated from the kernel. Anyone can compile and run scull,
		       	and scull is portable across the computer architectures on which Linux runs.
		

[The Design of scull]:
			The first step of driver writing is defining the capabilities that the driver offered to user programs.Since scull access part of the computer’s memory,
			So we’re free to do what we want with it. It can be a sequential,random-access , one device or many device, and so on.The scull useful as a template for writing
		       	real drivers for real devices.The scull source implements the following devices and each kind of device implemented by the module is referred to as a type.

			1->[scull0 to scull3]:
				The each consisting of a memory area that is both global[if the device is opened multiple times, the data contained within the device is shared by all the 
				file descriptors that opened it] and persistent[if the device is closed and reopened, data isn’t lost].
			2->[scullpipe0 to scullpipe3]:
				The four FIFO devices, which act like pipes.One process reads what another process writes.If multiple processes read the same device, they fight for data
				and we used the a sysnchronized method to avoid the race-condition or data corruption.Although real drivers synchronize with their devices using hardware
				interrupts.

[Major and Minor Number :]

			The special file in /dev is identifide by c .The character device in /dev/char directory is represented via device file.The device file is dev_t data type that store major and minor
		       	number .Modern Linux kernels allow multiple drivers to share major numbers .Specially major muber refer to the specific classs on driver. The minor number is used by the kernel to
		       	determine exactly which device is being referred to or differenct device that come under one major number:

					lrwxrwxrwx 1 root root  7 Nov 11 15:56 4:0 -> ../tty0
					lrwxrwxrwx 1 root root  7 Nov 11 15:56 4:1 -> ../tty1
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:10 -> ../tty10
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:11 -> ../tty11
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:12 -> ../tty12
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:13 -> ../tty13
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:14 -> ../tty14
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:15 -> ../tty15
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:16 -> ../tty16
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:17 -> ../tty17
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:18 -> ../tty18
					lrwxrwxrwx 1 root root  8 Nov 11 15:56 4:19 -> ../tty19

			On above snippt 4 number informing about the Major number(Specific class of driver) and (0,10,11,12,13,14,...) show how many device is connected with major number .


[Device Numbers]:
			The "device number" internally represent your hardware in the linux kernel.In linux kernel, the dev_t data typehold the device's both major and minor number .
			The major number represent driver sub-system and minor number show the device that attached with this particular subsytem.The dev_t is the 32-bit data type that hold :

				1-> The 12 bits set  for the Major number
				2-> The 20 bits  set for the Minor number

			So Your module never make any assumptions about the internal organization of device numbers it can be statically allocated  before compilation or dynamically allocate.
			In statically allocation you must know the major,minor number,and using this you can allocate the device file like this .

		Case01:
			In statically allocation you must know the major,minor number,and using this you can allocate the device file like this .
               		>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
					dev_t dev ;
					dev=MKDEV(int major, int minor);
              		<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		Case02:
			if you have the device number in case of dynamic allocation the you can extract the Major and Minor number using the wrapper function define in <linux/kdev_t.h>
	     		 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                        		dev_t dev ;
                        		major=MAJOR(dev_t dev);
					minor=MINOR(dev_t dev);
              		<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


[Allocation of Device Numbers]:


			The "device number" internally represent your hardware in the linux kernel, but for that we need  to register that particular device number in Linux kernel.The registration 
			of device number make an entry uncer the /proc/devices of root director .These kind of registration can be done via following way :

				>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				  1-> Statically device number  registration
				  2-> Dynamically device number  registration
				>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


		[1-> Statically device number  registration] :
 			The statically registration of device number  used the following function define in the <linux/fs.h>. This can be done followng way :

					int register_chrdev_region(dev_t first, unsigned int count,char *name);

 			[Parameter Explanation]:

			1->@first :This is device number,specified the starting rang{from which number you want start the device number it can be 0,1,34,56 ...any} but by default this value is 0 .
		        2->@count :The count is the total number of contiguous device numbers you are requesting.if count is large, the range you request could spill over to the next major number .
			3->@name  :The name of the device that should be associated with this number range; it will appear in /proc/devices and sysfs.

 			How we can do practically in coding
			===================================


			 int result ;
			     result = register_chrdev_region(10, 6,"pramodmaurya");
                         if(result < 0){
	                       printk(KERN_ALERT "Device registration failed %d",result);
				return -1 ;
				}
			printf(KERN_ALERT "Major number:%d Minor Number :%d",Major(dev),MINOR(dev));

			Return :
			1-> if the allocation was successfully then it return 0
			2-> if case of error, a negative error code will be returned



			Advantage :
			1-> The register_chrdev_region works well if you know ahead of time exactly which device numbers you want.
	   		2-> You can know device number for device file creation in advance .

			Disadvantage :
			 1->You can simply pick a number that appears to be unused but  once your driver is more widely deployed, a randomly picked major number will lead to conflicts and trouble.




		[2->Dynamically device number  registration]:

			So,as a driver writer, you have a choice: you can simply pick a number that appears to be unused. Picking up a number may work as long as the only user of your driver is you;
       			once your driver is more widely deployed, a randomly picked major number will lead to conflicts and trouble.To overcome with this problom we dynamically allocate a device number.
			The kernel will happily allocate a major number for you on the fly, but you must request this allocation by using a different function:

	 				int alloc_chrdev_region(dev_t *dev, unsigned int firstminor,unsigned int count, char *name);


 		[Parameter Explanation]:

			1->@dev : This is pointer to the device number allocated by kernel on fly
			2->@firstminor:The firstminor should be the requested first minor number to use; it is usually 0 .
			3->@count :The count is the total number of contiguous device numbers you are requesting.if count is large, the range you request could spill over to the next major number .
			4->@name  :The name of the device that should be associated with this number range; it will appear in /proc/devices and sysfs.


		[How we can do practically in coding]:
       

				dev_t dev ;
					int result;
					result = alloc_chrdev_region(&dev, 0,4,"parthu_maurya");
					if(result < 0){
	                			 printk(KERN_ALERT "Device registration failed %d",result);
						 result -1 ;
			     			}
	       				printf(KERN_ALERT "Major number:%d Minor Number :%d",Major(dev),MINOR(dev));

		[Note] :
		The above two functions allocate device numbers for your driver’s use, but they do not tell the kernel anything about what you will actually do with those numbers.Before a user-space program
		can access one of those device numbers,your driver needs to connect them to its internal functions that implement the device’s operations.

[Freening/Unregistering  the Device Number]:

	 	You should free them when they are no longer in use. Device numbers are freed using this funtion :

	 		void unregister_chrdev_region(dev_t first, unsigned int count);

		Usually this function called from the clean-up funtion(module_exit)


[Summary of dynamic/statically Allocation of Major Numbers] :

		Some major device numbers are statically assigned to the most common devices . When we statically allocate the major number then we don't know in advanced this number(major) is assigned
	       	to other driver or not.So,as a driver writer you can allocate major numbers in a dynamic manner.Picking a number may work as long as the only user of your driver is you; once your driver
	       	is more widely deployed, a randomly picked major number will lead to conflicts and trouble.




									[File Operations]

		During the device number registration we reserved some device numbers that internally represent your hardware in the linux kernel.So hardware is connnected with linux kernel but 
		we have not connected  any driver’s operations to those device numbers.So for driver’s operations related to device number we define file_operations structure that listed all the
		driver’s operations.The file_operations defined in <linux/fs.h> and is a collection of function pointers.So major method of file_operation structure is given bellow :


			struct file_operations {
        				struct module *owner;
        				loff_t (*llseek) (struct file *, loff_t, int);
        				ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
        				ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
        				long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
        				long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
        				int (*mmap) (struct file *, struct vm_area_struct *);
        				unsigned long mmap_supported_flags;
        				int (*open) (struct inode *, struct file *);
        				int (*flush) (struct file *, fl_owner_t id);
        				int (*release) (struct inode *, struct file *);
        				int (*fsync) (struct file *, loff_t, loff_t, int datasync);
        				int (*fasync) (int, struct file *, int);
        				ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
        				unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
     					unsigned (*mmap_capabilities)(struct file *);
					} __randomize_layout;

		The each open file under the "/dev/char/*" is associated with its own set of functions specified by file_operation structure.The operations in file_operation structure are convensionally
		access using system call for example :

			file_operation's Method					Convensional system Call 
			---------------------------------------------------------------------------------
				read                                             .read
				open                                             .open
				write                                            .write
				mmap                                             .mmap
				flush                                            .flush
			----------------------------------------------------------------------------------
		We can consider the file("/dev/char/*") to be an “object” and the functions operating on it to be its “methods” .

		The file_operations structure can be access by normally or using the pointer variable fops. The field in file_operation structure  point to the function in the driver implements a specific
	       	operation, or be left NULL for unsupported operations.The illusion given bellow :

			static const struct file_operations __fops = 
					{                          
				        .owner   = THIS_MODULE,                                         
     					.open    = device_open,                                     
       					.release = device_release,                                 
        				.read    = device_read,                                    
        				.write   = device_write,                                   
     					.llseek  = device_llseek,                                 
					}
	
		The following list introduces all the operations that an application can invoke on a device.The string __user in file_operation structure function denoting a pointer noting that a pointer
	       	is a user-space address that cannot be directly dereferenced.For normal compilation, __user has no effect, but it can be used by external checking software to find misuse of user-space addresses.

			struct module *owner
				The first file_operations field is not an operation at all; it is a pointer to the module that “owns” the structure.This field is used to prevent the module from
				being unloaded while its operations are in use.Almost all the time, it is simply initialized to THIS_MODULE


		The scull device driver implements only the most important device methods. Its file_operations structure is initialized as follows:

			struct file_operations scull_fops = {
					.owner  = THIS_MODULE,
					.llseek = scull_llseek,
					.read   = scull_read,
					.write  = scull_write,
					.ioctl  = scull_ioctl,
					.open   = scull_open,
					.release = scull_release,
					};
		This declaration uses the standard C tagged structure initialization syntax. This syntax is preferred because it makes drivers more portable across changes in the definitions of the structures
	       	and, arguably, makes the code more compact and readable.


								[The file Structure]

		A file descriptor is an abstract indicator used to access a file or other input/output resource, such as a pipe or network socket. A file descriptor is a non-negative integer.
		Each file descriptor present filetable/file using an integer value in operating related open from input/output resource like pipe,socket.

		struct file is the second most important data structure used in device drivers.A FILE descriptor(user space) is defined in the C library and never appears in kernel code and  struct file 
		(kernel space), on the other hand, is a kernel structure that never appears in user programs(user space).The file structure represents an open file for the device driver in kernel space.
		It is created by the kernel on open and is passed to any function that operates on the file, until the last close. After all instances of the file are closed, the kernel releases the data
		structure like in user space.In the kernel sources, a pointer to struct file is usually called either file or filp (“file pointer”).The most important fields of struct file are shown here.


				struct file {
        					union {
                				struct llist_node       fu_llist;
                				struct rcu_head         fu_rcuhead;
      							} f_u;
        					struct path             f_path;
      						struct inode            *f_inode;       /* cached value */
        					const struct file_operations    *f_op;
       						spinlock_t              f_lock;
        					enum rw_hint            f_write_hint;
       						atomic_long_t           f_count;
        					unsigned int            f_flags;
        					fmode_t                 f_mode;
        					struct mutex            f_pos_lock;
        					loff_t                  f_pos;
        					struct fown_struct      f_owner;
        					const struct cred       *f_cred;
        					struct file_ra_state    f_ra;	
        					void                    *private_data;
        					struct address_space    *f_mapping;
					} __randomize_layout
	
	    The file_operation is access by the file descriptor in linux kernel.



	    						[The inode Structure]
	  

	     The inode structure is used by the kernel internally to represent files or numerous file structures to a single inode structure.Therefore, it is different from the file structure that 
	     represents an open file descriptor.There can be numerous file structures representing multiple open descriptors on a single file, but they all point to a single inode structure.
	     The inode structure contains a great deal of information about the file. This contain only two fields of this structure are of interest for writing driver code:

	     			[dev_t i_rdev];
					    For inodes that represent device files, this field contains the actual device number.
				[struct cdev *i_cdev];
					     struct cdev is the kernel’s internal structure that represents char devices; this field contains a pointer to that structure when the inode refers to a char
					     device file.


	   						[Char Device Registration]

		The kernel uses structure "struct cdev" to represent char devices internally.You need allocate and register this structures, before the kernel invokes your device’s operations.
		The struct cdev include <linux/cdev.h> header file.There are two ways of allocating and initializing one of these structures.

		[Case01]:
			If you wish  to obtain a standalone cdev structure at runtime:

				struct cdev *my_cdev = cdev_alloc( );
				my_cdev->ops = &my_fops;
		[Case02]:
			You want to embed the cdev structure within a device-specific structure[struct file_operation] of your own by storing all device-specific operation in the cdev structure
			using the cdev_init function.Declaration and initalization of cdev_init function given bellow:

				void cdev_init(struct cdev *cdev, struct file_operations *fops);
			/*  @ Setting an cdev entry
			 *  @ Initializes @cdev, remembering/storing @fops, making it ready to add to the
			 *   void cdev_init(struct cdev *cdev, const struct file_operations *fops)
			 *       {
			 *          memset(cdev, 0, sizeof *cdev);
			 *          INIT_LIST_HEAD(&cdev->list);
			 *          kobject_init(&cdev->kobj, &ktype_cdev_default);
			 *          cdev->ops = fops;
			 *        }
			 * */
			Once the cdev structure is set up, the final step is to tell the kernel about it with a call function that is given bellow:

				int cdev_add(struct cdev *dev, dev_t num, unsigned int count);
			/**
 			* cdev_add() - add a char device to the system
			* @p: the cdev structure for the device
 			* @dev: the first device number for which this device is responsible
			* @count: the number of consecutive minor numbers corresponding to this device
			* cdev_add() adds the device represented by @p to the system, making it live immediately. A negative error code is returned on failure.
			* 
			* int cdev_add(struct cdev *p, dev_t dev, unsigned count)
			* {
			*	int error;
			*	p->dev = dev;
			*	p->count = count;
			*	error = kobj_map(cdev_map, dev, count, NULL,exact_match, exact_lock, p);
			*	if (error)
			*		return error;
			*	kobject_get(p->kobj.parent);
			*	return 0;
			*
			* }
 			*/

			So scull_setup_cdev must be initialized cdev structure and memorized the device specific structure in cdev and add the device to the linux kernel.
			The complete code is given bellow:

			/*
			* static void scull_setup_cdev(struct scull_dev *dev, int index)
			* {	
			* 	int err, devno = MKDEV(scull_major, scull_minor + index);
			* 	cdev_init(&dev->cdev, &scull_fops);
			* 	dev->cdev.owner = THIS_MODULE;
			* 	dev->cdev.ops = &scull_fops;
			* 	err = cdev_add (&dev->cdev, devno, 1);
			* 	if (err)
			* 	printk(KERN_NOTICE "Error %d adding scull%d", err, index);
			* }
			* */
			
						[########## Open and Release ################]

			[The open Method]:

			This is the first operation performed by driver on the device file or device,the driver is not required to declare a corresponding method.The open method
			used by the driver for initialization of device/device file for the later operations(read,write,llseek..etc).In most drivers, open should perform the 
			following tasks:
		       		1-> Check for device-specific errors(such as device-not-ready because it may be accessing by other driver OR Any hardware problem).
				2-> Initialize the device if it is being opened for the first time(Setting the read,write,or both permission,mouting) .
				3-> Update the "struct file_operation" f_op pointer, if necessary.
				4-> Allocate and filled the device specific data structure in to filp->private_data of device because kernel never remember this area.
			
			The prototype and dummy function for the open method is	given bellow :

				int (*open)(struct inode *inode, struct file *filp);

			/* int scull_open(struct inode *inode, struct file *filp)
			 * {
			 *  	struct scull_dev *dev; // device information specific structure
			 *  	dev = container_of(inode->i_cdev, struct scull_dev, cdev); //Best method to access device[cdev] in kernel using the scull_dev structure
			 *  	filp->private_data = dev;         //Refreshing the private data section of device 
			 *  	if ( (filp->f_flags & O_ACCMODE) = = O_WRONLY) {   // Trimming the length of the device to 0 if if open was write-only 
			 *  		scull_trim(dev);  // Ignore errors
			 *		}
			 *	return 0; success 
			 * */
			[The close Method]:

			The role of the release method is the reverse of open.You’ll find that the method implementation is called device_close instead of device_release .
			The device method should perform the following tasks for close method:
					1-> Deallocate anything that open allocated in filp->private_data.
					2-> Shut down the device on last close
			The basic form of scull has no hardware to shut down, so the code required is minimal: 
				/*int scull_release(struct inode *inode, struct file *filp)
				*{
				*return 0;
				*}
				**/

							[$$$$$$$$$  read and write $$$$$$$]

			The read and write methods both perform a similar task, that is, copying data from and to application code. Therefore, their prototypes are pretty similar:

				/*@flip->  The filp is the file pointer poiting the file descriptor in linux kernel
				 *@count-> The size of the requested data transfer
				 *@buff -> The buff argument points to the user buffer holding the data to be written or the empty buffer where the newly read data should be placed.
				 *@offp->  A pointer to a “long offset type” object that indicates the file position the user is accessing.
				 * */
				
				ssize_t read(struct file *filp, char __user *buff,size_t count, loff_t *offp);
				ssize_t write(struct file *filp, const char __user *buff,size_t count, loff_t *offp);

			The buff argument in the read and write methods is a user-space pointer cannot be directly dereferenced by kernel code. There are a few
			reasons for this restriction:
				1->The user-space pointer may not be valid while running in kernel mode at all based on kernel configuration and architecture.There may be no mapping for that address
				   or it could point to some other, random data.
				2->The paging also generated for user-space memory and the memory might not be resident in RAM when the system call is made.Attempting to reference the user-space memory
			           directly could generate a page fault or “oops,” which would result in the death of the process that made the system call.
				3->The pointer in supplied by a user program, which could be buggy or malicious.If your driver ever blindly dereferences a user-supplied pointer, it provides an open doorway 
				   allowing a user-space program to access or overwrite memory anywhere in the system.This compromising the security of your users’ systems.
			
			Derefrensing the user-space pointer by special, kernel-supplied functions in order to be safe which are defined in (<asm/uaccess.h>).The prototype of these function is given bellow
						
					unsigned long copy_to_user(void __user *to,const void *from,unsigned long count);
					unsigned long copy_from_user(void *to,const void __user *from,unsigned long count);
			Although these functions behave like normal memcpy functions, a little extra care must be used when accessing user space from kernel code.


			[The Read Method]:
					During the read method data is transfered between the kernel and user-space.The return value for read is interpreted by the calling application program:
					The read method perform  following task:
					1->{return value = count argument} 
					  If the return value read equals the count argument passed to the read system call, the requested number of bytes has been transferred. This is the optimal case.
					2->{return value < count argument}
					  If the value is positive, but smaller than count , only some part of the data has been transferred.
					3->{return value = 0}
					  If the value is 0 , end-of-file was reached (and no data was read).
					4->A negative value means there was an error. The value specifies what the error was, according to <linux/errno.h>
			Here is the dummy code for read function :

			/* ssize_t scull_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos)
			 *  {
			 *     struct scull_dev *dev = filp->private_data;
			 *     struct scull_qset *dptr;
			 *     int quantum = dev->quantum, qset = dev->qset;
			 *     int itemsize = quantum * qset;
			 *     int item, s_pos, q_pos, rest;
			 *     ssize_t retval = 0;
			 *
			 *
			 *
			 *
			 * */

			[The write Method]:
				The write, like read, can transfer less data than was requested, according to the following rules for the return value:
					1->If the value equals count , the requested number of bytes has been transferred.
					2->If the value is positive, but smaller than count , only part of the data has been transferred. The program will most likely retry writing the rest of the data.
					3->If the value is 0 , nothing was written. This result is not an error, and there is no reason to return an error code. Once again, the standard library retries
				       	the call to write.
					4->A negative value means an error occurred; as for read, valid error values are those defined in <linux/errno.h>.



					[***************** Happy Reading this end of character device driver section ***************************]

		













