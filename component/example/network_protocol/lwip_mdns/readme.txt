##################################################################################
#                                                                                #
#                      example_lwip_mdns                          #
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
        An example to use the mdns APIs in example_lwip_mdns.c.
	
Setup Guide
~~~~~~~~~~~
		GCC:  use CMD "make all EXAMPLE=lwip_mdns" to compile mdns example

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		1. In example, operations on mDNS responder and service are presented.
		2. If device Wi-Fi connection is setup by fast reconnect (Please refer to readme of fast reconnect example),
		the mDNS responder could be started after IP address is available.
		3. An example service is registered, updated. Finally, the mDNS responder is stopped.
		
Result description
~~~~~~~~~~~~~~~~~~
		1. To discover service in network, the "dns-sd" command provided in Bonjour SDK can be used.
		2. Bonjour SDK is available on Apple developer support site, and can be downloaded by Apple developer.
		3. In the following, "dns-sd -B _service1" command is used to browse for service instance of type "_service1", and service of "ameba._service1._tcp.local." is found.
		4. "dns-sd -L ameba _service1 local" command is used to look up this service instance, and the content of service text record is shown.

Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported IC :
                 RTL8730A, RTL872XE