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
   - `config_rsa.h`
		```C
		#define MBEDTLS_SSL_MAX_CONTENT_LEN 16384
		```

   - `example_websocket_client.c`
		```C
		wsclient_context *wsclient = create_wsclient("ws://websocket-echo.com", 0, NULL, NULL, 1500, 3);
		```

3. If using the WSS server:
   - `config_rsa.h`
		```C
		#define MBEDTLS_SSL_MAX_CONTENT_LEN	16384
		#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
		#define MBEDTLS_ECDH_C
		```

   - `example_websocket_client.c`
		```C
		wsclient_context *wsclient = create_wsclient("wss://websocket-echo.com", 0, NULL, NULL, 1500, 3);
		```

4. How to use:
   - Run `./build.py -a websocket_client` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

Can make automatical Wi-Fi connection (to AP with Internet access that can access the given server link) when booting by using wlan fast connect example.

A websocket client example thread will be started automatically when booting.

If using other websocket server, modify the `create_wsclient()` API and the `handle_message()` function depending on the condition of the server.

# Note

For WS server, the below warning message can be ignored during execution.
```
WARNING(lwip_getsockopt): EWOULDBLOCK(EAGAIN) IS NOT SO_ERROR(sockets.c:2985)
```

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
