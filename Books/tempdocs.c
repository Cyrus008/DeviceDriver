Packet Transmission:
===================
		The most important tasks performed by network interfaces are data transmission and reception.The transmission used for sending a packet over a network layer.Whenever the kernel needs
	       	to transmit a data packet,they used the driver’s "hard_start_transmit" to put the data on an outgoing queue. Each packet handled by the kernel is contained in a socket buffer structure.

		Each network packet belongs to a socket in the higher network layers, and the input/output buffers of any socket are lists of struct sk_buff structures. The same sk_buff structure is used to host network
	       	data throughout all the Linux network subsystems, but a socket buffer is just a packet as far as the interface is concerned.A pointer to sk_buff is usually called skb ,the socket buffer is a complex structure,
	       	and the kernel offers a number of functions to act on it.This structure is define in <linux/skbuff.h>.We need few basic facts about sk_buff to write a working driver.

		Whenever the kernel needs to transmit a data packet,drvier  create the socket buffer container  using the "dev_alloc_skb" to store the data packet using the single linked list manner .
		The socket buffer passed to hard_start_xmit contains the physical packet as it should appear on the media, complete with the transmission-level headers.The interface never  modify the data that being transmitted
		over network layer.The socket buffer structure element skb->data,skb->len points to the packet being transmitted, and  its length in octets.

		The snull packet transmission code given bellow and physical or hardware transmission funtion has been isolated in another function because every interface driver must implement it according to the specific hardware :


		Demo Code of Packet Transmission :
		---------------------------------

		int snull_tx(struct sk_buff *skb, struct net_device *dev)
		{
			int len;
			char *data, shortpkt[ETH_ZLEN];
			struct snull_priv *priv = netdev_priv(dev); /*We are extracting private data of the network interface*/
				data = skb->data;  /*We fetching the socket buffer data in form of data pointer*/
				len = skb->len;    /*We fetching the length of incomming packet*/
					if (len < ETH_ZLEN) {  /*Checking size of incomming packet length with ETH_ZLEN if len is less than ETH_ZLEN then we do following */
				memset(shortpkt, 0, ETH_ZLEN);  /*We setting shortpkt array size with ETH_ZLEN byte and fill it with 0*/
				memcpy(shortpkt, skb->data, skb->len);  /*Coping the data skb->data equal to skb->len to the shortpkt */
				len = ETH_ZLEN;  /*Assigned ETH_ZLEN the len for future used*/
				data = shortpkt; /*This shortpkt containing data will move the data */
				}
			dev->trans_start = jiffies; /* save the timestamp */
			/* Remember the skb, so we can free it at interrupt time */
			priv->skb = skb;  
			/* actual deliver of data is device-specific, and not shown here */
			snull_hw_tx(data, len, dev);  /*We processing this buffer data to data link  layer usng hardware specific driver*/
			return 0; /* Our simple device can not fail */
		}


		The transmission function do somthing: 
		-------------------------------------
		1->Performs some sanity checks on the packet and transmits the data through the hardware-related function.Need care about when the packet to be transmitted is shorter than the minimum length supported by the
	           underlying media ,that we need check in snull_tx funtion.Many Linux network drivers have been found to leak data in such situations. Rather than create that sort of security vulnerability, we copy short packets
		   into a separate array that we can explicitly zero-pad out to the full length required by the media.

hard_start_xmit
===============
		The most important tasks performed by network interfaces are data transmission and reception.The transmission used for sending a packet over a network layer.Whenever the kernel needs
                to transmit a data packet,they used the driver’s "hard_start_transmit" to put the data on an outgoing queue.The return value from hard_start_xmit should be 0 on success and on success
		indicates kernel driver has taken responsibility for the packet, should make its best effort to ensure that transmission succeeds, and must free the skb at the end.

		A nonzero return value indicates that the packet could not be transmitted at this time and the kernel will retry later for transmission.In this situation, your driver should stop the queue until whatever situation
		caused the failure has been resolved.The “hardware-related” transmission function (snull_hw_tx) is very trickery of the snull device,including manipulating the source and destination addresses, and has little of 
		interest to authors of real network drivers.




