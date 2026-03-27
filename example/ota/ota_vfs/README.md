# Example Description

This example is designed for firmware update via virtual file system.

Download `ota_all.bin` from the virtual file system automatically.

# HW Configuration

None

# SW configuration

1. Use the vfs.py to generate the FatFS binary file. 
	
	* Prepare a needed object folder including files before generating FatFS bin files. For example: `{SDK}/tools/image_scripts/image`
	* Copy `ota_all.bin` into `{SDK}/tools/image_scripts/image`
	* Execute the vfs.py
		```
		cd {SDK}/tools/image_scripts
		python vfs.py -t FATFS -s 512 -c 256 -dir image -out fatfs.bin
		```
	* Copy `fatfs.bin` into `component/soc/xxx/project/`

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

3. Reset the board and start the download.

# Expect result

A vitual file system download example thread will be started automatically when booting.

# Supported IC

* RTL8730E
* RTL8726E
* RTL8720E
* RTL8713E
* RTL8710E
* RTL8721Dx
* RTL8721F
