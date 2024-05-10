d##################################################################################
#                                                                                #
#                           example_http_download                                #
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
        This example downloads file from Web server via http.


Setup Guide
~~~~~~~~~~~
       1. Modify SERVER_HOST, SERVER_PORT and RESOURCE in example_http_download.c based on your HTTP server
       2.GCC:use CMD "make all EXAMPLE=http_download" to compile http_download example.


Result description
~~~~~~~~~~~~~~~~~~
        A http download example thread will be started automatically when booting.
        Can make automatical Wi-Fi connection when booting by using wlan fast connect example..

	
Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported IC :
                RTL8730A, RTL872XE