# PC RECORDER EXAMPLE

## Description:
**This example support audio record and playback.**

# Configuration:
GCC:use CMD "./build.py -a pc_recorder -p" to compile pc recorder example.

## For Example.
	1). Rebuild project and download.

	2). Connect uart adapter with PC
	  (1)TX: PA_25, RX: PA_24
	  (2)baudrate: 1500000

	3). Default payload size is 2048 bytes, make sure settings on RtkAudioRecordTool.exe stay the same as demo.

	4). Run RtkAudioTestToolv1.0.1/RtkAudioTestTool.exe on PC, and start to record.


## [Supported List]
	Supported:
		AmebaSmart, AmebaLite