Controlling Transmission Concurrency :
======================================
	1.The hard_start_xmit function is protected from concurrent calls by a spinlock in the net_device structure and As soon as the function returns, however, it may be called again.
	2.The hard_start_xmit function returns when the software is done instructing the hardware about packet transmission, but hardware transmission will likely not have been completed.
	3.This is not an issue with network driver, All of its work done using the CPU level, So packet transmission is complete before the transmission function returns.
	4.The Real hardware(Nic Card) interfaces transmit the packets asynchronously and have a limited amount of memory available to store outgoing packets. When that memory is exhausted
	  Then 	the driver needs to tell the networking system not to start any more transmissions until the hardware is ready to accept new data.
	5.This notification is accomplished by calling netif_stop_queue, this function introduced earlier to stop the queue. Once your driver has stopped its queue, it must arrange to restart
          the queue at some point in the future, when memory of hardware is again able to accept packets for transmission.The start and stopped transmissin given bellow:
	   
			void netif_stop_queue(struct net_device *dev);	
			void netif_wake_queue(struct net_device *dev);
	 
	This  2'nd function is just like netif_start_queue, except that it also pokes the networking system to make it start transmitting packets again.

	Fact About the Modern hardware :
	--------------------------------
		The  modern network hardware maintains an internal queue for multiple packets transmission .Using this apporoach it get the best performance from the network.
		The Network drivers for these hardware devices must support the multiple transmisions outstanding at any given time, but device memory can fill up either the hardware 
		supports multiple outstanding transmissions or not.Whenever device memory fills and there is no room for the largest possible packet, then the driver should stop the queue
		until space becomes available again .If you must disable packet transmission from anywhere other than your hard_start_xmit function the function you want to use is:

			void netif_tx_disable(struct net_device *dev);

		This function behaves much like netif_stop_queue, but it also ensures that, when it returns,then it means your hard_start_xmit method is not running on another CPU. The queue can be restarted
	       	with netif_wake_queue, as usual.


Transmission Timeouts :
======================
	The kernel drivers that deal with real hardware have to be prepared if hardware failed to respond occasionally.The interface has not any clue about hardware failoure,or the system
	can lose an interrupt and how driver handle this situation .This sort of problem is common with some devices designed to run on personal computers.
	
	This bugy problome can be short out using driver by setting timers,but again new problom can raised if the operation has not completed  due to timer expires, something is wrong or any terriable bugs.
	In this situation network system not able trace or assembly  that complicated "state machines" controlled by a mass of timers .

	The network drivers need not worry about detecting such problems themselves. So its need to set set a timeout period via network driver.which goes in the watchdog_timeo field of the net_device structure.
	This period, which is in jiffies, should be long enough to account for normal transmission delays .If the current system time exceeds the device’s trans_start time by at least the timeout period, the networking layer
       	eventually calls the driver’s tx_timeout method.

	That tx_timeout’s job is to do whatever is needed to clear up the problem and to ensure the proper completion of any other transmissions that were already in progress. It is important, in particular, that the driver not lose 
	track of any socket buffers that have been entrusted to it by the networking code.The snull has the ability to simulate transmitter lockups, which is controlled by two loadtime parameters:

	The snull has the ability to simulate transmitter lockups, which is controlled by two load-time parameters:'


				static int lockup = 0;
				module_param(lockup, int, 0);

				static int timeout = SNULL_TIMEOUT;
				module_param(timeout, int, 0);

	If the driver is loaded with the parameter lockup=n , a lockup is simulated once every n packets transmitted, and the watchdog_timeo field is set to the given timeout value.
	When simulating lockups, snull called netif_stop_queue to prevent other transmission attempts during the timeout value and if some thing is going terrible then the tx_timeout dropped the current serving transmission packet.
	The snull transmission timeout handler looks like this:
			void snull_tx_timeout (struct net_device *dev)
			  {
			    struct snull_priv *priv = netdev_priv(dev);
			    PDEBUG("Transmit timeout at %ld, latency %ld\n", jiffies,jiffies - dev->trans_start);
			    priv->status = SNULL_TX_INTR;
			    snull_interrupt(0, dev, NULL);
			    priv->stats.tx_errors++;
			    netif_wake_queue(dev);
			    return;
			}

	When a transmission timeout happens, the driver must mark the error in the interface statistics and arrange for the device to be reset to a sane state so that new packets can be transmitted.
	When a timeout happens in snull, the driver calls snull_interrupt to fill in the “missing” interrupt and restarts the transmit queue with netif_wake_queue.


