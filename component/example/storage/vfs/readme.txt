This example is for virtual file system module which wraps littlefs/fatfs.

[Setup Guide]
~~~~~~~~~~~
	1. In /ameba<test board>/project_<core>/src/main.c "app_filesystem_init" interface CONFIG your expected test mode.
		Test Mode: (VFS_LITTLEFS/VFS_FATFS, VFS_INF_FLASH/VFS_INF_SD)

		For VFS_LITTLEFS with VFS_INF_FLASH:
		ret = vfs_user_register("lfs", VFS_LITTLEFS, VFS_INF_FLASH, VFS_FLASH_R1, VFS_RW);

		For VFS_FATFS with VFS_INF_FLASH:
		ret = vfs_user_register("fatfs", VFS_FATFS, VFS_INF_FLASH, VFS_FLASH_R1, VFS_RW);

		For VFS_FATFS with VFS_INF_SD: (Only AmebaSmart support)
		ret = vfs_user_register("fatfs", VFS_FATFS, VFS_INF_SD, VFS_FLASH_R1, VFS_RW);

	2. Type command "make menuconfig" and choose CONFIG VFS
		For VFS_LITTLEFS:
		[*] Enable VFS LITTLEFS
		[ ] Enable VFS FATFS

		For VFS_FATFS with VFS_INF_FLASH:
		[ ] Enable VFS LITTLEFS
		[*] Enable VFS FATFS
		(FlASH) FATFS Memory Type

		For VFS_FATFS with VFS_INF_SD: (Only AmebaSmart support)
		[ ] Enable VFS LITTLEFS
		[*] Enable VFS FATFS
		(SD) FATFS Memory Type

	3. GCC: use CMD "make all EXAMPLE=vfs" to compile vfs example. 

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

