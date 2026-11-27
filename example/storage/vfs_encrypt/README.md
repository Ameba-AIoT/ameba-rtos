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

None. Insert an SD card when the SD card is the volume under test (see SW configuration).

# SW configuration

1. The volume under test is selected by menuconfig (`CONFIG File System`), no source change is needed:
   - SD card (FATFS): `CONFIG_VFS_SD_FATFS=y` (`CONFIG_VFS_SD_CARD=y`). The example registers it with `vfs_target_register("sdcard", VFS_TARGET_SD, VFS_RW)`.
   - VFS2 (FATFS): `CONFIG_VFS2_FATFS=y`, or build with the bundled config:
     ```
     ./ameba.py menuconfig -f example/storage/vfs_encrypt/fatfs.conf
     ```
     The flash layout must reserve a VFS2 partition, otherwise the example exits with `VFS target unavailable`.
     The example checks `CONFIG_VFS_SD_FATFS` before `CONFIG_VFS2_FATFS`, so on a SoC that enables the SD card by default (`RTL8735C`) also disable `CONFIG_VFS_SD_CARD`, otherwise it keeps testing the SD card. On `RTL8735C` the default NOR layout has a zero-sized VFS2 region, so VFS2 needs the NAND layout.
   - VFS1 (LittleFS): neither of the above is enabled.

   The bundled `fatfs_sd.conf` selects the SD card:
   ```
   ./ameba.py menuconfig -f example/storage/vfs_encrypt/fatfs_sd.conf
   ```

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Write vfs file (file_name = "vfs_file_1", value = "VFS module works normally with encrypt function !!!") succeeded

2. Read vfs file succeeded

3. Delete vfs file succeeded

# Note

On `RTL8735C` the default config enables the SD card (`CONFIG_VFS_SD_CARD=y`), so this example tests the SD card, insert an SD card before running it. To test VFS1, disable `CONFIG_VFS_SD_CARD`.

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
