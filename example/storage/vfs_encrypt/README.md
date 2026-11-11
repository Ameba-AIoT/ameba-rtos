# Example Description

This example demonstrates how to use the VFS module for data encryption. Unlike standard situations, `vfs_set_user_encrypt_callback` must be invoked prior use the VFS module for data storage. Encryption algorithm interface and keys should be prepared in advance.

Here is the prototype of the interface :
```C
void vfs_set_user_encrypt_callback(char *prefix, vfs_enc_callback_t encrypt_func, vfs_dec_callback_t decrypt_func, unsigned char iv_len);
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

1. The tested target is chosen by Kconfig in `example_vfs_encrypt.c`; the caller never passes a file system type.
Test Mode: (target, file system)
   - `VFS_TARGET_FLASH_VFS1` with LittleFS (default, registered at boot by `vfs_auto_register()`)
   - `VFS_TARGET_FLASH_VFS2` with FATFS (`CONFIG_VFS2_FATFS=y`)
   - `VFS_TARGET_SD` with FATFS (`CONFIG_VFS_SD_CARD=y`, `CONFIG_VFS_SD_FATFS=y`), registered by the example:
		```C
		ret = vfs_target_register("sdcard", VFS_TARGET_SD, VFS_RW);
		```
   The prefix used by the example is obtained with `vfs_get_prefix(<target>)`.

2. Use menuconfig and choose `CONFIG File System`, or apply a ready-made config:
   - VFS1 LittleFS: no extra config needed
   - VFS2 FATFS: `fatfs.conf`
   - SD FATFS: `fatfs_sd.conf`

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
RTL8735C
RTL8720F
