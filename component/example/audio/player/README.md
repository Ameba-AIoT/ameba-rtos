# CUSTOMER IMPLEMENTATION DATA SOURCE PLAY EXAMPLE

## Description:
**RTPlayer provides the APIs to implement operations related to manage playback.
example_player.c gives an example of the detailed implementation of an RTPlayer.**

# HW Configuration
Required Components: speaker

connect the speaker to board.

## Sw Configuration:
* On AmebaSmart
1. **Enlarge heap** when playing two audio files at the same time.
amebasmart_gcc_project/amebasmart_layout.ld
modify
       CA32_BL3_DRAM_NS (rwx) :                ORIGIN = 0x60300000, LENGTH = 0x60700000 - 0x60300000   /* CA32 BL3 DRAM NS: 4MB */
to
       CA32_BL3_DRAM_NS (rwx) :                ORIGIN = 0x60300000, LENGTH = 0x62000000 - 0x60300000   /* CA32 BL3 DRAM NS: 4MB */

2. **Enable playback configurations**:
cd `amebasmart_gcc_project`: `make menuconfig`
    < MENUCONFIG FOR AP CONFIG  --->
    [*] Enable Audio Framework
    (Mixer) Select Audio Interfaces
    < Audio Config  --->
    [*] Enable Media Framework,
    Demux  ---> choose the extractors,
    Codec  ---> choose the codecs.

    The choices of Demux and Codec are as follows:
    support audio format : WAV (Demux WAV + Codec PCM)
    support audio format : MP3 (Demux MP3 + Codec MP3)
    support audio format : AAC (Demux AAC + Codec AAC)
    support audio format : MP4 or M4A (Demux M4A + Codec AAC)
    support audio format : FLAC (Demux FLAC)
    support audio format : OGG or OPUS (Demux OGG + Codec OPUS)

3. **Modify** `component/ssl/mbedtls-2.16.9/include/mbedtls/config_rsa.h`
    ```
    increase MBEDTLS_SSL_MAX_CONTENT_LEN to avoid mbedtls read error
       #define MBEDTLS_SSL_MAX_CONTENT_LEN     16*1024
    ```

4. **Modify** `component/lwip/api/lwipopts.h`
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

5. **GCC:** use CMD `make all EXAMPLE=player` to compile example

* On AmebaLite
1. **Enlarge heap** when playing two audio files at the same time.
cd `amebalite_gcc_project`: `make menuconfig`
CONFIG Link Option
choise IMG2(Application) running on FLASH or PSRAM?
choise CodeInPsram_DataHeapInPsram

vim `amebalite_gcc_project/amebalite_layout.ld`
modify
#define PSRAM_KM4_IMG2_SIZE                     (KBYTES(1536) - (PSRAM_TZ_NSC_SIZE + PSRAM_TZ_ENTRY_SIZE + PSRAM_KM4_IMG3_SIZE))
to
#define PSRAM_KM4_IMG2_SIZE                     (KBYTES(2536) - (PSRAM_TZ_NSC_SIZE + PSRAM_TZ_ENTRY_SIZE + PSRAM_KM4_IMG3_SIZE))

1. **Enable playback configurations**:
    * For player running in KR4:
        cd `amebalite_gcc_project`: `make menuconfig`
        < MENUCONFIG FOR KR4 CONFIG  --->
        [*] Enable Audio Framework
        (Mixer) Select Audio Interfaces
        < Audio Config  --->
        [*] Enable Media Framework,
        Demux  ---> choose the extractors,
        Codec  ---> choose the codecs.

    * For player running in KM4:
       cd `amebalite_gcc_project`: `make menuconfig`
        < MENUCONFIG FOR KM4 CONFIG  --->
        [*] Enable Audio Framework
        (Mixer) Select Audio Interfaces
        < Audio Config  --->
        [*] Enable Media Framework,
        Demux  ---> choose the extractors,
        Codec  ---> choose the codecs.

    The choices of Demux and Codec are as follows:
    support audio format : WAV (Demux WAV + Codec PCM)
    support audio format : MP3 (Demux MP3 + Codec MP3)
    support audio format : AAC (Demux AAC + Codec AAC)
    support audio format : MP4 or M4A (Demux M4A + Codec AAC)
    support audio format : FLAC (Demux FLAC)
    support audio format : OGG or OPUS (Demux OGG + Codec OPUS)


3. **Modify** `component/ssl/mbedtls-2.16.9/include/mbedtls/config_rsa.h`
    ```
    increase MBEDTLS_SSL_MAX_CONTENT_LEN to avoid mbedtls read error
       #define MBEDTLS_SSL_MAX_CONTENT_LEN     16*1024
    ```

4. **Modify** `component/lwip/api/lwipopts.h`
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

4. **Config wifi**
    * For player running in KR4:
        * cd `project/realtek_amebaLite_va0_example/GCC-RELEASE/auto_build:` `make menuconfig`
        < CONFIG WIFI  ---> [ * ] Enable Wifi, (KR4_AP_KM4_NP) Processor Role

    * For player running in KM4:
        * cd `project/realtek_amebaLite_va0_example/GCC-RELEASE/auto_build`: `make menuconfig`
        < CONFIG WIFI  ---> [ * ] Enable Wifi, (KM4_AP_KR4_NP) Processor Role

5. **GCC:** use CMD `make all EXAMPLE=player` to compile example

## Usage:
**player_test [OPTION...]**
[-F audio_path]          An audio file path to play
```
player -F http://192.168.31.226/1.mp3
```

## Result description
        the corresponding music played in the speaker.