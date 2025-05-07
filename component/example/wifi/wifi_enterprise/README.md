# Example Description

Use 802.1X EAP methods to connect to AP and authenticate with backend radius server.

Current supported methods are EAP-TLS, PEAPv0/EAP-MSCHAPv2, and EAP-TTLS/MSCHAPv2.

# HW Configuration

None

# SW configuration

1. In `component/soc/amebaxxx/usrcfg/ameba_wificfg.c`.
	```C
	wifi_user_config.fast_reconnect_en = 0;
	```

2. Select which EAP method to compile in `app_example`.
	```C
	void app_example(void)
	{
		example_eap("tls");
		//example_eap("peap");
		//example_eap("ttls");
	}
	```

3. Modify the connection config (ssid, identity, password, cert) in `example_eap_config()` of `example_wifi_enterprise.c` based on your server's setting.
	1. `eap_target_ssid` - AP (that supports enterprise mode) SSID.
	2. `eap_identity`/`eap_password` - client name and secret from server configuration file.
	3. `eap_client_key_pwd` - default "" if the eap client key is already decrypted, else the client key password to decrypt client key can be found from server configuration file.
	4. `eap_client_cert`/`eap_client_key` needed only for EAP-TLS method and that can be found from server certs.
	5. eap_ca_cert needed to be provided from server certs if `ENABLE_EAP_SSL_VERIFY_SERVER` macro is assigned to 1 in `autoconf_eap.h`.

4. Get config arguments from `wifi_eap_config.c`  
(for example: `component/wifi/driver/include/autoconf_eap.h`)
   - add `#define PRE_CONFIG_EAP` in `autoconf_eap.h`

5. Enable specified eap method.  
(for example: `component/wifi/driver/include/autoconf_eap.h`)
   - modify the value of `CONFIG_ENABLE_PEAP` to 1 when use PEAP

6. For RTL8730A, RTL872XE, lib_eap is not included by default, it need to be add to your project and then build.  
   (for example: `amebadplus_gcc_project/project_km4/asdk/make/network/CMakeLists.txt`)
   - Add `add_subdirectory(eap)`

   (for example: `amebasmart_gcc_project/project_ap/asdk/make/network/CMakeLists.txt`)
   - Uncomment `eap`

7. Add lib_eap.a in cmake  
   (for example: `amebadplus_gcc_project/GCC-RELEASE/project_km4/libs.cmake`)
   - Uncomment `${APP_LIB_DIR}/lib_eap.a`

   (for example: `component/build_info/dependency_common.cmake`)
   - Uncomment `${d_SDK_LIB_APPLICATION_DIR}/lib_eap.a`

8. How to use:
   - Run `./build.py -a wifi_enterprise` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

An EAP connection thread will be started automatically when booting.

# Note

Please make sure the lib_wps, polarssl/mbedtls, ssl_ram_map are also builded.

If the connection failed, you can try the following directions to make it work:
1. Make sure the `mbedtls_config.h` of PolarSSL/MbedTLS include the SSL/TLS cipher suite supported by radius server.
2. Set a larger value to `SSL_MAX_CONTENT_LEN` in `mbedtls_config.h`
3. Try to change using SW crypto instead of HW crypto.

WPA3 Enterprise is not supported.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
