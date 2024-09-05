802.1X EAP METHOD SUPPLICANT EXAMPLE

Description:
Use 802.1X EAP methods to connect to AP and authenticate with backend radius server.
Current supported methods are EAP-TLS, PEAPv0/EAP-MSCHAPv2, and EAP-TTLS/MSCHAPv2.

Configuration:
1.Select which EAP method to compile in app_example.
(for example:component/example/wifi/wifi_enterprise/app_example.c)
	void app_example(void)
	{
		example_eap("tls");
		//example_eap("peap");
		//example_eap("ttls");
	}

2.Modify the connection config (ssid, identity, password, cert) in example_eap_config() of example_eap.c based on your server's setting.
(for example:component/example/wifi/wifi_enterprise/example_eap.c)
	a) eap_target_ssid - AP(that supports enterprise mode) SSID.
	b) eap_identity/eap_password - client name and secret from server configuration file.
	c) eap_client_key_pwd - default "" if the eap client key is already decrypted, else the client key password to decrypt client key can be found from server configuration file.
	d) eap_client_cert/eap_client_key needed only for EAP-TLS method and that can be found from server certs.
	e) eap_ca_cert needed to be provided from server certs if "ENABLE_EAP_SSL_VERIFY_SERVER" macro is assigned to 1 in "autoconf_eap.h".

3. Get config arguments from wifi_eap_config.c
(for example:component/wifi/driver/include/autoconf_eap.h)
	add #define PRE_CONFIG_EAP in autoconf_eap.h

4. Enable specified eap method
(for example:component/wifi/driver/include/autoconf_eap.h)
	modify the value of CONFIG_ENABLE_PEAP to 1 when use PEAP

5. For RTL8730A, RTL872XE, lib_eap is not included by default, it need to be add to your project and then build.
	Build lib_eap
    (for example:project/realtek_amebaDplus_va0_example/GCC-RELEASE/project_km4/asdk/make/network/Makefile)
	(for example:amebasmart_gcc_project/project_ap/asdk/make/network/Makefile)
		make -C eap all
		make -C eap clean

6. Add lib_eap.a in Makefile
   (for example:project/realtek_amebaDplus_va0_example/GCC-RELEASE/project_km4/asdk/make/Makefile.lib)
	Uncomment $(APP_LIB_DIR)/lib_eap.a in APP_LIB

7.GCC:use CMD "make all EXAMPLE=wifi_enterprise" to compile wifi_enterprise example

Execution:
An EAP connection thread will be started automatically when booting.

Trouble Shooting:
	ERROR: [eap_recvd]Malloc failed
	Solution: Increase the FreeRTOS heap in FreeRTOSConfig.h, 
       	#define configTOTAL_HEAP_SIZE	( ( size_t ) ( XX * 1024 ) )

Note:
Please make sure the lib_wps, polarssl/mbedtls, ssl_ram_map are also builded.
If the connection failed, you can try the following directions to make it work:
1. Make sure the config_rsa.h of PolarSSL/MbedTLS include the SSL/TLS cipher suite supported by radius server.
2. Set a larger value to SSL_MAX_CONTENT_LEN in config_rsa.h
3. Try to change using SW crypto instead of HW crypto.


[Supported List]
	Supported :
	    RTL8730A, RTL872XE
		
Note:
WPA3 Enterprise is not supported.
For RTL8730A, RTL872XE, lib_eap is not included by default, it need to be add to your project and then build.
For IAR, please add lib_eap.a to the lib fold in km4_application project.
