##################################################################################
#                                                                                #
#                             Example wifi_nat_repeater                          #
#                                                                                #
##################################################################################

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Result description
 - Supported List

 
Description
~~~~~~~~~~~
    A simple usage of nat repeater. 
	
Setup Guide
~~~~~~~~~~~
1.Define CONFIG_IP_NAT by selecting configuration in menuconfig：
	a.use CMD "make menuconfig" to show configurations;
	b.1)choose "MENUCONFIG FOR KM4 CONFIG" if you use amebadplus;
	  2)choose "MENUCONFIG FOR CA32 CONFIG" if you use amebasmart;
	c.choose "LWIP Config";
	d.choose "NAT REPEATER Enable".
		
2.Use CMD "make all EXAMPLE=wifi_nat_repeater" to compile nat repeater example.
	
Result description
~~~~~~~~~~~~~~~~~~
1.A softap will be started automatically after booting. 
2.Use atcmd to connect to the public WiFi under sta mode, then use cellphone to connect to softap. 
  If sta has linked to public WiFi before, it will connect to it automatically by fast connect after booting which means you don't have to use atcmd.
3.If sta has successfully connected to public WiFi which can access wan, then cellphone can also access wan.

	How to use atcmd to connect to public WiFi under sta mode:
	a.use CMD "ATW0=SSID";
	b.use CMD "ATW1=PWD" if public WiFi is set with password otherwise don't need this CMD;
	c.use CMD "ATWC".

Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported :
	      amebadplus amebasmart