Packet Reception:
=================
	The receiving data from the network is trickier than transmitting it, because an sk_buff must be allocated and handed off to the upper layers{Top Half} from within an atomic context.
	There are two modes of packet reception that may be implemented by network drivers: interrupt driven and polled.Most drivers implement the interrupt-driven technique,and some driver used polled technique
	for high-bandwidth adapters .

	The snull_rx function separates the “hardware” details from the device independent housekeeping.Therefore, the function snull_rx is called from the snull “interrupt” handler after the hardware has received the packet,
	and it is already in the computer’s memory. The snull_rx receives a pointer to the data and the length of the packet; its sole responsibility is to send the packet and some additional information to the upper layers of
       	networking code.This sample code is independent of the way the data pointer and length are obtained its just a funtion that is called from the interrupt handler


		void snull_rx(struct net_device *dev, struct snull_packet *pkt)
			{
				struct sk_buff *skb;
				struct snull_priv *priv = netdev_priv(dev); /*We are extracting private data of the network interface*/
			/*
			* The packet has been retrieved from the transmission * medium. Build an skb around it, so upper layers can handle it */
				skb = dev_alloc_skb(pkt->datalen + 2); /*Using the data length we allocate kernel space for the buffer. The dev_alloc_skb calls kmalloc with atomic priority, so it can be used safely at interrupt time.*/
					if (!skb) {     /*Return value from dev_alloc_skb must be checked*/
						if (printk_ratelimit( )) /*Generating hundreds or thousands of console messages per second is a good way to bog down the system entirely and hide the real source of problems
									  printk_ratelimit helps prevent that problem by returning 0 when too much output has gone to the console, and things need to be slowed down a bit.*/
						 printk(KERN_NOTICE "snull rx: low on mem - packet dropped\n"); 
						 priv->stats.rx_dropped++; /*We calculating the statics for the dropped packet and goto out*/
						 goto out;
						}
					memcpy(skb_put(skb, pkt->datalen), pkt->data, pkt->datalen); /*The packet data is copied into the buffer by calling memcpy; the skb_put function updates the end-of-data pointer in the buffer and
													returns a pointer to the newly created space.*/
					/* Write metadata, and then pass to the receive level */
					skb->dev = dev;
					skb->protocol = eth_type_trans(skb, dev);
					skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */
					priv->stats.rx_packets++; /*We calculating the statics for the dropped packet and goto out*/
					priv->stats.rx_bytes += pkt->datalen;
					netif_rx(skb);
			out:
				return;
			}
	
	Optimization of packet Rx:
	-------------------------
	For the high-performance driver for an interface that full-fill the bus-mastering I/O , need a possible optimization that we will consider here.So there is two possible way to do this :
	1-> Some drivers allocate socket buffers for incoming packets prior to their reception then instruct the interface to place the packet data directly into the socket buffer’s space.
	2-> The networking layer cooperates with this strategy by allocating all socket buffers in DMA capable space. Doing things this way we avoids the need for a separate copy operation to fill the
	    socket buffer, but requires being careful with buffer sizes because you won’t know in advance how big the incoming packet is.The implementation of a change_mtu method is also important in this 
	    situation, since it allows the driver to respond to a change in the maximum packet size.
	3-> The network layer needs spelled out some information  before  sensing of the packet.To this end, the dev and protocol fields must be assigned before the buffer is passed upstairs.
	4-> The Ethernet support  a helper function (eth_type_trans), which finds an appropriate value to put into protocol . Then we need to specify how checksumming is to be performed or has been performed on the
	    packet .The possible policies for skb->ip_summed are:
	    	
	    	4.1-> CHECKSUM_HW
				The device has already performed checksums in hardware. An example of a hardware checksum is the SPARC HME interface.
		4.2-> CHECKSUM_NONE
				Checksums have not yet been verified, and the task must be accomplished by system software. This is the default in newly allocated buffers.
		4.3-> CHECKSUM_UNNECESSARY
				Don’t do any checksums. This is the policy in snull and in the loopback interface.

	5-> Finally, the driver updates its statistics counter to record that a packet has been received. The statistics structure is made up of several fields; the most important are rx_packets , rx_bytes , tx_packets , 
	    and tx_bytes , which contain the number of packets received and transmitted and the total number of octets transferred.
	6-> The last step in packet reception is performed by netif_rx, which hands off the socket buffer to the upper layers. netif_rx actually returns an integer value; NET_RX_SUCCESS (0) means that the packet was successfully received;
            any other value indicates trouble.There are three return values ( NET_RX_CN_LOW , NET_RX_CN_MOD , and NET_RX_CN_HIGH ) that indicate increasing levels of congestion in the networking subsystem; NET_RX_DROP means the packet was
	    dropped.


