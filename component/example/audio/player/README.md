# CUSTOMER IMPLEMENTATION DATA SOURCE PLAY EXAMPLE

## Description:
**RTPlayer provides the APIs to implement operations related to manage playback.
example_player.c gives an example of the detailed implementation of an RTPlayer.**

# HW Configuration
Required Components: speaker

connect the speaker to board.

## Sw Configuration:
Sw Configuration:
1. **Modify** `component/ssl/mbedtls-2.16.9/include/mbedtls/config_rsa.h`
    ```
    increase MBEDTLS_SSL_MAX_CONTENT_LEN to avoid mbedtls read error
       #define MBEDTLS_SSL_MAX_CONTENT_LEN     16*1024
    ```

2. **Modify** `component/lwip/api/lwipopts.h`
    ```
    increase PBUF_POOL_SIZE to avoid slow downloading speed when playing two http/https audio at the same time.
  	Modify
        #elif defined(ENABLE_AMAZON_COMMON)
	       #define PBUF_POOL_SIZE          30
	    #else
	       #define PBUF_POOL_SIZE          20
	    #endif
	to
        #elif defined(ENABLE_AMAZON_COMMON)
	       #define PBUF_POOL_SIZE          30
	    #else
	       #define PBUF_POOL_SIZE          38
	    #endif

       ...
       enable LWIP_SOCKET_SET_ERRNO for non-block socket connect
       #define LWIP_SOCKET_SET_ERRNO           1
    ```

3. **GCC:** use CMD `./build.py -a player -p` to compile example

4. If you want to use the functionality of caching files to flash
   **Modify** example_player.c
   //#define USE_CACHE to #define USE_CACHE

## Usage:
**player_test [OPTION...]**
[-F audio_path]          An audio file path to play
```
player -F http://192.168.31.226/1.mp3
```

## Result description
        the corresponding music played in the speaker.
