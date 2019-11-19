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














[The Internal Representation of Device Numbers]:
			In linux kernel, the dev_t data type{#include<linux/types.h>}  hold the device's both major and minor number .The dev_t is the 32-bit data type that hold :
				1-> The 12 bits set  for the Major number
				2-> The 20 bits  set for the Minor number
			So Your module should  never make any assumptions about the internal organization of device numbers it can be statically allocated  before compilation or dynamically allocate.In
		       	statically allocation you must know the major,minor number,and using this you can allocate the device file like this .

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


[Allocating the  Device Numbers]:


			The "device number" internally represent your hardware in the linux kernel, but for that we need  to register that particular device number in Linux kernel.The registration of device 
			number make an entry uncer the /proc/devices of root director .These kind of registration can be done via following way :

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




2->Dynamically device number  registration:
==========================================
		So,as a driver writer, you have a choice: you can simply pick a number that appears to be unused. Picking up a number may work as long as the only user of your driver is you;
       		once your driver is more widely deployed, a randomly picked major number will lead to conflicts and trouble.To overcome with this problom we dynamically allocate a device number.
		The kernel will happily allocate a major number for you on the fly, but you must request this allocation by using a different function:

	 		int alloc_chrdev_region(dev_t *dev, unsigned int firstminor,unsigned int count, char *name);


 	Parameter Explanation:
	-----------------------
		1->@dev : This is pointer to the device number allocated by kernel on fly
		2->@firstminor:The firstminor should be the requested first minor number to use; it is usually 0 .
		3->@count :The count is the total number of contiguous device numbers you are requesting.if count is large, the range you request could spill over to the next major number .
		4->@name  :The name of the device that should be associated with this number range; it will appear in /proc/devices and sysfs.


	How we can do practically in coding:
        ===================================

		dev_t dev ;
		int result;
			result = alloc_chrdev_region(&dev, 0,4,"parthu_maurya");
			if(result < 0){
	                	 printk(KERN_ALERT "Device registration failed %d",result);
				 result -1 ;
			     }
	       printf(KERN_ALERT "Major number:%d Minor Number :%d",Major(dev),MINOR(dev));

Note :
		The above functions allocate device numbers for your driver’s use, but they do not tell the kernel anything about what you will actually do with those numbers.Before a user-space program can
	       	access one of those device numbers,your driver needs to connect them to its internal functions that implement the device’s operations.


Freening/Unregistering  the Device Number:
==========================================
	 	You should free them when they are no longer in use. Device numbers are freed using this funtion :

	 		void unregister_chrdev_region(dev_t first, unsigned int count);

		Usually this function called from the clean-up funtion(module_exit)


Summary of dynamic/statically Allocation of Major Numbers :
===========================================================
		Some major device numbers are statically assigned to the most common devices . When we statically allocate the major number then we don't know in advanced this number(major) is assigned
	       	to other driver or not.So,as a driver writer you can allocate major numbers in a dynamic manner.Picking a number may work as long as the only user of your driver is you; once your driver
	       	is more widely deployed, a randomly picked major number will lead to conflicts and trouble.

