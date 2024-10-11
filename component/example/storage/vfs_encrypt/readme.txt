Description
~~~~~~~~~~~
	This example demonstrates how to use the VFS module for data encryption. Unlike standard situations, "vfs_set_user_encrypt_callback" must be invoked prior use the VFS module for data storage. Encryption algorithm interface and keys should be prepared in advance. Plaintxt will be padded according to the length of grouped data.
	Here is the prototype of the interface :
		void vfs_set_user_encrypt_callback(char *prefix, vfs_encrypt_callback_t encrypt_func, vfs_decrypt_callback_t decrypt_func, unsigned char iv_len);
			prefix : decided by user when registering the vfs (refer file system AN for more information)
			encrypt_func : func which provided by user to encrypt data
			decrypt_func : func which provided by user to decrypt data
			iv_len : length of grouped data in encryption algorithms (which decide data align length stored in memory)

[Setup Guide]
~~~~~~~~~~~
	AmebaSmart		: Support Flash / SD card.
	AmebaLite & AmebaDplus	: Only support Flash.

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

	3. GCC: use CMD "make all EXAMPLE=vfs_encrypt" to compile vfs encryption example. 

[Supported List]
~~~~~~~~~~
	AmebaLite
	AmebaSmart
	AmebaDplus

Result description
~~~~~~~~~~~~~~~~~~
	Example Steps:
		1. Write vfs file (file_name= vfs_key, value = VFS module works normally with encrypt function !!!)
		2. Read vfs file
		3. Delete vfs file