The Interrupt Handler
======================
	The most hardware interfaces are controlled by means of an interrupt handler.The hardware interrupts the processor to signal one of two possible events:
		1->A new packet has arrived
	        2->The transmission of an outgoing packet is complete.
	Network interfaces can also generate interrupts to signal errors, link status changes, and so on.

	The usually interrupt service  routine can tell the difference between a new-packet-arrived interrupt and transmition done notification by checking a status register found on the physical device.The status register is
	hardware dependent but its can be implemented via software dev->priv also using the snull interface. The interrupt handler for a network interface looks like this:


		static void snull_regular_interrupt(int irq, void *dev_id, struct pt_regs *regs)
			{
				int statusword;
				struct snull_priv *priv;
				struct snull_packet *pkt = NULL;
/*
* As usual, check the "device" pointer to be sure it is really interrupting. Then assign "struct device *dev"
*/
				struct net_device *dev = (struct net_device *)dev_id;
/* ... and check with hw if it's really ours */
/* paranoid */
				if (!dev)
					return;
						/* Lock the device */
				priv = netdev_priv(dev);
				spin_lock(&priv->lock); /*Locking the critical Resources */
				/* retrieve statusword: real netdevices use I/O instructions */
				statusword = priv->status;
				priv->status = 0;
				if (statusword & SNULL_RX_INTR)   /*Transmission updating via regular interrupt handler using statusword & SNULL_RX_INTR*/
			       	{
				/* send it to snull_rx for handling */
				pkt = priv->rx_queue;
				if (pkt) {
					priv->rx_queue = pkt->next;
					snull_rx(dev, pkt);
					}
				}
				
				if (statusword & SNULL_TX_INTR) {     /*Transmission updating via regular interrupt handler  using statusword & SNULL_TX_INTR*/
					/* a transmission is over: free the skb */
					priv->stats.tx_packets++;  /*statistics are updated for transmission of packets*/
					priv->stats.tx_bytes += priv->tx_packetlen;    /*statistics are updated for transmission of bytes*/
					dev_kfree_skb(priv->skb); /*Releasig the socket buffer when  all transmission and receving is done*/
					}
					spin_unlock(&priv->lock); /*Unlocking the critical resources*/
					if (pkt) snull_release_buffer(pkt); /* Do this outside the lock! */
					return;
		}

 	The handler’s first task is to retrieve a pointer to the correct struct net_device . This pointer usually comes from the dev_id pointer received as an argument.
	The interesting part of this handler deals with the “transmission done” situation.The dev_kfree_skb is called to when we need free the critical section .There are, actually, three variants of this
	function that may be called:

			1->dev_kfree_skb(struct sk_buff *skb);
						This version should be called when you know that your code will not be running in interrupt context.
			2->dev_kfree_skb_irq(struct sk_buff *skb);
						If you know that you will be freeing the buffer in an interrupt handler, use this version, which is optimized for that case.
			3->dev_kfree_skb_any(struct sk_buff *skb);
						This is the version to use if the relevant code could be running in either interrupt or noninterrupt context.

	Finally, if your driver has temporarily stopped the transmission queue, this is usually the place to restart it with netif_wake_queue.


