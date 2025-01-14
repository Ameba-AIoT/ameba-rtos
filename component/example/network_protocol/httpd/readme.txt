##################################################################################
#                                                                                #
#                             example_httpd                                      #
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
        This example is based on HTTPD API, an HTTP/HTTPS server example with a simple homepage and GET/POST method test pages are provided.
	
Setup Guide
~~~~~~~~~~~
1.In lwipopts.h, please set #define SO_REUSE  1  

2.Both HTTP and HTTPS are supported by this exmaple, and can be changed by modifying USE_HTTPS in example_httpd.c.
	#define USE_HTTPS    0 //for HTTP
	#define USE_HTTPS    1 //for HTTPS

3.[config_rsa.h]:
	#define MBEDTLS_CERTS_C
	#define MBEDTLS_SSL_SRV_C
	
4.GCC:use CMD "make all EXAMPLE=httpd" to compile httpd example.

       
        Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
		
Result description
~~~~~~~~~~~~~~~~~~
        This example is a httpd example thread is started automatically when booting.
        Can test with a Web browser connecting to the homepage of httpd server.
        Should link PolarSSL bignum.c to SRAM to speed up SSL handshake if starting HTTPS server.

Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported IC :
                RTL8730A, RTL872XE

