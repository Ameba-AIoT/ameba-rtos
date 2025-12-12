# Example Description

This example demonstrates how to use the VFS module for data encryption. Unlike standard situations, `vfs_set_user_encrypt_callback` must be invoked prior use the VFS module for data storage. Encryption algorithm interface and keys should be prepared in advance. Plaintxt will be padded according to the length of grouped data.

Here is the prototype of the interface :
```C
void vfs_set_user_encrypt_callback(char *prefix, vfs_encrypt_callback_t encrypt_func, vfs_decrypt_callback_t decrypt_func, unsigned char iv_len);
```
```
prefix : decided by user when registering the vfs (refer file system AN for more information)
encrypt_func : func which provided by user to encrypt data
decrypt_func : func which provided by user to decrypt data
iv_len : length of grouped data in encryption algorithms (which decide data align length stored in memory)
```

# HW Configuration

None

# SW configuration

1. Modify the interface function `app_filesystem_init()` located in `amebaXXX_gcc_project/project_<core>/src/main.c` to config your expected test mode.
Test Mode: (`VFS_LITTLEFS`/`VFS_FATFS`, `VFS_INF_FLASH`/`VFS_INF_SD`)
   - For `VFS_LITTLEFS` with `VFS_INF_FLASH`:
		```C
		ret = vfs_user_register(VFS_PREFIX, VFS_LITTLEFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
		```

   - For `VFS_FATFS` with `VFS_INF_FLASH`:
		```C
		ret = vfs_user_register(VFS_PREFIX, VFS_FATFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
		```

   - For `VFS_FATFS` with `VFS_INF_SD`: (Only AmebaSmart support)
		```C
		ret = vfs_user_register(VFS_PREFIX, VFS_FATFS, VFS_INF_SD, VFS_REGION_1, VFS_RW);
		```

2. Type command `./menuconfig.py` and choose `CONFIG VFS`
   - For `VFS_LITTLEFS`:
		```
		[*] Enable VFS LITTLEFS
		[ ] Enable VFS FATFS
		```

   - For `VFS_FATFS` with `VFS_INF_FLASH`:
		```
		[ ] Enable VFS LITTLEFS
		[*] Enable VFS FATFS
		FATFS Memory Type (FlASH)
		```

   - For `VFS_FATFS` with `VFS_INF_SD`: (Only AmebaSmart support)
		```
		[ ] Enable VFS LITTLEFS
		[*] Enable VFS FATFS
		FATFS Memory Type (SD)
		```

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Write vfs file (file_name = "vfs_file_1", value = "VFS module works normally with encrypt function !!!") succeeded

2. Read vfs file succeeded

3. Delete vfs file succeeded

# Note

None

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
