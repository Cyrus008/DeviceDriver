                                                   /*Short story on "PCI BUS INTERFACE" */
                                                  
Introduction:
	     Here we overview of the higher-level bus architectures.A bus is made up from both an electrical and a programming interface.The kernel functions{kernel module} that access
	     PCI peripherals, because these days the PCI bus is the most commonly used peripheral bus on desktops and bigger computers.The bus is the one that is best supported by the
	     kernel.

The PCI Interface:
	    PCI is actually a complete set of specifications that define how different parts of a computer should interact.The probing techniques and an autodetecting the IRQ
	    number can be used with PCI devices to find its hardware and gain access to it .The pci specification can offers an alternative of probing.The PCI architecture was designedas a replacement for the ISA standard, 
	    with five main goals:
		
		1->To get better performance when transferring data between the computer and its peripherals,
		2->To be as platform independent as possible mean it can support on many cpu architechture{x86_64,arm32/64,ppc,mips like that}
		3->To simplify adding and removing peripherals to the system.
		4->The PCI bus achieves better performance than ISA by using a higher clock rate,it can be run on factor of the system clock{25 or 33 MHz} or it can be deployed at external higher frequency {66-MHz and even 133-MHz}
		5-> The pci can be equipped with a 32-bit data bus, and a 64-bit extension has been included in the specification. 
	   
	    The pci designe to support independent platform ,due to this reason PCI is used extensively on IA-32, Alpha, PowerPC,SPARC64,and IA-64 systems,and some other platforms as well.
	    The PCI’s support "auto-detection of interface boards". The PCI devices are "jumperless and are automatically configured at boot time". Then,the device driver must be able to access configuration information in the device in
	    order to complete initialization.This happens without the need to perform any probing.

PCI Addressing:
	  The  each PCI peripheral is identified by following parameter :
	  	0-> A Domain Number
	  	1-> A bus number
		2-> A device number
		3-> A function number.

	So formate for PCI Addressing  <Domain_name:Bus_number:Device_name:Function_number>

	 
	The PCI specification permits a single system to host up to 256 buses, but because 256 buses are not sufficient for many large systems. So the Linux now supports PCI domains.Each PCI domain can host up to 256 buses, and bus 
	hosts up to 32 devices ,and each device can be a multifunction board with a maximum of eight functions. So each function can be identified at "hardware level by a 16-bit address, or key".Device drivers written for Linux, though,
       	don’t need to deal with those binary addresses, because they use a specific data structure, called pci_dev ,to act on the devices.


	Plugging more than one bus in a single system is accomplished by means of bridges, special-purpose PCI peripherals for joining two buses.The overall layout of a PCI system is a tree where each bus is connected to an upper-layer
       	bus, up to bus 0 at the root of the tree.The CardBus PC-card system is also connected to the PCI system via bridges.The 16-bit hardware addresses associated with PCI peripherals,although mostly hidden in the struct pci_dev object,
       	are still visible occasionally, especially when lists of devices are being used.

	One such situation is the output of lspci and the layout of information in "/sys/bus/pci/".The sysfs representation of PCI devices also shows this addressing scheme, with the addition of the PCI domain
       	information with the addition of the PCI domain information. When the hardware address is displayed, it can be shown as two values (an 8-bit bus number and an 8-bit device and function number).
	Consider the following example :
        +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	$ lspci     /*List all PCI devices*/
	00:00.0 Host bridge: Intel Corporation Xeon E3-1200 v6/7th Gen Core Processor Host Bridge/DRAM Registers (rev 05)
	00:02.0 VGA compatible controller: Intel Corporation HD Graphics 630 (rev 04)
	00:14.0 USB controller: Intel Corporation 200 Series/Z370 Chipset Family USB 3.0 xHCI Controller
	00:14.2 Signal processing controller: Intel Corporation 200 Series PCH Thermal Subsystem
	00:16.0 Communication controller: Intel Corporation 200 Series PCH CSME HECI #1
	00:17.0 SATA controller: Intel Corporation 200 Series PCH SATA controller [AHCI mode]
	00:1f.0 ISA bridge: Intel Corporation 200 Series PCH LPC Controller (Q270)
	00:1f.2 Memory controller: Intel Corporation 200 Series/Z370 Chipset Family Power Management Controller
	00:1f.3 Audio device: Intel Corporation 200 Series PCH HD Audio
	00:1f.4 SMBus: Intel Corporation 200 Series/Z370 Chipset Family SMBus Controller

       $ cat /proc/bus/pci/devices | cut -f1  /*list all PCI devices Address*/
		0000
		0010
		00a0
		00a2
		00b0
		00b8
		00f8
		00fa
		00fb
		00fc
		00fe




	$ ls -l /sys/bus/pci/devices/  /*Mapped all pci list*/
	  total 0
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:00.0 -> ../../../devices/pci0000:00/0000:00:00.0
	lrwxrwxrwx 1 root root 0 Jun 26 16:59 0000:00:02.0 -> ../../../devices/pci0000:00/0000:00:02.0
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:14.0 -> ../../../devices/pci0000:00/0000:00:14.0
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:14.2 -> ../../../devices/pci0000:00/0000:00:14.2
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:16.0 -> ../../../devices/pci0000:00/0000:00:16.0
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:17.0 -> ../../../devices/pci0000:00/0000:00:17.0
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:1f.0 -> ../../../devices/pci0000:00/0000:00:1f.0
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:1f.2 -> ../../../devices/pci0000:00/0000:00:1f.2
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:1f.3 -> ../../../devices/pci0000:00/0000:00:1f.3
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:1f.4 -> ../../../devices/pci0000:00/0000:00:1f.4
	lrwxrwxrwx 1 root root 0 Jun 26 16:15 0000:00:1f.6 -> ../../../devices/pci0000:00/0000:00:1f.6

	All three lists of devices are sorted in the same order, since lspci uses the /proc files as its source of information.So now i am taking the "VGA compatible controller" which address is  0010 that pointing PCI address in
       	this for "0000:00:02.0" Finally we can write :


      		<Address> --> <Domain_name:Bus_number:Device_name:Function_number>
                 0x0010		0000		00	 02		0
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	
	
	
	The hardware circuitry of each peripheral board answers queries pertaining to three address spaces:
		1->The memory locations,
	        2->The I/O ports
	        3->The configuration registers.


	The first two above address spaces are shared by all the devices on the same PCI bus .The configuration space, on the other hand, exploits geographical addressing. Configuration queries address only one slot at a time,
       	so they never collide.As far as the driver is concerned, memory and I/O regions are accessed in the usual ways via inb, readb, and so forth. Configuration transactions, on the other hand, are performed by calling specific kernel 
	functions to access configuration registers.

	With regard to interrupts, every PCI slot has four interrupt pins, and each device function can use one of them without being concerned about how those pins are routed to the CPU. Such routing is the responsibility of the 
	computer platform and is implemented outside of the PCI bus.The I/O space in a PCI bus uses a 32-bit address bus (leading to 4 GB of I/O ports),while the memory space can be accessed with either 32-bit or 64-bit addresses.

	The  every memory and I/O address region offered by the interface board can be remapped by means of configuration transactions. That is, the firmware initializes PCI hardware at system boot, mapping each region to a different 
	address to avoid collisions.The addresses to which these regions are currently mapped can be read from the configuration space, so the Linux driver can access its devices without probing. After reading the configuration registers,	      the driver can safely access its hardware.

	The PCI configuration space consists of 256 bytes for each device function and the layout of the configuration registers is standardized.Four bytes of the configuration space hold a unique function ID, so the driver can identify
       	its device by looking for the specific ID for that peripheral.




