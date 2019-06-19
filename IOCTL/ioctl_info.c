 								/******************************************/
									 I/O Control in Linux IOCTL
								/*******************************************/
Introduction :
	     Operating system put separate virtual memory into kernel space and user space.The kernel space is strictly reserved for running the "kernel, kernel extensions, and most device drivers".
	     The user space is the memory area where all "user mode applications" work and this memory can be swapped out when necessary.There are many ways to Communicate between the User space and Kernel Space, they are:
	     
	     				1->IOCTL
	     				2->Procfs
					3->Sysfs
					4->Configfs
					5->Debugfs
					6->Sysctl
					7->UDP Sockets
					8->Netlink Sockets

	    The  ioctl() system call manipulates the "underlying device parameters" of special files{device driver}.The ioctl is referred as Input and Output Control that is used to talking the device driver{Indirectly Hardware} .
            So using the ioctl system call we controlled the hardware under a particular device driver from user space .For some specific operation the kernel does not provide any system call by default in this scenario ioctl come 
	    in the picture .Some real time applications of ioctl is given bellow :
			
			1-> Ejecting the media from a “cd” drive
	    		2-> Change the Baud Rate of Serial port		
			3-> The volume control for an audio device, display configuration for a video device. 
			4-> Controlling the External hardware 
			5-> The most common hardware-controlling operation done via ioctl and  Reading or Writing device registers 

	
	   Input/Output Control (ioctl, in short) is a common operation, or system call, available in most driver categories. It will fits all kind of system call. If there is no other system call that meets a particular requirement, 
	   then ioctl() is the one to use. basically, anything to do with device input/output, or device-specific operations, yet versatile enough for any kind of operation .

Steps involved in IOCTL:
========================
		There are some steps involved to use IOCTL that given bellow :

		1-> Create IOCTL command in driver
		2-> Write IOCTL function in driver
		3-> Create IOCTL command in User space application
		4-> Use IOCTL system call in User space

	
	  Now we are going to discuss about the linux kernel system .


Create IOCTL Command in Driver:
===============================
		To implement a new ioctl command we need to follow the following steps.

		1-> Define the ioctl code :
			
			#define "ioctl_name" __IOX("magic number","command number","argument type")
		Argument :
			where IOX can be :
				1.1->   “IO”: an ioctl with no parameters
				1.2->  “IOW”: an ioctl with write parameters (copy_from_user)
				1.3->  “IOR”: an ioctl with read parameters (copy_to_user)
				1.4-> “IOWR”: an ioctl with both write and read parameters
				1.5-> The Magic Number is a unique/character/major numbe  that will differentiate our set of ioctl calls from the other ioctl calls.
				1.6-> The Command Number is the number that is assigned to the ioctl . This is used to differentiate the commands from one another.
				1.7-> The last is the type of data.

		2->Add the header file linux/ioctl.h to make use of the above mentioned calls{ioctl_name} . For example :

			#include <linux/ioctl.h>
			
			#define WR_VALUE _IOW('a','a',int32_t*)
			#define RD_VALUE _IOR('a','b',int32_t*)

Write IOCTL function in driver:
===============================
	
			#include <linux/ioctl.h>
 
			#define WR_VALUE _IOW('a','a',int32_t*)
			#define RD_VALUE _IOR('a','b',int32_t*)
                                                              
			static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
				{
         			   switch(cmd) {
                				case WR_VALUE:
                      						copy_from_user(&value ,(int32_t*) arg, sizeof(value));
                        					printk(KERN_INFO "Value = %d\n", value);
                        					break;
						case RD_VALUE:
                        					copy_to_user((int32_t*) arg, &value, sizeof(value));
                        					break;
        					}
       				 return 0;
				}


			static struct file_operations fops =
			{
			 .owner = THIS_MODULE,
			 .read = etx_read,
			 .write = etx_write,
			 .open = etx_open,
			 .unlocked_ioctl = etx_ioctl,
			 .release = etx_release,
			};

			static int __init ioctl_init(void)
	                {
	

                        }



















