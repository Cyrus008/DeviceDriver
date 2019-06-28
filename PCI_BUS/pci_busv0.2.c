									++++++++++++++++++++++++++++++++++++++++++++
										 PCI-BUS V0.02 VERSION
								        ++++++++++++++++++++++++++++++++++++++++++++


The main pci structure exp :
----------------------------
	The main structure that all PCI drivers must used in order to  registered with kernel is the "struct pci_driver structure". This structure consists of a number of function callbacks and variables that describe 
	the PCI driver of PCI core{ardware}.Here are the fields in this structure that a PCI driver needs to be aware of:

			struct pci_driver {
					struct list_head node;
					const char   *name;
					const struct pci_device_id *id_table;					/* Must be non-NULL for probe to be called */
					int  (*probe)(struct pci_dev *dev, const struct pci_device_id *id);	/* New device inserted */
					void (*remove)(struct pci_dev *dev);					/* Device removed (NULL if not a hot-plug capable driver) */
					int  (*suspend)(struct pci_dev *dev, pm_message_t state);		/* Device suspended */
					int  (*suspend_late)(struct pci_dev *dev, pm_message_t state);
					int  (*resume_early)(struct pci_dev *dev);
					int  (*resume)(struct pci_dev *dev);					/* Device woken up */
					void (*shutdown)(struct pci_dev *dev);
					int  (*sriov_configure)(struct pci_dev *dev, int num_vfs); 		/* On PF */
					const struct pci_error_handlers *err_handler;
					const struct attribute_group **groups;
					struct device_driver driver;
					struct pci_dynids   dynids;
				};
	Structure member explanation :
	------------------------------
	1-> const char *name;
			    The name of the driver. It must be unique among all PCI drivers in the kernel, and is normally as module name of the driver.It shows up in sysfs under /sys/bus/pci/drivers/ when the driver is in the kernel.
			     p8@pramod:~$ ls -l /sys/bus/pci/drivers  

			     total 24
				drwxr-xr-x 2 root root 0 Jun 27 18:17 agpgart-intel
				drwxr-xr-x 2 root root 0 Jun 27 18:17 agpgart-via
				drwxr-xr-x 2 root root 0 Jun 27 18:17 ahci
				drwxr-xr-x 2 root root 0 Jun 27 18:17 asiliantfb
				drwxr-xr-x 2 root root 0 Jun 27 18:17 ata_generic
				drwxr-xr-x 2 root root 0 Jun 27 18:17 ata_piix
				drwxr-xr-x 2 root root 0 Jun 27 18:17 e1000e
				drwxr-xr-x 2 root root 0 Jun 27 18:17 ehci-pci
				drwxr-xr-x 2 root root 0 Jun 27 18:17 i915
				drwxr-xr-x 2 root root 0 Jun 27 18:17 imsttfb
				drwxr-xr-x 2 root root 0 Jun 27 18:17 iosf_mbi_pci
				drwxr-xr-x 2 root root 0 Jun 27 18:17 mei_me
				drwxr-xr-x 2 root root 0 Jun 27 18:17 ohci-pci
				drwxr-xr-x 2 root root 0 Jun 27 18:17 parport_pc
				drwxr-xr-x 2 root root 0 Jun 27 18:17 pata_sis
				drwxr-xr-x 2 root root 0 Jun 27 18:17 pcieport
				drwxr-xr-x 2 root root 0 Jun 27 18:17 pwm-lpss
				drwxr-xr-x 2 root root 0 Jun 27 18:17 serial
				drwxr-xr-x 2 root root 0 Jun 27 18:17 shpchp
				drwxr-xr-x 2 root root 0 Jun 27 18:17 skl_uncore
				drwxr-xr-x 2 root root 0 Jun 27 18:17 snd_hda_intel
				drwxr-xr-x 2 root root 0 Jun 27 18:17 uhci_hcd
				drwxr-xr-x 2 root root 0 Jun 27 18:17 virtio-pci
				drwxr-xr-x 2 root root 0 Jun 27 18:17 xen-platform-pci
				drwxr-xr-x 2 root root 0 Jun 27 18:17 xhci_hcd

	2-> const struct pci_device_id *id_table;
			Pointer to the struct pci_device_id table described earlier in this chapter.This is access via the pci_driver structure.This structure given bellow :


			struct pci_device_id {
					__u32 vendor, device;		/* Vendor and device ID or PCI_ANY_ID*/
					__u32 subvendor, subdevice;	/* Subsystem ID's or PCI_ANY_ID */
					__u32 class, class_mask;	/* (class,subclass,prog-if) triplet */
					kernel_ulong_t driver_data;	/* Data private to the driver */
					};
     	3-> Callback funtion in pci_driver structure 
     			
     
     				int  (*probe)(struct pci_dev *dev, const struct pci_device_id *id);	/* New device inserted */
				void (*remove)(struct pci_dev *dev);	/* Device removed (NULL if not a hot-plug capable driver) */
				int  (*suspend)(struct pci_dev *dev, pm_message_t state);	/* Device suspended */
				int  (*suspend_late)(struct pci_dev *dev, pm_message_t state);
				int  (*resume_early)(struct pci_dev *dev);
				int  (*resume)(struct pci_dev *dev);	/* Device woken up */
				void (*shutdown)(struct pci_dev *dev);
				int  (*sriov_configure)(struct pci_dev *dev, int num_vfs); /* On PF */
			
			

	4-> struct list_head node;
			The linkage struct for list nodes. This struct must be part of your to-be-linked struct. struct list_head is required for both the head of the list and for each list node.

