					/*
					 *       Network document initial draft 0.1
					 *
					 *
					 * */



Network interface :
		    The network interface is a software or hardware interface between two pieces of equipment or protocol layers in a computer network.
		    A network interface will usually have some form of network address  This may consist of a node identifier and a port number or may be a unique node ID in its own right.
		    A network interface is the point of interconnection between a computer and a private or public network. A network interface is generally a network interface card (NIC), but does not have to have a physical form.
		    Instead, the network interface can be implemented in software.

How snull Is Designed:
		   Here we discusses the design concepts of snull network interface. Although this design information might appear as the marginal use,failing to understand this design  might lead to problems when you play with the 
		   sample code.The sample interface created via design decision should remain independent of real hardware,just like most of the sample code used in this document .This constraint led to something that resembles the 
		   loopback interface.

		   The snull is not a loopback interface; however, it simulates conversations with real remote hosts in order to better demonstrate the task of writing a network driver.Another feature of snull is that it supports 
		   only IP traffic.This is a consequence of the internal workings of the interface 



Assigning IP Numbers:
		The snull module creates two interfaces. These interfaces are different from a simple loopback, in that whatever you transmit through one of the interfaces loops back to the other one, not to itself.



The Physical Transport of Packets:
		According to the data transport the snull interfaces belong to the Ethernet class.The snull emulates Ethernet based on Ethernet technology be it 10base-T, 100base-T, or Gigabit.The the kernel offers some generalized
		support for Ethernet devices, and there’s no reason not to use it.The advantage of being an Ethernet device is so strong that even the plip interface declares itself as an Ethernet device.
		The last advantage of using the Ethernet setup for snull is that you can run tcpdump on the interface to see the packets go by. Watching the interfaces with tcpdump can be a useful way to see how the two interfaces work.


Device Registration :
======================
		When a driver module is loaded into a running kernel, it requests resources and offers facilities .The driver should probe for its device and its hardware location (I/O ports and IRQ line)but not register them.
		The way a network driver is registered by its module initialization function is different from char and block drivers.

		Since there is no equivalent of major and minor numbers for network interfaces, a network driver does not request such a number.Instead, the driver inserts a data structure for each newly detected interface into a global 
		list of network devices.Each interface is described by a "struct net_device" item, which is defined in <linux/netdevice.h>. The snull driver keeps pointers to two of these structures (for sn0 and sn1 ) using array.


				struct net_device *snull_devs[2];  //Here snull_dev is pointer array that store the two pointer

		The net_device structure contains a kobject so that reference-counted and exported via sysfs.This structure is dynamically allocated at run time .The kernel function provided a funtion alloc_netdev for dynamically 
	       	allocation of net_device structure. Funtion prototype given bellow:


			struct net_device *alloc_netdev(int sizeof_priv, const char *name, void (*setup)(struct net_device *));
		 Argument explanation :
		  
		 1->sizeof_priv :
		 		The sizeof_priv is the size of the driver’s “private data” area of network devices,that area is allocated along with the net_device structure.In fact, the two are allocated together in one large chunk 
				of memory .
		 2->name   :
		 		The name is the name of this interface, as is seen by user space; this name can have a printf-style %d in it.
		 3->setup :		
		 		The setup is a funtion pointer to an initialization function that is called to set up the rest of the net_device structure.


		Real time demonstration :
		++++++++++++++++++++++++
				#include<linux/netdevice.h>
				struct net_device *zurico_devs[2];    /*Here zurico_devs is the structure arrya point that store this address*/	

				static int __init zurico_init_module(void)
				{
				    struct snull_priv zurico_priv;  /*This structure is private to each device. It is used to pass packets in and out, so there is place for a packet*/
				    int result, i, ret = -ENOMEM;   /*Here ENOMEM is called out of memory this is one value that we initalized at starting */
				    zurico_devs[0]=alloc_netdev(sizeof(struct zurico_priv), "sn%d",snull_init);	/*This function allocated the net_device structure dynamically for one device on success it return addess*/
				    zurico_devs[1]=alloc_netdev(sizeof(struct zurico_priv), "sn%d",snull_init);	/*This function allocated the net_device structure dynamically for one device on success it return addess*/
				    if (zurico_devs[0] == NULL || zurico_devs[1] == NULL)
					return -1 ;

				    	ret = -ENODEV;

				/*Once the net_device structure has been initialized, completing the process is just a matter of passing the structure to register_netdev.*/
					for (i = 0; i < 2;  i++)
						if ((result = register_netdev(snull_devs[i]))) /*This function registed our two device{zurico_dev[2] using the register_netdev}*/
							printk("snull: error %i registering device \"%s\"\n",result, snull_devs[i]->name);
						else
							ret = 0;
                                    return 0 ;
				}

	
		              The networking subsystem provides a number of helper functions wrapped around alloc_netdev for various types of interfaces. The most common is alloc_etherdev,which is defined in <linux/etherdevice.h>.
			      			
			      					struct net_device *alloc_etherdev(int sizeof_priv);
			     
				This function allocates a network device using eth%d for the name argument. It It provides its own initialization function (ether_setup) that sets several net_device fields with appropriate values
			       	for Ethernet devices.It provides its own initialization function (ether_setup) that sets several net_device fields with appropriate values for Ethernet devices.

				Thus, there is no driver-supplied initialization function for alloc_etherdev .The driver should simply do its required initialization directly after a successful allocation.
				Writers of drivers for other types of devices may want to take advantage of one of the other helper functions such as :

					1->alloc_fcdev (defined in <linux/fcdevice.h>)
       					2->fiber-channel devices, alloc_fddidev (<linux/fddidevice.h>) for FDDI devices
				        3->alloc_trdev (<linux/trdevice.h>) for token ring devices.



