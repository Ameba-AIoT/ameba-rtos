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
	1. In project/realtek_ameba<test board>_va0_example/src/src_<ap core>/main.c "app_filesystem_init" interface CONFIG your expected test mode.(VFS_LITTLEFS/VFS_FATFS, VFS_INF_FLASH/VFS_INF_SD)
	2. GCC: use CMD "make all EXAMPLE=vfs_encrypt" to compile vfs encryption example. 

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