Registering a PCI Driver :
-------------------------
	To register the struct pci_driver with the PCI core, a call to pci_register_driver is made with a pointer to the struct pci_driver .This is traditionally done in the module initialization code for the PCI driver:
	

			#include <linux/pci.h>	
		
			static int __init pci_skel_init(void)
			{
			  return pci_register_driver(&pci_driver);
			}

	The pci_register_driver function either returns a negative error number or 0 if everything was registered successfully.It does not return the number of devices that were bound to the driver or an error number if no devices
       	were bound to the driver.When the PCI driver is to be unloaded, the struct pci_driver needs to be unregistered from the kernel. This is done with a call to pci_unregister_driver. When this call happens, any PCI devices that
       	were currently bound to this driver are removed, and the remove function for this PCI driver is called before the pci_unregister_driver function returns.

			static void __exit pci_skel_exit(void)
			{
				pci_unregister_driver(&pci_driver);
			}


Enabling the PCI Device :
-------------------------
	Initialize device before it's used by a driver. Ask low-level code to enable I/O and memory. Wake up the device if it was suspended. We don't actually enable the device many times if we call this function repeatedly 
	(we just increment the count).In the probe function for the PCI driver, before the driver can access any device resource (I/O region or interrupt) of the PCI device, the driver must call the pci_enable_device function:

	Synopsis:
			int pci_enable_device(struct pci_dev *dev);
	
	Practical emplimentation :

			static int probe(struct pci_dev *dev, const struct pci_device_id *id)
			 {
				 if(pci_enable_device(dev)) 
				 {
                                    dev_err(&dev->dev, "Can't enable PCI device ...Horrible\n");
				    return -ENODEV;
				 }
				if (skel_get_revision(dev) == 0x42)
				    return -ENODEV;
			return 0 ;
			}

	This function actually enables the device. It wakes up the device and in some cases also assigns its interrupt line and I/O regions.


Accessing the Configuration Space :
-----------------------------------
	After the driver has detected the device, it usually needs to read from or write to the three address spaces:
	
		1-> The memory, port, and configuration.
	
	In particular, accessing the configuration space is important  to the driver,because it is the only way it can find out where the device is mapped in memory and in the I/O space .The microprocessor has no way to access the
       	configuration space directly,the computer vendor has to provide a way to do it. To access configuration space,the CPU must write and read registers in the PCI controller, but the exact implementation is vendor dependent.
	As far as the driver is concerned, the configuration space can be accessed through 8-bit, 16-bit, or 32-bit data transfers.The relevant functions are prototyped in <linux/pci.h>:

	Reading from PCI controller :
	============================
					1->int pci_read_config_byte(struct pci_dev *dev, int where, u8 *val);
					2->int pci_read_config_word(struct pci_dev *dev, int where, u16 *val);
					3->int pci_read_config_dword(struct pci_dev *dev, int where, u32 *val);

	These reading funtion read one, two, or four bytes from the configuration space of the device identified by dev .The where argument is the byte offset from the beginning of the configuration space.
	The value fetched from the configuration space is returned through the val pointer, and the return value of the functions is an error code. The word and dword functions convert the value just read from little-endian to	
	the native byte order of the processor .

	Writing to PCI controller :
        ============================
					1-> int pci_write_config_byte(struct pci_dev *dev, int where, u8 val);
					2-> int pci_write_config_word(struct pci_dev *dev, int where, u16 val);
					3-> int pci_write_config_dword(struct pci_dev *dev, int where, u32 val);

	Write one, two, or four bytes to the configuration space.The device is identified by dev as usual, and the value being written is passed as val .The word and dword functions convert the value to little-endian before writing 
	to the peripheral device.All of the previous functions{Reading/Writing} are implemented as inline functions that really call the following functions .

         Reading from PCI BUS :
         ============================
	 pci_read and pci_write interanlly used the these funtion that given bellow .Feel free to use these functions instead of the above in case the driver does not have access to a struct pci_dev at any paticular moment in time:


				1-> int pci_bus_read_config_byte (struct pci_bus *bus, unsigned int devfn, int where, u8 *val);
				2-> int pci_bus_read_config_word (struct pci_bus *bus, unsigned int devfn, int where, u16 *val);
				3-> int pci_bus_read_config_dword (struct pci_bus *bus, unsigned int devfn,int where, u32 *val);

	Here everything same as	the pci_read_functions, but struct pci_bus * and devfn variables used  instead of a struct pci_dev * .
	
	 Writing from PCI BUS :
         =======================
	Just like the pci_write_ functions, but struct pci_bus * and devfn variables are needed instead of a struct pci_dev * .

				1-> int pci_bus_write_config_byte (struct pci_bus *bus, unsigned int devfn, int where, u8 val);
	 			2-> int pci_bus_write_config_word (struct pci_bus *bus, unsigned int devfn, int where, u16 val);
				3-> int pci_bus_write_config_dword (struct pci_bus *bus, unsigned int devfn, int where, u32 val);
	
	The best way to address the configuration variables using the pci_read_ functions is by means of the symbolic names defined in <linux/pci.h>.For example, the following small function retrieves the revision ID of a device 
	by passing the symbolic name for where to pci_read_config_byte:

			static unsigned char skel_get_revision(struct pci_dev *dev)
			   {
				u8 revision;
				pci_read_config_byte(dev, PCI_REVISION_ID, &revision);
				return revision;
			  }


Accessing the I/O and Memory Spaces :
------------------------------------

	

















