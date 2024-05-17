##################################################################################
#                                                                                #
#                              example_bcast                                     #
#                                                                                #
##################################################################################

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Parameter Setting and Configuration
 - Result description
 - Supported List


Description
~~~~~~~~~~~
        This example will listen broadcast message on port 49152.
        Send packet with the content of received packet to broadcast address.

        Note:
        If you encounter some message like:
            ERROR: sendto broadcast
            [Driver]: skb_unavailable=1 in last 2 seconds
        It means that the skb buffer is not enough for the massive UDP packets to be sent.
        If you want to prevent the error you can add some delay time between sending packets or enlarge the skb buffer configuration.


Setup Guide
~~~~~~~~~~~
		1. Add broadcast example to SDK
        
		/component/example/network_protocol/bcast
		.
		|-- example_bcast.c
		|-- example_bcast.h
		`-- readme.txt
            
		2. Set UDP packages in [lwipopts.h]
			#define LWIP_UDP  1
  
		3. Add broadcast example source files to project
		(a) For IAR project, add bcast example to group <example>
			$PROJ_DIR$\..\..\..\component\example\network_protocol\bcast\example_bcast.c

		4. GCC: use CMD "make all EXAMPLE=bcast" to compile bcast example.

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Can make automatical Wi-Fi connection when booting by using wlan fast connect example.


Result description
~~~~~~~~~~~~~~~~~~
        A broadcast example thread will be started automatically when booting.

        To test:
        1. Prepare a NB and connect to the same AP Ameba connected.
        2. NB: iperf -c 192.168.1.255 -t 60 -i 1 -p 49152 -u
        3. The recv/send messages should be printed out on Ameba console.
        4. Use sniffer to make sure the packets send from Ameba are broadcast messages.


Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported IC :
                 RTL8730A, RTL872XE