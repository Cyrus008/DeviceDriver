										/*************************/
										     Opening and Closing
										/************************/

SYNOMS
=======		
		SIOCSIFADDR              Socket I/O Control Set Interface Address
		SIOCSIFFLAGS             Socket I/O Control Set Interface Flags	

Opening and Closing :
		     The driver do probing only for the interface at "kernel booting  time or module load time" . Before the interface can carry packets, the kernel must open it and assign an address to it.
		     The kernel opens or closes an interface in response to the ifconfig command.When ifconfig is used to assign an address to the interface, it performs two tasks.
		     1->It assigns the address by means of ioctl(SIOCSIFADDR) .
		     2->Then it sets the IFF_UP bit in dev->flag by means of ioctl(SIOCSIFFLAGS) 

		     As far as the device is concerned, ioctl(SIOCSIFADDR) does nothing. No driver function is invoked—the task is device independent, and the kernel performs it.The latter command (ioctl(SIOCSIFFLAGS)), however, 
		     calls the open method for the device.


		     Similarly, when the interface is shut down, ifconfig uses ioctl(SIOCSIFFLAGS) to clear IFF_UP , and the stop method is called.Both device methods return 0 in case of success and the usual negative value in case
		     of error.The driver has to perform many of the same tasks as the char and block drivers do.

		     the open() requests any system resources thats its needs and tells the interface to triggerd up. The stop shuts down the interface and releases system resources.Network drivers must perform some additional steps
		     at open time.At the open time the hardware (MAC) address needs to be copied from the hardware device to dev->dev_addr before the interface can communicate with the outside world.

		     The snull software interface assigns it from within open call .The MAC is just a fakes  hardware number of length ETH_ALEN(Length of Ethernet hardware addresses) .The open method start the interface’s transmit queue
		     once it is ready to start sending data.The kernel provides a function to start the queue:

		     		void netif_start_queue(struct net_device *dev);  /*This is interface's transmit queue for ready to start sending data*/

		     Practical Example of Open method :
		    	 	  	
			int snull_open(struct net_device *dev)
			  {
		            memcpy(dev->dev_addr, "\0SNUL0", ETH_ALEN);    /*Assign the hardware address of the board to driver module (dev->dev_addr)*/
			    if (dev = = snull_devs[1])    /*Here we are checking which device is using on open method*/
				    dev->dev_addr[ETH_ALEN-1]++ ; /*We are accessing ETH_ALEN one by one so multicast address we can map*/
			            netif_start_queue(dev); /* Transmit queue for ready to start sending data*/
		       return 0;
			  }
		
         	    in the absence of real hardware, there is little to do in the open method. The same is true of the stop method; it just reverses the operations of open.the function implementing stop is often called close or release.

		    int snull_release(struct net_device *dev)
			{
				/* release ports, irq and such -- like fops->close */
			netif_stop_queue(dev); /* can't transmit any more */
			return 0;
			}

		  The function void netif_stop_queue(struct net_device *dev);is the opposite of netif_start_queue;it marks the device as being unable to transmit any more packets. The function must be called when the interface 
		  is closed.but can also be used to temporarily stop transmission.
		
		  				
		  							/*Packet Transmission*/
Packet Reception :
==================
	     The most important tasks performed by network interfaces are data transmission and reception.Transmission refers to the act of sending a packet over a network link.Whenever the kernel needs to transmit a data packet, 
	     it calls the driver’s hard_start_transmit method to put the data on an outgoing queue. Each packet handled by the kernel is contained in a socket buffer structure(struct sk_buff) defind in (struct sk_buff) .
	     The sk_buff get its name "socket" form the Unix abstraction and represent a network connection .

	     if the interface has nothing to do with sockets, each network packet belongs to a socket in the higher network layers, and the input/output buffers of any socket are lists of struct sk_buff structures.
	     The same sk_buff structure is used to host network data throughout all the Linux network subsystems, but a socket buffer is just a packet as far as the interface is concerned.The socket buffer is a complex structure,
	     and the kernel offers a number of functions to act on it.

	     The socket buffer passed to hard_start_xmit contains the physical packet as it should appear on the media, complete with the transmission-level headers.The interface doesn’t need to modify the data being transmitted. 
	     skb->data points to the packet being transmitted, and skb->len is its length in octets.The snull packet transmission code follows;
		
		     	int snull_tx(struct sk_buff *skb, struct net_device *dev)
			{
				int len;
				char *data, shortpkt[ETH_ZLEN];
				struct snull_priv *priv = netdev_priv(dev); /*Device specific private data for device */
				data = skb->data;  /*Accessing the data from socket buffer*/
				len  = skb_>len ;  /*Storing the length of data packet that comming from sk_buff*/

				if (len < ETH_ZLEN) {   /*Comparing the len of data with ETH_ZLEN  if it len then we assigned ETH_ZLEN byte memory to an array and initalized it with 0*/
					memset(shortpkt, 0, ETH_ZLEN); 
					memcpy(shortpkt, skb->data, skb->len); /*We are pushing the data to destination that commming from the sk_buf */
					len = ETH_ZLEN;  /*Now updating the len with ETH_ZLEN */
					data = shortpkt; /*Now again copy the content of shortpkt to skb->data*/
				}

				dev->trans_start = jiffies; /* save the timestamp */
				priv->skb = skb;   /* Remember the skb, so we can free it at interrupt time so we storing it in private part of net_device*/
				snull_hw_tx(data, len, dev);  /*actual deliver of data on  device-specific*/
				return 0; /* Our simple device can not fail */

		   }

	The transmission function  performs some sanity checks on the packet and transmits the data through the hardware-related function.The care that is taken when the packet(len) to be transmitted is shorter than the minimum
	length(ETH_ZLEN) supported by the underlying media.Many Linux network drivers have been found to leak data in such situations.

	Rather than create that sort of security vulnerability, we copy short packets into a separate array that we can explicitly zero-pad out to the full length required by the media.
	Code is given bellow :

			if (len < ETH_ZLEN) {   /*Comparing the len of data with ETH_ZLEN  if it len then we assigned ETH_ZLEN byte memory to an array and initalized it with 0 */
                                        memset(shortpkt, 0, ETH_ZLEN); 
                                        memcpy(shortpkt, skb->data, skb->len); /*We are pushing the data to destination that commming from the sk_buf */
                                        len = ETH_ZLEN;  /*Now updating the len with ETH_ZLEN */
                                        data = shortpkt; /*Now again copy the content of shortpkt to skb->data*/
                                }



	The return value from hard_start_xmit should be 0 on success; at that point, your driver has taken responsibility for the packet, should make its best effort to ensure
	that transmission succeeds, and must free the skb at the end.



