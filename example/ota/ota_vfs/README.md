# Example Description

This example demonstrates image update using Over-the-Air (OTA) programming over Virtual File System (VFS).

The Ameba board downloads `ota_all.bin` from the VFS (FATFS) automatically. This method stores the OTA image in the internal flash file system, making it suitable for scenarios where network connectivity is not available.

**Key Features:**
- OTA update via internal flash FATFS file system
- No network connectivity required
- Suitable for offline image updates

# HW Configuration

```
+------------------+
|                  |
|  Ameba Board     |
|  (RTL8730E/...   |
|   RTL8726E, etc) |
|                  |
+------------------+
```

**Required Components:**
- Ameba development board
- USB cable for programming

**Connection Requirements:**
- Ameba board connected to PC via USB for initial programming
- No network connection required for OTA update via VFS

# SW Configuration

1. **Prepare Image Files**

   Prepare a folder containing all required files for the VFS image:
   ```bash
   # Example: Create image folder and copy ota_all.bin
   mkdir -p {SDK}/tools/image_scripts/image
   cp ota_all.bin {SDK}/tools/image_scripts/image/
   ```

2. **Generate FATFS Binary**

   Execute the vfs.py script to generate the FATFS binary file:
   ```bash
   cd {SDK}/tools/image_scripts
   python vfs.py -t FATFS -s 512 -c 256 -dir image -out fatfs.bin
   ```

3. **Copy FATFS Binary**

   Copy the generated `fatfs.bin` into the project folder:
   ```
   {SDK}/component/soc/xxx/project/
   ```

4. **Enable FATFS in APP Image**

   Enable FATFS within APP Image in menuconfig:
   ```
   CONFIG VFS --->
      [*] Enable VFS FATFS
          [*] FATFS within APP Image
   ```
   Save and exit.

5. **Select Image Switch Strategy**

   Refer to [Image Pattern and Version Number](https://aiot.realmcu.com/zh/latest/rtos/ota/index.html#version-number) to select the appropriate image switch strategy for your application.

6. **Build and Flash**
   - Refer to the [SDK Examples](https://aiot.realmcu.com/en/latest/rtos/sdk/sdk_example/index.html) to generate images
   - Download images to board using Ameba Image Tool

7. **Start OTA Update**

   Reset the board to start the download

# Expected Result

After booting, a VFS download example thread will be started automatically. The board will:
1. Initialize the VFS FATFS
2. Locate and read `ota_all.bin` from the FATFS
3. Download and verify the image integrity
4. Automatically reset and boot from the new image after successful OTA

**Note:** The board will boot from the new image only if:
- The image switch strategy is set to "switch based on valid tag"
- The new image version number is greater than the current image version number

**Success Indicators:**
- Serial console shows successful VFS initialization
- "OTA SUCCESS" message appears after image update
- Board automatically reboots with the new image

# Notes

- Ensure the FATFS binary is properly generated and copied to the project folder
- The VFS method does not require network connectivity
- The OTA image size should fit within the allocated FATFS space

# Supported IC

* RTL8730E
* RTL8726E
* RTL8720E
* RTL8713E
* RTL8710E
* RTL8721Dx
* RTL8721F
