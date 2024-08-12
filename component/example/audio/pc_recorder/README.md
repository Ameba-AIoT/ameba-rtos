# PC RECORDER EXAMPLE

## Description:
**This example support audio record and playback.**

# Configuration:
GCC:use CMD "make all EXAMPLE=pc_recorder" to compile pc recorder example.

## For Example, run pc recorder on KM4.
1. only for record
	1). For GCC environmnet, type command "make menuconfig" in auto_build
		and choose
		(1)CONFIG WIFI => Enable Wifi => Processor Role(KM4_AP_KR4_NP)
		(2)MENUCONFIG FOR KM4 CONFIG => Audio Config => Enable Audio Framework => Select Audio Interfaces => Mixer
		save and exit.

	2). Rebuild project and download.

	3). Connect uart adapter with PC
	  (1)TX: PA_25, RX: PA_24
	  (2)baudrate: 1500000

	4). Default payload size is 2048 bytes, make sure settings on RtkAudioRecordTool.exe stay the same as demo.

	5). Run RtkAudioTestToolv1.0.1/RtkAudioTestTool.exe on PC, and start to record.

2. for record and playback
	1). For GCC environmnet, type command "make menuconfig" in auto_build
		and choose
		(1)CONFIG WIFI => Enable Wifi => Processor Role(KM4_AP_KR4_NP)
		(2)MENUCONFIG FOR KM4 CONFIG => Audio Config => Enable Audio Framework => Select Audio Interfaces => Mixer
		(3)=> Enable Midea Framework => Demux => Demux MP3
		(4)=> Codec => Codec MP3
		save and exit.

	2). Rebuild project and download.

	3). Connect uart adapter with PC
	  (1)TX: PA_25, RX: PA_24
	  (2)baudrate: 1500000

	4). Default payload size is 2048 bytes, make sure settings on RtkAudioRecordTool.exe stay the same as demo.

	5). Run RtkAudioTestToolv1.0.1/RtkAudioTestTool.exe on PC, browse mp3 music from PC, seleck playback and start to record.


## For Example, run pc recorder on KR4.
1. only for record
	1). For GCC environmnet, type command "make menuconfig" in auto_build
		and choose:
		(1)CONFIG WIFI => Enable Wifi => Processor Role(KR4_AP_KM4_NP)
		(2)MENUCONFIG FOR KR4 CONFIG => Audio Config => Enable Audio Framework => Select Audio Interfaces => Mixer
		save and exit.

	2). Rebuild project and download.

	3). Connect uart adapter with PC
	  (1)TX: PA_25, RX: PA_24
	  (2)baudrate: 1500000

	4). Default payload size is 2048 bytes, make sure settings on RtkAudioTestToolv1.0.1/RtkAudioTestTool.exe stay the same as demo.

	5). Run RtkAudioTestToolv1.0.1/RtkAudioTestTool.exe on PC, and start to record.

2. for record and playback
	1). For GCC environmnet, type command "make menuconfig" in auto_build
		and choose
		(1)CONFIG WIFI => Enable Wifi => Processor Role(KR4_AP_KM4_NP)
		(2)MENUCONFIG FOR KR4 CONFIG => Audio Config => Enable Audio Framework => Select Audio Interfaces => Mixer
		(3)=> Enable Midea Framework => Demux => Demux MP3
		(4)=> Codec => Codec MP3
		save and exit.

	2). Rebuild project and download.

	3). Connect uart adapter with PC
	  (1)TX: PA_25, RX: PA_24
	  (2)baudrate: 1500000

	4). Default payload size is 2048 bytes, make sure settings on RtkAudioTestToolv1.0.1/RtkAudioTestTool.exe stay the same as demo.

	5). Run RtkAudioTestToolv1.0.1/RtkAudioTestTool.exe on PC, browse mp3 music from PC, seleck playback and start to record.

## [Supported List]
	Supported:
		AmebaSmart, AmebaLite