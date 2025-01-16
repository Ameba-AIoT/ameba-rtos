##################################################################################
#                                                                                #
#                              example_atcmd_spi_master                          #
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
        In this example, the control of the AT command module via SPI is simply demonstrated. In the example, 
		the input to SPI is sourced from the UART interface, and the return output from the AT command module 
		is sent to the UART interface.
		
		In this example, two synchronized GPIOs need to be configured to notify the AT command SPI slave 
		to send data and to receive the signal indicating that the slave is ready.

Setup Guide
~~~~~~~~~~~
		1. Modify the SPI and UART configurations as needed. (like pins, spi clock frequence, uart baudrate)
		
		2. GCC: use CMD "./build.py -a atcmd_spi_master" to compile spi_master example.


Result description
~~~~~~~~~~~~~~~~~~
		You can send AT command instructions through the UART port and view the results.s

Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported IC :
                 Amebadplus