Packet Reception :
==================
	Receiving data from the network is trickier than transmitting it, because an sk_buff must be allocated and handed off to the upper layers from within an atomic context.
	There are two modes of packet reception that may be implemented by network drivers:interrupt driven and polled. Most drivers implement the interrupt-driven technique and Some drivers for high-bandwidth adapters may 
	also implement the polled technique;

	The implementation of snull separates the “hardware” details from the device-independent housekeeping.Therefore, the function snull_rx is called from the snull “interrupt” handler after the hardware has received the packet, 
	and it is already in the computer’s memory.The snull_rx receives a pointer to the data and the length of the packet; its sole responsibility is to send the packet and some additional information to the upper layers of networking
       	code.This code is independent of the way the data pointer and length are obtained.



	void snull_rx(struct net_device *dev, struct snull_packet *pkt)
		{
		 	struct sk_buff *skb;   /*Socket buffer structure*/
			struct snull_priv *priv = netdev_priv(dev);  /*Accessing private data of the device*/
			/*The packet has been retrieved from the transmission medium. Build an skb around it, so upper layers can handle it */
			skb = dev_alloc_skb(pkt->datalen + 2);     /*Building the skb */
			if (!skb) {			/*Checking the status of socket buffer and printk_ratelimit*/
				if (printk_ratelimit( ))
					printk(KERN_NOTICE "snull rx: low on mem - packet dropped\n");   
				priv->stats.rx_dropped++;  /*if both condition is failed then packet is dropped*/
				goto out;
			      }
		     memcpy(skb_put(skb, pkt->datalen), pkt->data, pkt->datalen); /*if both condition is true then we copy a fixed length of data from pkt to skb_put destination*/
		     skb->dev = dev;   /*Write metadata, and then pass to the receive level */
		     skb->protocol = eth_type_trans(skb, dev);  /*Find appropriate value to put into protocol*/
		     skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */
		     priv->stats.rx_packets++;
		     priv->stats.rx_bytes += pkt->datalen;
		    netif_rx(skb);
		out:
		return;
			}
	The function is sufficiently act as a template for any network driver but some explanation is necessary before you can reuse this code fragment with confidence.
	code Explanation :
	==================
	1->The first step is to allocate a buffer to hold the packet. Note that the buffer allocation function (dev_alloc_skb) needs to know the data length.The information is used by the function to allocate space for the buffer.
           dev_alloc_skb calls kmalloc with atomic priority, so it can be used safely at interrupt time.The kernel offers other interfaces to socket-buffer allocation
	2->We call printk_ratelimit before complaining about failures, however.
	3->Once there is a valid skb pointer, the packet data is copied into the buffer by calling memcpy; the skb_put function updates the end-of-data pointer in the buffer and returns a pointer to the newly created space.
	4->The Ethernet support code exports a helper function (eth_type_trans), which finds an appropriate value to put into protocol . Then we need to specify how checksumming is to be performed or has been performed on the packet.



The Interrupt Handler:
=====================
	Most hardware interfaces are controlled by means of an interrupt handler.The hardware interrupts the processor using the signal one of two possible events:
	1->A new packet has arrived
	2->The transmission of an outgoing packet is complete.
	3->The signal errors, link status changes.

	The usual interrupt routine tell the difference between a "new-packet-arrived" interrupt and a "done-transmitting" notification by checking a status register found on the physical device.
	The snull interface works similarly, but its status word is implemented in software and lives in dev->priv . The interrupt handler for a network interface looks like this:


	static void snull_regular_interrupt(int irq, void *dev_id, struct pt_regs *regs)
	{
		int statusword;
		struct snull_priv *priv;
		struct snull_packet *pkt = NULL;
		struct net_device *dev = (struct net_device *)dev_id; /** As usual, check the "device" pointer to be sure it is really interrupting.Then assign "struct device *dev"*/
		/* ... and check with hw if it's really ours */
		/* paranoid */
		if (!dev)
			return;

		priv = netdev_priv(dev);  /* Lock the device */
		spin_lock(&priv->lock);
		statusword = priv->status;
		priv->status = 0;
			if (statusword & SNULL_RX_INTR) {/* send it to snull_rx for handling */
			pkt = priv->rx_queue;
				if (pkt) {
					priv->rx_queue = pkt->next;
					snull_rx(dev, pkt);
					}
				}
					if (statusword & SNULL_TX_INTR) {
					/* a transmission is over: free the skb */
						priv->stats.tx_packets++;
						priv->stats.tx_bytes += priv->tx_packetlen;
						dev_kfree_skb(priv->skb);
				}
					/* Unlock the device and we are done */
						spin_unlock(&priv->lock);
						if (pkt) 
							snull_release_buffer(pkt); /* Do this outside the lock! */
				return;
		}


































