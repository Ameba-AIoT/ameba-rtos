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
	a.use CMD "./menuconfig.py" to show configurations;
	b.choose "CONFIG LWIP";
	c.choose "NAT REPEATER Enable".
		
2.Use CMD "./build.py -a wifi_nat_repeater -p" to compile nat repeater example.
	
Result description
~~~~~~~~~~~~~~~~~~
1.A softap will be started automatically after booting. 
2.Use atcmd to connect to the public WiFi under sta mode, then use cellphone to connect to softap. 
  If sta has linked to public WiFi before, it will connect to it automatically by fast connect after booting which means you don't have to use atcmd.
3.If sta has successfully connected to public WiFi which can access wan, then cellphone can also access wan.

	How to use atcmd to connect to public WiFi under sta mode:
	use CMD "AT+WLCONN=ssid,xxx,pw,xxx"; if public WiFi is set with password otherwise don't need "pw,xxx";

Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported :
	      amebadplus