# Ameba Speech Development Kit

## Description

This folder contains Speech Development.

It shows how to use AIVoice flow with realtime audio stream and will be always-on.

**Audio functions such as recording and playback are integrated in the MCU. The example needs to be used with SpeechMind in MCU.**

The audio recorder will in send the 3 channel audio, channel 1 and channel 2 are microphone signals with 2mic50mm array and channel 3 is AEC reference signal. You can try saying “你好小强” “打开空调” “你好小强” “播放音乐” etc.

**Note: AFE res, kws lib, fst lib should match the content of audio, otherwise AIVoice can not detect.**

## Supported IC
1. AmebaLite
2. AmebaSmart

## HW Configuration
Required Components: speaker
connect the speaker to board.

## AmebaSmart
### Sw Configuration:
1. Add the speechmind into image build.
Add the following line under the \
**###############################  ADD COMPONENT ###################################**\
section in the file\
**amebasmart_gcc_project/project_ap/asdk/make/image2/CMakeLists.txt:**
```
ameba_add_subdirectory_if_exist(${c_CMPT_APP_DIR}/speechmind)
```

2. Menuconfig
Type command `./menuconfig.py` under the project directory and :
```
CONFIG Link Option  --->
    IMG2(Application) running on PSRAM or FLASH? (FLASH)  --->
CONFIG APPLICATION  --->
    Audio Config  --->
        -*- Enable Audio Framework
            Select Audio Interfaces (Mixer)  --->
            Audio Devices  --->
        [*] Enable Media Framework
            Demux  --->
                [*] Demux MP3
            Codec  --->
                [*] Codec MP3
    AI Config  --->
        [*] Enable TFLITE MICRO
        [*] Enable AIVoice
            Select AFE Resource (afe_res_2mic50mm)  --->
            Select VAD Resource (vad_v7_200K)  --->
            Select KWS Resource (kws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v4_300K)  --->
            Select ASR Resource (asr_cn_v8_2M)  --->
```

### Build the resource bin
1. Modify
```
{VFS1,          0x08623000, 0x08642FFF},
```
to
```
{VFS1,          0x08623000, 0x087FFFFF},
```
in the file\
**component/soc/usrcfg/amebasmart/ameba_flashcfg.c**

2. cp tools/littlefs/linux/mklittlefs component/application/speechmind/res/
3. Set the file permissions with the following command:\
   **chmod 777 mklittlefs**
4. Caculate the storage capacity of resource :\
**0x087FFFFF+1-0x08623000 = 0x1DD000**
5. use the follow cmd to build:\
**./mklittlefs -b 4096 -p 256 -s 0x1DD000 -c test test.bin**

### Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * Add the Image Name **test.bin** At the last line with start address:**0x08623000**, end address:**0x08800000** by Ameba Image Tool.
   * `Download` images to board by Ameba Image Tool.

## AmebaLite
### Build the dsp bin
According to the examples/speechmind_demo/README.md in the aivoice repository.

### Sw Configuration:
1. Add the speechmind into image build.
Add the following line under the \
**###############################  ADD COMPONENT ###################################**\
section in the file\
**amebalite_gcc_project/project_km4/asdk/make/image2/CMakeLists.txt:**
```
ameba_add_subdirectory_if_exist(${c_CMPT_APP_DIR}/speechmind)
```

2. Menuconfig
Type command `./menuconfig.py` under the project directory and :
```
CONFIG DSP Enable  --->
    [*] Enable DSP
CONFIG Link Option  --->
    IMG2(Application) running on FLASH or PSRAM? (CodeInXip_DataHeapInPsram)  --->
CONFIG APPLICATION  --->
    Audio Config  --->
        -*- Enable Audio Framework
            Select Audio Interfaces (Mixer)  --->
            Audio Devices  --->
        [*] Enable Media Framework
            Demux  --->
                [*] Demux MP3
            Codec  --->
                [*] Codec MP3
    IPC Message Queue Config  --->
        [*] Enable IPC Message Queue
        [*]     Enable RPC
    AI Config  --->
        [*] Enable TFLITE MICRO
```

### Build the resource bin
1. Modify
```
{VFS1,          0x083E0000, 0x083FFFFF}
```
to
```
{VFS1,          0x083E0000, 0x085BCFFF},
```
in the file\
**component/soc/usrcfg/amebalite/ameba_flashcfg.c**

2. cp tools/littlefs/linux/mklittlefs component/application/speechmind/res/
3. Set the file permissions with the following command:\
   **chmod 777 mklittlefs**
4. Caculate the storage capacity of resource :\
**0x085BCFFF+1-0x083E0000 = 0x1DD000**
5. use the follow cmd to build:\
**./mklittlefs -b 4096 -p 256 -s 0x1DD000 -c test test.bin**

### Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * Add the Image Name **test.bin** At the last line with start address:**0x083E0000**, end address:**0x085BD000** by Ameba Image Tool.
   * `Download` images to board by Ameba Image Tool.