Boot Time:
	The working mechnism of PCI can be observerd by system rebooting but we need device configuration before system booting.The PCI device can be configured or without configured.When the power is applied to the PCI device then
	only configured transaction device is active and remaining is inactive.At power on, the device has :

			1-> No memory configuration in the computer’s address space .
			2-> No I/O ports mapped in the computer’s address space .
			3-> The device-specific feature, such as interrupt reporting, is disabled .
	
	The every PCI motherboard is equipped with PCI-aware firmware, called the "BIOS, NVRAM, or PROM", depending on the platform.The firmware offers access to the device configuration address space by reading and writing registers in 
	the PCI controller.At system boot, the firmware performs configuration transactions with every PCI peripheral in order to allocate a safe place for each address region it offers.By the time a device driver accesses the device,
       	its memory and I/O regions have already been mapped into the processor’s address space.

	The user can look at the PCI device list and the devices’ configuration registers by reading "/proc/bus/pci/devices" and "/proc/bus/pci/*/*". The individual PCI device directories in the sysfs tree can be found 
	in /sys/bus/pci/devices. A PCI device directory contains a number of different files:


	 $ tree  -L 1 /sys/bus/pci/devices/0000\:00\:00.0
	 /sys/bus/pci/devices/0000:00:00.0
		├── ari_enabled
		├── broken_parity_status
		├── class
		├── config
		├── consistent_dma_mask_bits
		├── d3cold_allowed
		├── device
		├── dma_mask_bits
		├── driver -> ../../../bus/pci/drivers/skl_uncore
		├── driver_override
		├── enable
		├── firmware_node -> ../../LNXSYSTM:00/LNXSYBUS:00/PNP0A08:00/INT3472:01
		├── irq
		├── local_cpulist
		├── local_cpus
		├── modalias
		├── msi_bus
		├── numa_node
		├── power
		├── remove
		├── rescan
		├── resource
		├── revision
		├── subsystem -> ../../../bus/pci
		├── subsystem_device
		├── subsystem_vendor
		├── uevent
		└── vendor


	Tree Explanation:

		1->The file config is a binary file that allows the raw PCI config information to be read from the device .
		2->The files vendor, device,subsystem_device, subsystem_vendor, and class all refer to the specific values of this PCI device .
		3->The file irq shows the current IRQ assigned to this PCI device, and the file resource shows the current memory resources allocated by this device.

	
	Configuration Registers and Initialization :

		Here we will look up the configuration registers that PCI devices contain. All PCI devices feature at least a 256-byte address space.The first 64 bytes are standardized, while the rest are device dependent.
		Here we will consider the device-independent configuration space{ standardized 64 byte } :

	        0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xA 0xB  0xC 0xD 0xE  0xF 	
	0x00
	0x10
	0x20
	0x30

	If we consider this picture carefully then we Row show{4 section} and coloum showing 16 byte .So total number of the bytes : 16 bytesx4 =64 Bytes
	
	The some of the PCI configuration registers are required and some are optional. Every PCI device must contain meaningful values in the required registers, whereas the contents of the optional registers depend on the actual
       	capabilities of the peripheral.It’s interesting to note that the PCI registers are always little-endian. Although the standard is designed to be architecture independent, the PCI designers sometimes show a slight bias toward 
	the PC environment.	


	The driver writer should be careful about byte ordering when accessing multibyte configuration registers; code that works on the PC might not work on other platforms. The Linux developers have taken care of the byte-ordering
       	problem  but the issue must be kept in mind.Describing all the configuration items is beyond the scope but we’re interested in is how a driver can look for its device and how it can accessthe device’s configuration space.

	Three or five PCI registers identify a device: vendorID , deviceID , and class are the three that are always used.Every PCI manufacturer assigns proper values to these read-only registers, and the driver can use them to look 
	for the device.Additionally, the fields subsystem vendorID and subsystem deviceID are sometimes set by the vendor to further differentiate similar devices.They represented via a strucuture :

		struct pci_device_id {
				__u32 vendor, device;		/* Vendor and device ID or PCI_ANY_ID*/
			  __u32 subvendor, subdevice;		/* Subsystem ID's or PCI_ANY_ID */
			     __u32 class, class_mask;		/* (class,subclass,prog-if) triplet */
			  kernel_ulong_t driver_data;		/* Data private to the driver */
		};

	
	There are two helper macros that should be used to initialize a " struct pci_device_id" structure:

	1-> PCI_DEVICE(vendor, device) :
		This creates a struct pci_device_id that matches only the specific vendor and device ID. The macro sets the subvendor and subdevice fields of the structure to PCI_ANY_ID .
	2->PCI_DEVICE_CLASS(device_class, device_class_mask) :
		This creates a struct pci_device_id that matches a specific PCI class.

	We using these macros to define the type of devices  driver supports can be found in the following kernel files :

	-----------------------------------------------------------------------------------------------------------------------------------
	Case 01 : drivers/usb/host/ehci-hcd.c:
					     static const struct pci_device_id pci_ids[ ] = { {
						/* handle any USB 2.0 EHCI controller */
					    PCI_DEVICE_CLASS(((PCI_CLASS_SERIAL_USB << 8) | 0x20), ~0),
						.driver_data = (unsigned long) &ehci_driver,
						},
						{ /* end: all zeroes */ }
						};
	Case02 	:drivers/i2c/busses/i2c-i810.c:
					
					tatic struct pci_device_id i810_ids[ ] = {
						{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810_IG1) },
						{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810_IG3) },
						{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810E_IG) },
						{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82815_CGC) },
						{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82845G_IG) },
						{ 0, },
						}	

	These examples create a list of struct pci_device_id structures, with an empty structure set to all zeros as the last value in the list.This array of IDs is used in the struct pci_driver (described below), and it is also used
       	to tell user space which devices this specific driver supports.


	MODULE_DEVICE_TABLE:
			This pci_device_id structure needs to be exported to user space to allow the hotplug and module loading systems to know what module works with what hardware devices.
			The macro MODULE_DEVICE_TABLE accomplishes this. An example is:

			MODULE_DEVICE_TABLE(pci, i810_ids);

			This statement creates a local variable called __mod_pci_device_table that points to the list of struct pci_device_id .The Later in the kernel build process, the "depmod program" searches all modules for the
		       	symbol __mod_pci_device_table .If that symbol is found, it pulls the data out of the module and adds it to the file /lib/modules/KERNEL_VERSION/modules.pcimap.

			After depmod completes, all PCI devices that are supported by modules in the kernel are listed, along with their module names, in that file.When the kernel tells the hotplug system that a new PCI device has 
			been found, the hotplug system uses the modules.pcimap file to find the proper driver to load.

	
	
			/*************************************************Theory Part Level 01 End ***********************************************************************/