Receive Interrupt Mitigation
============================
	In a network driver the processor is interrupted for every packet received and transmitted by your interface.For normal mode of the operation their is not a problem but for high-bandwidth interfaces,the 
	processor received thousands of packets per second.The overall performance of CPU degrade  due to handling a floud of interrupt per second.

	For improving the performance of Linux on high-end systems, the kernel developers  created an alternative interface (called NAPI) based on polling.“Polling” can be a dirty word among driver developers, who often see
	polling techniques as inelegant and inefficient. When the system has a high-speed interface handling heavy traffic, there is always more packets to process. There is no need to interrupt the processor in such situations
	it is enough that the new packets be collected from the interface and stored in small size of buffer when buffer is fulll then we interrupt the processor for packet receiving and transmitting.

	Stopping receive interrupts can reduced the huge amount of load from the processor. NAPI-compliant drivers also instructed don't feed packets into the kernel if those packets are just dropped in the networking code
       	due to congestion.For various reasons, NAPI drivers are also less likely to reorder packets.

	Not all devices can operate in the NAPI mode A NAPI-capable interface must be able to store several packets (either on the card itself, or in an in-memory DMA ring). The interface should be capable of disabling interrupts for
       	received packets, while continuing to interrupt for successful transmissions and other events.

	Relatively few drivers implement the NAPI interface. If you are writing a driver for an interface that may generate a huge number of interrupts, however, taking the time to implement NAPI may well prove worthwhile.
	The snull driver, when loaded with the use_napi parameter set to a nonzero value, operates in the NAPI mode. At initialization time, we have to set up a couple of extra struct net_device fields:
	
			if (use_napi) {
					dev->poll=snull_poll;
					dev->weight=2;
					}
	Parameter :
		1->poll :
			The poll field must be set for your driver that using polling function.
		2->weight:
			The weight field describes how much traffic should be accepted from the interface when resources are tight.There are no strict rules for how the weight parameter should be set .
			by convention, 10 MBps Ethernet interfaces set weight to 16 , while faster interfaces use 64 .You should not set weight to a value greater than the number of packets your interface can store.

	The next step in the creation of a NAPI-compliant driver is to change the interrupt handler. When your interface (which should start with receive interrupts enabled) signals that a packet has arrived, the interrupt handler should
       	not process that packet. Instead, it should disable further receive interrupts and tell the kernel that it is time to start polling the interface. In the snull “interrupt” handler, the code that responds to packet reception 
	interrupts has been changed to the following:

			if (statusword & SNULL_RX_INTR) {
					snull_rx_ints(dev, 0); /* Disable further interrupts */
					netif_rx_schedule(dev);
					}

	When the interface tells us that a packet is available, the interrupt handler leaves it in the interface; all that needs to happen at this point is a call to netif_rx_schedule,which causes our poll method to be called at some
       	future point.The poll method has this prototype:
				
					int (*poll)(struct net_device *dev, int *budget);
	
	The snull implementation of the poll method looks like this:


		static int snull_poll(struct net_device *dev, int *budget)
			{
				int npackets = 0, quota = min(dev->quota, *budget);
				struct sk_buff *skb;
				struct snull_priv *priv = netdev_priv(dev);
				struct snull_packet *pkt;
					while (npackets < quota && priv->rx_queue) 
					{
						pkt = snull_dequeue_buf(dev);
						skb = dev_alloc_skb(pkt->datalen + 2);
						if (! skb) {
							if (printk_ratelimit( ))
							printk(KERN_NOTICE "snull: packet dropped\n");
							priv->stats.rx_dropped++;
							snull_release_buffer(pkt);
							continue;
							}
						memcpy(skb_put(skb, pkt->datalen), pkt->data, pkt->datalen);
						skb->dev = dev;
						skb->protocol = eth_type_trans(skb, dev);
						skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */
						netif_receive_skb(skb);
						/* Maintain stats */
						npackets++;
						priv->stats.rx_packets++;
						priv->stats.rx_bytes += pkt->datalen;
						snull_release_buffer(pkt);
					}
/* If we processed all packets, we're done; tell the kernel and reenable ints */
				*budget -= npackets;
				dev->quota -= npackets;
				if (! priv->rx_queue)
			       		{
					netif_rx_complete(dev);
					snull_rx_ints(dev, 1);
					return 0;
					}
/* We couldn't process everything. */
				return 1;
				}

	The central part of the function is concerned with the creation of an skb holding the packet; this code is the same as what we saw in snull_rx before.We need to consider basic field of this function

	1->budght :
		  The maximum number of packets that we are allowed to pass into the kernel.The budget value is a maximum number of packets that the current CPU can receive from all interfaces.
	2->Quota  :
		  The quota is a per-interface value that usually starts out as the weight assigned to the interface at initialization time.
	3->Packet :
		  The packet  stored by sk buffer is fed to the kernel using netif_receive_skb, rather than netif_rx.
	4->       :
		  If the poll method is able to process all of the available packets within the limits given to it, it should re-enable receive interrupts, call netif_rx_complete to turn off polling, and return 0 .
		  A return value of 1 indicates that there are packets remaining to be processed.