Initializing Each Device:
========================
		
			We have looked at the allocation and registration of net_device structures, but we passed over the intermediate step of completely initializing that structure.
			Note that struct net_device is always put together at runtime; it cannot be set up at compile time in the same manner as a file_operations or block_device_operations structure.
			This initialization must be complete before calling register_netdev.The net_device structure is large and complicatedthe kernel takes care of some Ethernet wide defaults through the ether_setup function
		       	(which is called by alloc_etherdev).

			Since snull uses alloc_netdev, it has a separate initialization function. The snull_init is passed as argument in alloc_netdev The core of this function (snull_init) is as follows:
	     
			Practical example :
			==================


			#include<linux/netdevice.h>

			static const struct net_device_ops snull_netdev_ops = {
			.ndo_open            = snull_open,
			.ndo_stop            = snull_release,
			.ndo_start_xmit      = snull_tx,
			.ndo_do_ioctl        = snull_ioctl,
			.ndo_set_config      = snull_config,
			.ndo_get_stats       = snull_stats,
			.ndo_change_mtu      = snull_change_mtu,
			.ndo_tx_timeout      = snull_tx_timeout
			};

			void device_init(struct net_device *dev) /* The init function (sometimes called probe).*/
			{
				struct zurico_priv *priv;  /*Private data structure for each device..and setup Ethernet network device*/
				ether_setup(dev); /* Using this function kernel take care of net_device structure */
				dev->watchdog_timeo = timeout;
				dev->netdev_ops = &snull_netdev_ops;
				dev->flags           |= IFF_NOARP;
				dev->features        |= NETIF_F_HW_CSUM;

				priv = netdev_priv(dev);
				if (use_napi) {
					netif_napi_add(dev, &priv->napi, snull_poll,2);
				memset(priv, 0, sizeof(struct snull_priv)); /*Here we setting the private memory and initalized with 0*/
				snull_rx_ints(dev, 1);  /* enable receive interrupts */
				snull_setup_pool(dev);  /*Set up a device's packet pool.*/

				}
			}
	
                        struct net_device *zurico_devs[2];    /*Here zurico_devs is the structure arrya point that store this address*/

                                static int __init zurico_init_module(void)
                                {
                                    struct zurico_priv;  /*This structure is private to each device. It is used to pass packets in and out, so there is place for a packet*/
                                    int result, i, ret = -ENOMEM;   /*Here ENOMEM is called out of memory this is one value that we initalized at starting */
                                    zurico_devs[0]=alloc_netdev(sizeof(struct zurico_priv), "sn%d",device_init); /*This function allocated the net_device structure dynamically for one device on success it return addess*/

				return 0 ;
				}

			The above code is a fairly routine initialization of the net_device structure; it is mostly a matter of storing pointers to our various driver functions.The single unusual feature of the code is setting IFF_NOARP 
			in the flags. This specifies that the interface cannot use the Address Resolution Protocol (ARP). ARP is a low-level Ethernet protocol; its job is to turn IP addresses into Ethernet medium access control (MAC) 
			addresses.
	
			The assignment to hard_header_cache is there for a similar reason: it disables the caching of the (nonexistent) ARP replies on this interface.The assignment to hard_header_cache is there for a similar reason: 
			it disables the caching of the (nonexistent) ARP replies on this interface.One more struct net_device field, priv .Its role is similar to that of the private_data pointer that we used for char drivers.

			Unlike fops->private_data ,this priv pointer is allocated along with the net_device structure. Direct access to the priv field is also discouraged, for performance and flexibility reasons. When a driver needs to
		       	get access to the private data pointer, it should use the netdev_priv function.Thus, the snull driver is full of declarations such as:


								struct snull_priv *priv = netdev_priv(dev);   /*This will store private data of snull_priv structure */

			The snull module declares a snull_priv data structure to be used for priv :


				struct snull_priv {
						struct net_device_stats stats;
						int status;
						struct snull_packet *ppool;
						struct snull_packet *rx_queue;
						int rx_int_enabled;                            /* List of incoming packets*/
						int tx_packetlen;
						u8 *tx_packetdata;
						struct sk_buff *skb;
						spinlock_t lock;
						};
			The structure includes, among other things, an instance of struct net_device_stats ,which is the standard place to hold interface statistics.
			The following lines in snull_init allocate and initialize dev->priv :
		
			void device_init(struct net_device *dev) /* The init function (sometimes called probe).*/
                        {
				priv = netdev_priv(dev);
				memset(priv, 0, sizeof(struct snull_priv));
				spin_lock_init(&priv->lock);
				snull_rx_ints(dev, 1);    /* enable receive interrupts */

			}
Module Unloading
================
		Nothing special happens when the module is unloaded. The module cleanup function simply unregisters the interfaces, performs whatever internal cleanup is required, and releases the net_device structure 
		back to the system:

			void snull_cleanup(void)
			{		
			int i;
				for (i = 0; i < 2; i++) {
					if (snull_devs[i]) {
					unregister_netdev(snull_devs[i]);   /*removes the interface from the system;*/
					snull_teardown_pool(snull_devs[i]);
					free_netdev(snull_devs[i]);  /*returns the net_device structure to the kernel*/
					}
				}
					return;
			}
	


















			









