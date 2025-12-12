# Example Description

A simple websocket client example which send "hello" and "world" to server.

The server will reply the message it received.

Once the client received "world", it will disconnect with server.

The SSL websocket server:
```
	wss://websocket-echo.com
```

The websocket server without SSL:
```
	ws://websocket-echo.com
```

# HW Configuration

None

# SW configuration

1. Both WS client and WSS client are supported by this example, and can be changed by modifying `USING_SSL` in `wsclient_api.h`.
   - `wsclient_api.h`
		```C
		#define USING_SSL //using the WSS server
		#define USING_SSL //using the WS server,comment out this macro
		```
2. If using the WS server:
   - `example_websocket_client.c`
		```C
		wsclient_context *wsclient = create_wsclient("ws://websocket-echo.com", 0, NULL, NULL, 1500, 1500, 3);
		```

3. If using the WSS server:
   - `mbedtls_config.h`
		```C
		#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
		#define MBEDTLS_ECDH_C
		```

   - `example_websocket_client.c`
		```C
		wsclient_context *wsclient = create_wsclient("wss://websocket-echo.com", 0, NULL, NULL, 1500, 1500, 3);
		```

4. If encounter the following errors in an SSL connection. Set `MBEDTLS_SSL_IN_CONTENT_LEN` by `./menuconfig.py` and choose `CONFIG SSL`-> `Maximum len of incoming fragments` -> set large size.
	```C
	#define MBEDTLS_ERR_SSL_BAD_INPUT_DATA                    -0x7100
	#define MBEDTLS_ERR_SSL_INVALID_RECORD                    -0x7200
	```

5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

Can make automatical Wi-Fi connection (to AP with Internet access that can access the given server link) when booting by using wlan fast connect example.

A websocket client example thread will be started automatically when booting.

If using other websocket server, modify the `create_wsclient()` API and the `handle_message()` function depending on the condition of the server.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