The Socket Buffers{strcut sk_buff *skb}
======================================

		The socket buffer is described by the sk_buff structure that defined in <linux/skbuff.h>,The structure is at the core of the network subsystem of the Linux kernel, and we now introduce both the main fields of the
	       	structure and the functions used to act on it.The internals of sk_buff contents can be helpful when you are tracking down problems and when you are trying to optimize your code.The usual warning applies here: 
		
			if you write code that takes advantage of knowledge of the sk_buff structure, you should be prepared to see it break with future kernel releases. Still, sometimes the performance advantages justify the additional 
			maintenance cost.


		The Important Fields
		====================
		01_Field :
	       		  This is structure of network interfaced that can be accessed via socket buffer :		
		  		struct net_device *dev;
		 02_Field :
  	                  This structure is doubly linked list that can move forward and backward using the following element :
				struct sk_buff	*next;
				struct sk_buff	*prev;			  
	        03_Field :
			  It used the struct sock that used for  network layer representation of sockets .
		04_Field :
			 This structure contain  inner_transport_header,inner_network_header, inner_mac_header .They are represented mainly transport,network,link layer.
			
	       		 	__u16			inner_transport_header;
				__u16			inner_network_header;
				__u16			inner_mac_header;	

		05_Field :
			  These elements must be at the end, see alloc_skb() for details. 

			  		sk_buff_data_t		tail; /*tail is the end of the valid octets*/
					sk_buff_data_t		end; /*The end points to the maximum address tail can reach.	*/
					unsigned char		*head,*data; /*The head points to the beginning of the allocated space and data is the beginning of the valid octets*/
					unsigned int		truesize;
					refcount_t		users;
			Another way to look at it is that the available buffer space is skb->end - skb->head , and the currently used data space is skb->tail - skb->data .

									

Functions Acting on Socket Buffers:
==================================
	Network devices that use an sk_buff structure act on it by means of the official interface functions. Many functions operate on socket buffers; here are the most interesting ones:
	
			struct sk_buff *alloc_skb(unsigned int len, int priority);
			struct sk_buff *dev_alloc_skb(unsigned int len);
			void   kfree_skb(struct sk_buff *skb);
			void   dev_kfree_skb(struct sk_buff *skb);
			void   dev_kfree_skb_irq(struct sk_buff *skb);
			void   dev_kfree_skb_any(struct sk_buff *skb);
			unsigned char *skb_put(struct sk_buff *skb, int len);
			unsigned char *__skb_put(struct sk_buff *skb, int len);

			unsigned char *skb_push(struct sk_buff *skb, int len);
			unsigned char *__skb_push(struct sk_buff *skb, int len);

			int skb_tailroom(struct sk_buff *skb);
			int skb_headroom(struct sk_buff *skb);
			void skb_reserve(struct sk_buff *skb, int len);
			unsigned char *skb_pull(struct sk_buff *skb, int len);
			int skb_is_nonlinear(struct sk_buff *skb);
			int skb_headlen(struct sk_buff *skb);

			void *kmap_skb_frag(skb_frag_t *frag);
			void kunmap_skb_frag(void *vaddr);
			

		  				%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  THE END  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
			



			
