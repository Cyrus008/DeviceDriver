						/*Input Output Controller */
Introduction about IOCTL :
========================

  	A complete device driver that the user can write to and read from. But a real device usually offers more functionality than synchronous read and write.
  	Now that we’re equipped with debugging tools that make disaster the things.The better understanding of "concurrent issues" by the firmware to help keep
  	the things from going awry{disaster}. 
  	We start with implementing the ioctl system call, that provides common interface used for device control.We proceed to various ways of synchronizing with user
	space;by the end of this chapter you have a good idea of how to put processes to sleep (and wake them up), implement nonblocking I/O, and inform user space when
	your devices are available for reading or writing  .
  
  
ioctl:
======
    	Most drivers additionally need the ability to read and write the device .This ability to perform various types of hardware control via the device driver.
   	Most devices can perform operations beyond simple data transfers.The user space must often be able to request,{for example, that the device lock its door,
	eject its media} report error information, change a baud rate, or self destruct. These operations are usually supported via the ioctl method, which implements 
	the system call by the same name. In user space, the ioctl system call has the following prototype:
    
    			        int ioctl(int fd, unsigned long cmd, ...);


Intresting fact about ioctl() system call :
===========================================
	The prototype stands out in the list of Linux system calls because of the dots which usually mark the function as having a variable number of arguments.
	In a real system, however, a system call can’t actually have a variable number of arguments.

	System calls must have a well-defined prototype, because user programs can access them only through hardware “gates.” Therefore, the dots in the prototype
	represent not a variable number of arguments but a single optional argument, traditionally identified as char *argp .The dots are simply there to prevent type
	checking during compilation. The actual nature of the third argument depends on the specific control command being issued (the second argument).

	Some commands take no arguments, some take an integer value, and some take a pointer to other data. Using a pointer is the way to pass arbitrary data to the 
	ioctl call; the device is then able to exchange any amount of data with user space.Due to unstructured nature of ioctl its fall out of favor among kernel
	developers.Each ioctl command is, essentially, a separate, usually undocumented system call, and there is no way to audit these calls in any sort of 
	comprehensive manner.It is also difficult to make the unstructured ioctl arguments work identically on all systems;

	Possible alternatives include embedding commands{copy data from and to user space} into the data stream or using virtual filesystems, either sysfs or
	driver-specific filesystems.However, the fact remains that ioctl is often the easiest and most straightforward choice for true device operations.
	The ioctl driver method has a prototype that differs somewhat from the user-space version:

			int (*ioctl) (struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);

             
	Intresting Fact:
	++++++++++++++++
		1->The inode and filp pointers value are corresponding to the file descriptor fd and passed by the application same parameters passed to the
	       	   open method.
		2-> The cmd argument is passed from the user unchanged
		3-> The optional argargument is passed in the form of an unsigned long,regardless of whether it was given by the user as an integer or a pointer.

	
	If the invoking program doesn’t pass a third argument, the arg value received by the driver operation is undefined. Because type checking is disabled on the
       	extra argument, the compiler can’t warn you if an invalid argument is passed to ioctl, and any associated bug would be difficult to spot.
		
Choosing the ioctl Commands:
============================

	The ioctl command numbers should be unique across the system in order to prevent errors caused by issuing the right command to the wrong device. Such a mismatch
       	is not unlikely to happen, and a program might find itself trying to change the baud rate of a non-serial-port input stream, such as a FIFO or an audio device
	The header defines the bitfields you will be using:

		1->Type (magic numberi)
		2->Ordinal number
		3->Direction of transfer
	        4->The size of argument.

	The define ioctl command numbers uses four bitfields, which have the following meanings. New symbols introduced in this list are defined in <linux/ioctl.h>.

	type :
	     The magic number.Just choose one number and use it throughout the driver. This field is eight bits wide ( _IOC_TYPEBITS ).
	number:
	    The ordinal (sequential) number. It’s eight bits ( _IOC_NRBITS ) wide.
	direction:
	    The direction of data transfer, if the particular command involves a data transfer.The possible values are given bellow :
	    	1-> _IOC_NONE : (no data transfer)
		2-> _IOC_READ : (from the device,so the driver must write to user space)
		3->,_IOC_WRITE: (To the device,so the driver must read from user space)
	        4->_IOC_READ|_IOC_WRITE : (data is transferred both ways).
		3-> _IOC_READ and _IOC_WRITE can be extracted using a logical AND operation.
	size:
	  The size of user data involved. The width of this field is architecture dependent,but is usually 13 or 14 bits. You can find its value for your specific
	  architecture in the macro _IOC_SIZEBITS . It’s not mandatory that you use the size field—the kernel does not check it—but it is a good idea.
	  The size of user data involved. The width of this field is architecture dependent,but is usually 13 or 14 bits. You can find its value for your specific 
	  architecture in the macro _IOC_SIZEBITS . It’s not mandatory that you use the size field—the kernel does not check it—but it is a good idea.
	

	  The header file for ioctl define in the <linux/ioctl.h>, defines macros that help set up the command numbers as follows:

		1-> _IO(type,nr) (for a command that has no argument)
	  	2-> _IOR(type,nr,datatype) (for reading data from the driver)
	  	3-> _IOW(type,nr,datatype) (for writing data) 
	  	4-> _IOWR(type,nr,datatype) (for bidirectional transfers).



       
