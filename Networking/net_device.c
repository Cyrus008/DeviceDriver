					/*+++++++++++++++++++++++++++++++++++++*/
						The net_device Structure
					/***************************************/

The net_device Structure in Detail :
====================================

The net_device structure is  core of the network driver layer and deserves a complete description.This list describes all the fields, but more to provide a reference than to be memorized.

	struct net_device  {
			1-> Global Information
			2-> Hardware Information
			3-> Interface Information
			4-> Utility Fields
Global Information:
	The first part of struct net_device is composed of the following fields:
		struct net_device  {
			char name[IFNAMSIZ]; 
				The name of the device. If the name set by the driver contains a %d format string,register_netdev replaces it with a number to make a unique name; assigned numbers start at 0 .
			unsigned long state;
				Device state. The field includes several flags. Drivers do not normally manipulate these flags directly; instead, a set of utility functions has been provided.
			struct net_device *next;
				Pointer to the next device in the global linked list. This field shouldn’t be touched by the driver.
			int (*init)(struct net_device *dev);
				An initialization function. If this pointer is set, the function is called by register_netdev to complete the initialization of the net_device structure.
		
Hardware Information:
			The following fields contain low-level hardware information for relatively simple devices.They are a holdover from the earlier days of Linux networking; most modern drivers do make use of them
			We list them here for completeness.

			unsigned  long   rmem_end ;
			unsigned  long   rmem_start;
			unsigned  long   mem_end
			unsigned  long   mem_start

			Device memory information. These fields hold the beginning and ending addresses of the shared memory used by the device. If the device has different receive and transmit memories, the mem fields are used for
		       	transmit memory and the rmem fields for receive memory.The rmem fields are never referenced outsideof the driver itself. By convention, the end fields are set so that end - start is the amount of available 
			onboard memory.


			unsigned long base_addr;    /*The I/O base address of the network interface.The ifconfig command can be used to display or modify the current value.*/
			unsigned char irq;	   /*The assigned interrupt number. The value of dev->irq is printed by ifconfig when interfaces are listed.*/
			unsigned char if_port;/*The port in use on multiport devices. This field is used, for example, with devices that support both coaxial ( IF_PORT_10BASE2 ) and twisted-pair ( IF_PORT_100BASET )Ethernet connections.*/
			unsigned char dma;   /*The DMA channel allocated by the device. The field makes sense only with some peripheral buses, such as ISA.*/
Interface Information:
			Most of the information about the interface is correctly set up by the ether_setup function . Ethernet cards can rely on this general-purpose function for most of these fields, but the flags and dev_addr fields 
			are device specific and must be explicitly assigned at initialization time.

			Some non-Ethernet interfaces can use helper functions similar to ether_setup. drivers/net/net_init.c exports a number of such functions, including the following:

			void ltalk_setup(struct net_device *dev);  /*Sets up the fields for a LocalTalk device*/
			void fc_setup(struct net_device *dev);   /*Initializes fields for fiber-channel devices*/
			void fddi_setup(struct net_device *dev); /*Configures an interface for a Fiber Distributed Data Interface (FDDI) network*/
			void hippi_setup(struct net_device *dev); /*Prepares fields for a High-Performance Parallel Interface (HIPPI) high-speed interconnect driver*/

			void tr_setup(struct net_device *dev); /*Handles setup for token ring network interfaces*/

			Most devices are covered by one of these classes. If yours is something radically new and different, however, you need to assign the following fields by hand:

Utility Fields          
			The remaining struct net_device data fields are used by the interface to hold useful status information.Some of the fields are used by ifconfig and netstat to provide the user with information about the current 
			configuration. Therefore, an interface should assign values to these fields:

			unsigned long trans_start;
			unsigned long last_rx;
			int watchdog_timeo;
			void *priv;
			struct dev_mc_list *mc_list;
			int mc_count;
			spinlock_t xmit_lock;
			int xmit_lock_owner;

The Device Methods:
==================
		Similar to the char and block drivers,each network device declares the functions that act on it. Operations that can be performed on network interfaces are listed in this section. Some of the operations can be left NULL
	        ,and others are usually untouched because ether_setup assigns suitable methods to them.Device methods for a network interface can be divided into two groups: fundamental and optional.

		Fundamental methods include those that are needed to be able to use the interface;optional methods implement more advanced functionalities that are not strictly required.
	       	The following are the fundamental methods:
		
		int (*open)(struct net_device *dev);  /*Opens the interface. The interface is opened whenever ifconfig activates it.*/
		int (*stop)(struct net_device *dev); /*Stops the interface. The interface is stopped when it is brought down.*/
		int (*hard_start_xmit) (struct sk_buff *skb, struct net_device *dev);/*Method that initiates the transmission of a packet.*/
		int (*hard_header) (struct sk_buff *skb, struct net_device *dev, unsigned short type, void *daddr, void *saddr, unsigned len);/*Function that builds the hardware header from the source and destination hardware addresses*/
		int (*rebuild_header)(struct sk_buff *skb);/*Function used to rebuild the hardware header after ARP resolution completes but before a packet is transmitted.*/
		void (*tx_timeout)(struct net_device *dev);/*Method called by the networking code when a packet transmission fails to complete within a reasonable period,*/
		struct net_device_stats *(*get_stats)(struct net_device *dev);
		int (*set_config)(struct net_device *dev, struct ifmap *map);


		The remaining device operations are optional:

		int (*poll)(struct net_device *dev; int *quota);
		void (*poll_controller)(struct net_device *dev);
		int (*do_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
		void (*set_multicast_list)(struct net_device *dev);
		int (*set_mac_address)(struct net_device *dev, void *addr);
		int (*change_mtu)(struct net_device *dev, int new_mtu);
		int (*header_cache) (struct neighbour *neigh, struct hh_cache *hh);
		int (*header_cache_update) (struct hh_cache *hh, struct net_device *dev,unsigned char *haddr);
		int (*hard_header_parse) (struct sk_buff *skb, unsigned char *haddr);





