
This example is for virtual file system module which wraps littlefs/fatfs.

[Setup Guide]
~~~~~~~~~~~
	    1. In project/realtek_ameba<test board>_va0_example/src/src_<ap core>/main.c "app_filesystem_init" interface CONFIG your expected test mode.(VFS_LITTLEFS/VFS_FATFS, VFS_INF_FLASH/VFS_INF_SD)
	    2. If changed interface type under fatfs, FATFS_DISK_XX should be set to 1 in component/file_system/vfs/vfs_fatfs.h.
	    3. GCC: use CMD "make all EXAMPLE=vfs" to compile kv example. 

[Supported List]
~~~~~~~~~~
	    AmebaLite
	    AmebaSmart
	    AmebaDplus

Result description
~~~~~~~~~~~~~~~~~~
	   Example Steps:
			1. Write vfs file (file_name= vfs_key, value = VFS module works normally !!!)
			2. Read vfs file
			3. Delete vfs file