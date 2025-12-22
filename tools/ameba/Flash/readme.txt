<<<<<<< HEAD   (fc8555 [tools]Fix nand flash download fail issue)
<Usage of AmebaFlash.py for Ameba SoCs>

AmebaFlash.py options:
  -h, --help            show this help message and exit
  -d DOWNLOAD, --download DOWNLOAD
                        download images
  -p PORT [PORT ...], --port PORT [PORT ...]  
                        serial port
  -b BAUDRATE, --baudrate BAUDRATE
                        serial port baud rate
  -f PROFILE, --profile PROFILE
                        device profile
  -i IMAGE, --image IMAGE
                        single image
  -r IMAGE-DIR, --image-dir IMAGE-DIR
                        image directory
  -e ERASE, --erase ERASE
                        erase flash
  -m {nor,nand,ram}, --memory-type {nor,nand,ram}, default is nor,
                        specified memory type
  -a START_ADDRESS, --start-address START_ADDRESS
                        start address, hex
  -n END_ADDRESS, --end-address END_ADDRESS
                        end address, hex
  -z SIZE, --size SIZE  size in KB
  -o LOG_FILE, --log-file LOG_FILE
                        set log file with path
  -v, --version         show program's version number and exit
  
  --chip-erase CHIP_ERASE
                        chip erase flag option download
  --layout-list LAYOUT_LIST
                        layout info, list
  --log-level LOG_LEVEL
                        set log level						

command e.g.:
> download single image
  --download
  --start-address, start address to download
  --profile, device profile
  --image, image file
  --port, serial port
  --baudrate, download baudrate
  ./AmebaFlash.py --download --profile E:\git_repo\meta_tools\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --start-address 0x08000000 --image "D:\Images\image_dplus\white space\image_all.bin"  --port COM92 --baudrate 1500000  

> download images with profile layout
  --download
  --start-address, start address to download
  --profile, device profile
  --image-dir, image path where store images and image name should be match with ImageName defined in device profile
  --port, serial port
  --baudrate, download baudrate
  ./AmebaFlash.py --download --profile E:\git_repo\meta_tools\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --image-dir "D:\Images\image_dplus\white space" --port COM92 --baudrate 1500000 
 
> chip erase
  erase full chip, only useful for nor flash
  --chip-erase
  --memory-type, nor is valid
  ./AmebaFlash.py  --chip-erase --memory-type nor

> erase user area
  --start-address, start address to erase
  --end-address, is required only for nand flash erase)
  --size, unit: KB
  --memory-type, nor, nand, ram
  ./AmebaFlash.py --erase --start-address 0x08000000 --memory-type nor --end-address 0x08001000 --size 1024
 
log demo:
>>./AmebaFlash.py --download --port COM92 --start-address 0x08000000 --baudrate 1500000 --profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --image Z:\workspace\debug\images\image_dp\image_all.bin --memory-type nor
[2025-11-27 11:04:19.437][I] [main]Flash Version: 1.1.1.0
[2025-11-27 11:04:19.437][I] [main]Device profile: E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev
[2025-11-27 11:04:19.437][I] [main]Serial port: ['COM92']
[2025-11-27 11:04:19.437][I] [main]Baudrate: 1500000
[2025-11-27 11:04:19.462][I] [main]Image info:
[2025-11-27 11:04:19.462][I] [main]> Image: Z:\workspace\debug\images\image_dp\image_all.bin
[2025-11-27 11:04:19.463][I] [main]> StartAddress: 134217728
[2025-11-27 11:04:19.463][I] [main]> EndAddress: 136314880
[2025-11-27 11:04:19.463][I] [main]> MemoryType: 1
[2025-11-27 11:04:19.464][I] [main]> FullErase: False
[2025-11-27 11:04:19.464][I] [main]> Mandatory: True
[2025-11-27 11:04:19.464][I] [main]> Description: image_all.bin
[2025-11-27 11:04:19.464][I] [main]Chip erase: False
[2025-11-27 11:04:19.581][I] [main]Device profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev loaded
[2025-11-27 11:04:19.583][I] [main]Setting path: E:\git_repo\meta_tools\Setting.json
[2025-11-27 11:04:19.690][I] [COM92]Check supported flash size...
[2025-11-27 11:04:20.136][I] [COM92]Current supported flash size <=16MB
[2025-11-27 11:04:20.136][I] [COM92]Image download start...
[2025-11-27 11:04:25.353][I] [COM92]Device info:
[2025-11-27 11:04:25.353][I] [COM92]* DID: 0x6845
[2025-11-27 11:04:25.353][I] [COM92]* ImageType: 0x0001
[2025-11-27 11:04:25.353][I] [COM92]* CmdSetVersion: 2.0
[2025-11-27 11:04:25.353][I] [COM92]* MemoryType: NOR
[2025-11-27 11:04:25.365][I] [COM92]* FlashMID: 0xc8
[2025-11-27 11:04:25.368][I] [COM92]* FlashDID: 0x1765
[2025-11-27 11:04:25.368][I] [COM92]* FlashCapacity: 64Mb/8MB
[2025-11-27 11:04:25.368][I] [COM92]* FlashPageSize: 1024B
[2025-11-27 11:04:25.370][I] [COM92]* WiFiMAC: 00:00:00:00:00:00
[2025-11-27 11:04:25.380][I] [COM92]image_all.bin download...
[2025-11-27 11:04:27.562][I] [COM92]Programming progress: 10
[2025-11-27 11:04:29.521][I] [COM92]Programming progress: 20
[2025-11-27 11:04:31.562][I] [COM92]Programming progress: 30
[2025-11-27 11:04:33.532][I] [COM92]Programming progress: 40
[2025-11-27 11:04:35.273][I] [COM92]Programming progress: 50
[2025-11-27 11:04:36.920][I] [COM92]Programming progress: 60
[2025-11-27 11:04:38.562][I] [COM92]Programming progress: 70
[2025-11-27 11:04:40.216][I] [COM92]Programming progress: 80
[2025-11-27 11:04:41.865][I] [COM92]Programming progress: 90
[2025-11-27 11:04:43.501][I] [COM92]Programming progress: 100
[2025-11-27 11:04:43.504][I] [COM92]image_all.bin download done: 2048KB / 18119.0ms / 925.0Kbps
[2025-11-27 11:04:43.864][I] [COM92]All images download done
[2025-11-27 11:04:43.864][I] [COM92]Finished PASS
[2025-11-27 11:04:43.880][I] [main]All flash threads have completed
[2025-11-27 11:04:43.930][I] [COM92]COM92 closed.

>>./AmebaFlash.py --download --port COM92 --start-address 0x08000000 --baudrate 1500000 --profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --image-dir Z:\workspace\debug\images\image_dp --memory-type nor
[2025-11-27 11:07:08.394][I] [main]Flash Version: 1.1.1.0
[2025-11-27 11:07:08.396][I] [main]Device profile: E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev
[2025-11-27 11:07:08.396][I] [main]Serial port: ['COM92']
[2025-11-27 11:07:08.396][I] [main]Baudrate: 1500000
[2025-11-27 11:07:08.396][I] [main]Image dir: Z:\workspace\debug\images\image_dp
[2025-11-27 11:07:08.396][I] [main]Chip erase: False
[2025-11-27 11:07:08.492][I] [main]Device profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev loaded
[2025-11-27 11:07:08.494][I] [main]Setting path: E:\git_repo\meta_tools\Setting.json
[2025-11-27 11:07:08.597][I] [COM92]Check supported flash size...
[2025-11-27 11:07:13.844][I] [COM92]Current supported flash size <=16MB
[2025-11-27 11:07:13.844][I] [COM92]Image download start...
[2025-11-27 11:07:19.058][I] [COM92]Device info:
[2025-11-27 11:07:19.063][I] [COM92]* DID: 0x6845
[2025-11-27 11:07:19.064][I] [COM92]* ImageType: 0x0001
[2025-11-27 11:07:19.064][I] [COM92]* CmdSetVersion: 2.0
[2025-11-27 11:07:19.064][I] [COM92]* MemoryType: NOR
[2025-11-27 11:07:19.066][I] [COM92]* FlashMID: 0xc8
[2025-11-27 11:07:19.066][I] [COM92]* FlashDID: 0x1765
[2025-11-27 11:07:19.066][I] [COM92]* FlashCapacity: 64Mb/8MB
[2025-11-27 11:07:19.066][I] [COM92]* FlashPageSize: 1024B
[2025-11-27 11:07:19.070][I] [COM92]* WiFiMAC: 00:00:00:00:00:00
[2025-11-27 11:07:19.173][I] [COM92]km4_boot_all.bin download...
[2025-11-27 11:07:19.384][I] [COM92]Programming progress: 12
[2025-11-27 11:07:19.416][I] [COM92]Programming progress: 21
[2025-11-27 11:07:19.487][I] [COM92]Programming progress: 31
[2025-11-27 11:07:19.563][I] [COM92]Programming progress: 40
[2025-11-27 11:07:19.638][I] [COM92]Programming progress: 50
[2025-11-27 11:07:19.714][I] [COM92]Programming progress: 62
[2025-11-27 11:07:19.747][I] [COM92]Programming progress: 71
[2025-11-27 11:07:19.833][I] [COM92]Programming progress: 81
[2025-11-27 11:07:19.896][I] [COM92]Programming progress: 90
[2025-11-27 11:07:19.974][I] [COM92]Programming progress: 100
[2025-11-27 11:07:19.976][I] [COM92]km4_boot_all.bin download done: 32KB / 770.0ms / 340.0Kbps
[2025-11-27 11:07:19.984][I] [COM92]km0_km4_app.bin download...
[2025-11-27 11:07:21.537][I] [COM92]Programming progress: 10
[2025-11-27 11:07:22.350][I] [COM92]Programming progress: 20
[2025-11-27 11:07:23.183][I] [COM92]Programming progress: 30
[2025-11-27 11:07:23.973][I] [COM92]Programming progress: 40
[2025-11-27 11:07:24.844][I] [COM92]Programming progress: 50
[2025-11-27 11:07:25.632][I] [COM92]Programming progress: 60
[2025-11-27 11:07:26.439][I] [COM92]Programming progress: 70
[2025-11-27 11:07:27.341][I] [COM92]Programming progress: 80
[2025-11-27 11:07:28.142][I] [COM92]Programming progress: 90
[2025-11-27 11:07:28.970][I] [COM92]Programming progress: 100
[2025-11-27 11:07:28.971][I] [COM92]km0_km4_app.bin download done: 824KB / 8967.0ms / 752.0Kbps
[2025-11-27 11:07:29.115][I] [COM92]All images download done
[2025-11-27 11:07:29.115][I] [COM92]Finished PASS
[2025-11-27 11:07:29.115][I] [main]All flash threads have completed
[2025-11-27 11:07:29.169][I] [COM92]COM92 closed.


>>./AmebaFlash.py --erase --port COM92 --start-address 0x08000000 --baudrate 1500000 --profile E:\git_repo\meta_tools\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --size 1024 --memory-type nor
[2025-11-27 11:16:05.208][I] [main]Flash Version: 1.1.1.0
[2025-11-27 11:16:05.208][I] [main]Device profile: E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev
[2025-11-27 11:16:05.208][I] [main]Serial port: ['COM92']
[2025-11-27 11:16:05.208][I] [main]Baudrate: 1500000
[2025-11-27 11:16:05.208][I] [main]Chip erase: False
[2025-11-27 11:16:05.304][I] [main]Device profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev loaded
[2025-11-27 11:16:05.304][I] [main]Setting path: E:\git_repo\meta_tools\Setting.json
[2025-11-27 11:16:10.650][I] [COM92]Device info:
[2025-11-27 11:16:10.650][I] [COM92]* DID: 0x6845
[2025-11-27 11:16:10.650][I] [COM92]* ImageType: 0x0001
[2025-11-27 11:16:10.655][I] [COM92]* CmdSetVersion: 2.0
[2025-11-27 11:16:10.655][I] [COM92]* MemoryType: NOR
[2025-11-27 11:16:10.655][I] [COM92]* FlashMID: 0xc8
[2025-11-27 11:16:10.655][I] [COM92]* FlashDID: 0x1765
[2025-11-27 11:16:10.655][I] [COM92]* FlashCapacity: 64Mb/8MB
[2025-11-27 11:16:10.655][I] [COM92]* FlashPageSize: 1024B
[2025-11-27 11:16:10.667][I] [COM92]* WiFiMAC: 00:00:00:00:00:00
[2025-11-27 11:16:10.671][I] [COM92]NOR flash erase: start address=0x8000000, size=1024KB.
[2025-11-27 11:16:10.725][I] [COM92]Erase nor done
[2025-11-27 11:16:10.726][I] [COM92]Finished PASS
[2025-11-27 11:16:10.727][I] [main]All flash threads have completed
[2025-11-27 11:16:10.777][I] [COM92]COM92 closed.


>>./AmebaFlash.py --chip-erase --port COM92 --baudrate 1500000 --profile E:\git_repo\meta_tools\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --memory-type nor
[2025-11-27 11:19:12.035][I] [main]Flash Version: 1.1.1.0
[2025-11-27 11:19:12.035][I] [main]Device profile: E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev
[2025-11-27 11:19:12.035][I] [main]Serial port: ['COM92']
[2025-11-27 11:19:12.035][I] [main]Baudrate: 1500000
[2025-11-27 11:19:12.035][I] [main]Chip erase: True
[2025-11-27 11:19:12.125][I] [main]Device profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev loaded
[2025-11-27 11:19:12.125][I] [main]Setting path: E:\git_repo\meta_tools\Setting.json
[2025-11-27 11:19:17.435][I] [COM92]Device info:
[2025-11-27 11:19:17.436][I] [COM92]* DID: 0x6845
[2025-11-27 11:19:17.436][I] [COM92]* ImageType: 0x0001
[2025-11-27 11:19:17.440][I] [COM92]* CmdSetVersion: 2.0
[2025-11-27 11:19:17.441][I] [COM92]* MemoryType: NOR
[2025-11-27 11:19:17.442][I] [COM92]* FlashMID: 0xc8
[2025-11-27 11:19:17.442][I] [COM92]* FlashDID: 0x1765
[2025-11-27 11:19:17.442][I] [COM92]* FlashCapacity: 64Mb/8MB
[2025-11-27 11:19:17.443][I] [COM92]* FlashPageSize: 1024B
[2025-11-27 11:19:17.445][I] [COM92]* WiFiMAC: 00:00:00:00:00:00
[2025-11-27 11:19:17.448][I] [COM92]Chip erase start
[2025-11-27 11:19:21.423][I] [COM92]Finished PASS
[2025-11-27 11:19:21.426][I] [main]All flash threads have completed
[2025-11-27 11:19:21.480][I] [COM92]COM92 closed.
=======
<Usage of AmebaFlash.py for Ameba SoCs>

AmebaFlash.py options:
  -h, --help            show this help message and exit
  -d DOWNLOAD, --download DOWNLOAD
                        download images
  -p PORT [PORT ...], --port PORT [PORT ...]  
                        serial port
  -b BAUDRATE, --baudrate BAUDRATE
                        serial port baud rate
  -f PROFILE, --profile PROFILE
                        device profile
  -i IMAGE, --image IMAGE
                        single image
  -r IMAGE-DIR, --image-dir IMAGE-DIR
                        image directory
  -e ERASE, --erase ERASE
                        erase flash
  -m {nor,nand,ram}, --memory-type {nor,nand,ram}, default is nor,
                        specified memory type
  -a START_ADDRESS, --start-address START_ADDRESS
                        start address, hex
  -n END_ADDRESS, --end-address END_ADDRESS
                        end address, hex
  -z SIZE, --size SIZE  size in KB
  -o LOG_FILE, --log-file LOG_FILE
                        set log file with path
  -v, --version         show program's version number and exit
  
  --chip-erase CHIP_ERASE
                        chip erase flag option download
  --layout-list LAYOUT_LIST
                        layout info, list
  --log-level LOG_LEVEL
                        set log level						

command e.g.:
> download single image
  --download
  --start-address, start address to download
  --profile, device profile
  --image, image file
  --port, serial port
  --baudrate, download baudrate
  ./AmebaFlash.py --download --profile E:\git_repo\meta_tools\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --start-address 0x08000000 --image "D:\Images\image_dplus\white space\image_all.bin"  --port COM92 --baudrate 1500000  

> download images with profile layout
  --download
  --start-address, start address to download
  --profile, device profile
  --image-dir, image path where store images and image name should be match with ImageName defined in device profile
  --port, serial port
  --baudrate, download baudrate
  ./AmebaFlash.py --download --profile E:\git_repo\meta_tools\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --image-dir "D:\Images\image_dplus\white space" --port COM92 --baudrate 1500000 
 
> chip erase
  erase full chip, only useful for nor flash
  --chip-erase
  --memory-type, nor is valid
  ./AmebaFlash.py  --chip-erase --memory-type nor

> erase user area
  --start-address, start address to erase
  --end-address, is required only for nand flash erase)
  --size, unit: KB
  --memory-type, nor, nand, ram
  ./AmebaFlash.py --erase --start-address 0x08000000 --memory-type nor --end-address 0x08001000 --size 1024
 
> read wifi mac
  --read-wifimac
  --profile, device profile
  --baudrate, read baudrate
  --memory-type, memory type
  --port, serial port
  
 
log demo:
>>./AmebaFlash.py --download --port COM92 --start-address 0x08000000 --baudrate 1500000 --profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --image Z:\workspace\debug\images\image_dp\image_all.bin --memory-type nor
[2025-11-27 11:04:19.437][I] [main]Flash Version: 1.1.1.0
[2025-11-27 11:04:19.437][I] [main]Device profile: E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev
[2025-11-27 11:04:19.437][I] [main]Serial port: ['COM92']
[2025-11-27 11:04:19.437][I] [main]Baudrate: 1500000
[2025-11-27 11:04:19.462][I] [main]Image info:
[2025-11-27 11:04:19.462][I] [main]> Image: Z:\workspace\debug\images\image_dp\image_all.bin
[2025-11-27 11:04:19.463][I] [main]> StartAddress: 134217728
[2025-11-27 11:04:19.463][I] [main]> EndAddress: 136314880
[2025-11-27 11:04:19.463][I] [main]> MemoryType: 1
[2025-11-27 11:04:19.464][I] [main]> FullErase: False
[2025-11-27 11:04:19.464][I] [main]> Mandatory: True
[2025-11-27 11:04:19.464][I] [main]> Description: image_all.bin
[2025-11-27 11:04:19.464][I] [main]Chip erase: False
[2025-11-27 11:04:19.581][I] [main]Device profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev loaded
[2025-11-27 11:04:19.583][I] [main]Setting path: E:\git_repo\meta_tools\Setting.json
[2025-11-27 11:04:19.690][I] [COM92]Check supported flash size...
[2025-11-27 11:04:20.136][I] [COM92]Current supported flash size <=16MB
[2025-11-27 11:04:20.136][I] [COM92]Image download start...
[2025-11-27 11:04:25.353][I] [COM92]Device info:
[2025-11-27 11:04:25.353][I] [COM92]* DID: 0x6845
[2025-11-27 11:04:25.353][I] [COM92]* ImageType: 0x0001
[2025-11-27 11:04:25.353][I] [COM92]* CmdSetVersion: 2.0
[2025-11-27 11:04:25.353][I] [COM92]* MemoryType: NOR
[2025-11-27 11:04:25.365][I] [COM92]* FlashMID: 0xc8
[2025-11-27 11:04:25.368][I] [COM92]* FlashDID: 0x1765
[2025-11-27 11:04:25.368][I] [COM92]* FlashCapacity: 64Mb/8MB
[2025-11-27 11:04:25.368][I] [COM92]* FlashPageSize: 1024B
[2025-11-27 11:04:25.370][I] [COM92]* WiFiMAC: 00:00:00:00:00:00
[2025-11-27 11:04:25.380][I] [COM92]image_all.bin download...
[2025-11-27 11:04:27.562][I] [COM92]Programming progress: 10
[2025-11-27 11:04:29.521][I] [COM92]Programming progress: 20
[2025-11-27 11:04:31.562][I] [COM92]Programming progress: 30
[2025-11-27 11:04:33.532][I] [COM92]Programming progress: 40
[2025-11-27 11:04:35.273][I] [COM92]Programming progress: 50
[2025-11-27 11:04:36.920][I] [COM92]Programming progress: 60
[2025-11-27 11:04:38.562][I] [COM92]Programming progress: 70
[2025-11-27 11:04:40.216][I] [COM92]Programming progress: 80
[2025-11-27 11:04:41.865][I] [COM92]Programming progress: 90
[2025-11-27 11:04:43.501][I] [COM92]Programming progress: 100
[2025-11-27 11:04:43.504][I] [COM92]image_all.bin download done: 2048KB / 18119.0ms / 925.0Kbps
[2025-11-27 11:04:43.864][I] [COM92]All images download done
[2025-11-27 11:04:43.864][I] [COM92]Finished PASS
[2025-11-27 11:04:43.880][I] [main]All flash threads have completed
[2025-11-27 11:04:43.930][I] [COM92]COM92 closed.

>>./AmebaFlash.py --download --port COM92 --start-address 0x08000000 --baudrate 1500000 --profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --image-dir Z:\workspace\debug\images\image_dp --memory-type nor
[2025-11-27 11:07:08.394][I] [main]Flash Version: 1.1.1.0
[2025-11-27 11:07:08.396][I] [main]Device profile: E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev
[2025-11-27 11:07:08.396][I] [main]Serial port: ['COM92']
[2025-11-27 11:07:08.396][I] [main]Baudrate: 1500000
[2025-11-27 11:07:08.396][I] [main]Image dir: Z:\workspace\debug\images\image_dp
[2025-11-27 11:07:08.396][I] [main]Chip erase: False
[2025-11-27 11:07:08.492][I] [main]Device profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev loaded
[2025-11-27 11:07:08.494][I] [main]Setting path: E:\git_repo\meta_tools\Setting.json
[2025-11-27 11:07:08.597][I] [COM92]Check supported flash size...
[2025-11-27 11:07:13.844][I] [COM92]Current supported flash size <=16MB
[2025-11-27 11:07:13.844][I] [COM92]Image download start...
[2025-11-27 11:07:19.058][I] [COM92]Device info:
[2025-11-27 11:07:19.063][I] [COM92]* DID: 0x6845
[2025-11-27 11:07:19.064][I] [COM92]* ImageType: 0x0001
[2025-11-27 11:07:19.064][I] [COM92]* CmdSetVersion: 2.0
[2025-11-27 11:07:19.064][I] [COM92]* MemoryType: NOR
[2025-11-27 11:07:19.066][I] [COM92]* FlashMID: 0xc8
[2025-11-27 11:07:19.066][I] [COM92]* FlashDID: 0x1765
[2025-11-27 11:07:19.066][I] [COM92]* FlashCapacity: 64Mb/8MB
[2025-11-27 11:07:19.066][I] [COM92]* FlashPageSize: 1024B
[2025-11-27 11:07:19.070][I] [COM92]* WiFiMAC: 00:00:00:00:00:00
[2025-11-27 11:07:19.173][I] [COM92]km4_boot_all.bin download...
[2025-11-27 11:07:19.384][I] [COM92]Programming progress: 12
[2025-11-27 11:07:19.416][I] [COM92]Programming progress: 21
[2025-11-27 11:07:19.487][I] [COM92]Programming progress: 31
[2025-11-27 11:07:19.563][I] [COM92]Programming progress: 40
[2025-11-27 11:07:19.638][I] [COM92]Programming progress: 50
[2025-11-27 11:07:19.714][I] [COM92]Programming progress: 62
[2025-11-27 11:07:19.747][I] [COM92]Programming progress: 71
[2025-11-27 11:07:19.833][I] [COM92]Programming progress: 81
[2025-11-27 11:07:19.896][I] [COM92]Programming progress: 90
[2025-11-27 11:07:19.974][I] [COM92]Programming progress: 100
[2025-11-27 11:07:19.976][I] [COM92]km4_boot_all.bin download done: 32KB / 770.0ms / 340.0Kbps
[2025-11-27 11:07:19.984][I] [COM92]km0_km4_app.bin download...
[2025-11-27 11:07:21.537][I] [COM92]Programming progress: 10
[2025-11-27 11:07:22.350][I] [COM92]Programming progress: 20
[2025-11-27 11:07:23.183][I] [COM92]Programming progress: 30
[2025-11-27 11:07:23.973][I] [COM92]Programming progress: 40
[2025-11-27 11:07:24.844][I] [COM92]Programming progress: 50
[2025-11-27 11:07:25.632][I] [COM92]Programming progress: 60
[2025-11-27 11:07:26.439][I] [COM92]Programming progress: 70
[2025-11-27 11:07:27.341][I] [COM92]Programming progress: 80
[2025-11-27 11:07:28.142][I] [COM92]Programming progress: 90
[2025-11-27 11:07:28.970][I] [COM92]Programming progress: 100
[2025-11-27 11:07:28.971][I] [COM92]km0_km4_app.bin download done: 824KB / 8967.0ms / 752.0Kbps
[2025-11-27 11:07:29.115][I] [COM92]All images download done
[2025-11-27 11:07:29.115][I] [COM92]Finished PASS
[2025-11-27 11:07:29.115][I] [main]All flash threads have completed
[2025-11-27 11:07:29.169][I] [COM92]COM92 closed.


>>./AmebaFlash.py --erase --port COM92 --start-address 0x08000000 --baudrate 1500000 --profile E:\git_repo\meta_tools\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --size 1024 --memory-type nor
[2025-11-27 11:16:05.208][I] [main]Flash Version: 1.1.1.0
[2025-11-27 11:16:05.208][I] [main]Device profile: E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev
[2025-11-27 11:16:05.208][I] [main]Serial port: ['COM92']
[2025-11-27 11:16:05.208][I] [main]Baudrate: 1500000
[2025-11-27 11:16:05.208][I] [main]Chip erase: False
[2025-11-27 11:16:05.304][I] [main]Device profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev loaded
[2025-11-27 11:16:05.304][I] [main]Setting path: E:\git_repo\meta_tools\Setting.json
[2025-11-27 11:16:10.650][I] [COM92]Device info:
[2025-11-27 11:16:10.650][I] [COM92]* DID: 0x6845
[2025-11-27 11:16:10.650][I] [COM92]* ImageType: 0x0001
[2025-11-27 11:16:10.655][I] [COM92]* CmdSetVersion: 2.0
[2025-11-27 11:16:10.655][I] [COM92]* MemoryType: NOR
[2025-11-27 11:16:10.655][I] [COM92]* FlashMID: 0xc8
[2025-11-27 11:16:10.655][I] [COM92]* FlashDID: 0x1765
[2025-11-27 11:16:10.655][I] [COM92]* FlashCapacity: 64Mb/8MB
[2025-11-27 11:16:10.655][I] [COM92]* FlashPageSize: 1024B
[2025-11-27 11:16:10.667][I] [COM92]* WiFiMAC: 00:00:00:00:00:00
[2025-11-27 11:16:10.671][I] [COM92]NOR flash erase: start address=0x8000000, size=1024KB.
[2025-11-27 11:16:10.725][I] [COM92]Erase nor done
[2025-11-27 11:16:10.726][I] [COM92]Finished PASS
[2025-11-27 11:16:10.727][I] [main]All flash threads have completed
[2025-11-27 11:16:10.777][I] [COM92]COM92 closed.


>>./AmebaFlash.py --chip-erase --port COM92 --baudrate 1500000 --profile E:\git_repo\meta_tools\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --memory-type nor
[2025-11-27 11:19:12.035][I] [main]Flash Version: 1.1.1.0
[2025-11-27 11:19:12.035][I] [main]Device profile: E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev
[2025-11-27 11:19:12.035][I] [main]Serial port: ['COM92']
[2025-11-27 11:19:12.035][I] [main]Baudrate: 1500000
[2025-11-27 11:19:12.035][I] [main]Chip erase: True
[2025-11-27 11:19:12.125][I] [main]Device profile E:\git_repo\meta_tools\devices\Profiles\AmebaDplus_FreeRTOS_NOR.rdev loaded
[2025-11-27 11:19:12.125][I] [main]Setting path: E:\git_repo\meta_tools\Setting.json
[2025-11-27 11:19:17.435][I] [COM92]Device info:
[2025-11-27 11:19:17.436][I] [COM92]* DID: 0x6845
[2025-11-27 11:19:17.436][I] [COM92]* ImageType: 0x0001
[2025-11-27 11:19:17.440][I] [COM92]* CmdSetVersion: 2.0
[2025-11-27 11:19:17.441][I] [COM92]* MemoryType: NOR
[2025-11-27 11:19:17.442][I] [COM92]* FlashMID: 0xc8
[2025-11-27 11:19:17.442][I] [COM92]* FlashDID: 0x1765
[2025-11-27 11:19:17.442][I] [COM92]* FlashCapacity: 64Mb/8MB
[2025-11-27 11:19:17.443][I] [COM92]* FlashPageSize: 1024B
[2025-11-27 11:19:17.445][I] [COM92]* WiFiMAC: 00:00:00:00:00:00
[2025-11-27 11:19:17.448][I] [COM92]Chip erase start
[2025-11-27 11:19:21.423][I] [COM92]Finished PASS
[2025-11-27 11:19:21.426][I] [main]All flash threads have completed
[2025-11-27 11:19:21.480][I] [COM92]COM92 closed.

>>./AmebaFlash.py --read-wifimac --baudrate 1500000 --profile E:\git_repo\meta_tools\Profiles\AmebaDplus_FreeRTOS_NOR.rdev --memory-type nor --port COM7
[2025-12-12 10:16:11.191][I] [main]AmebaFlash Version: 1.1.1.3
[2025-12-12 10:16:11.191][I] [main]Device profile: Devices\Profiles\AmebaSmart_FreeRTOS_NOR.rdev
[2025-12-12 10:16:11.191][I] [main]Serial port: ['COM7']
[2025-12-12 10:16:11.191][I] [main]Baudrate: 1500000
[2025-12-12 10:16:11.260][I] [main]Device profile Devices\Profiles\AmebaSmart_FreeRTOS_NOR.rdev loaded
[2025-12-12 10:16:11.260][I] [main]Settings path: E:\git_repo\meta_tools\flash_v1.1.1.3_beta\Settings.json
[2025-12-12 10:16:12.339][I] [COM7]WiFiMAC: 00:00:00:00:00:00
[2025-12-12 10:16:12.341][I] [COM7]close COM7...
[2025-12-12 10:16:12.341][I] [COM7]COM7 closed.
[2025-12-12 10:16:12.343][I] [COM7]Finished PASS
[2025-12-12 10:16:12.343][I] [main]All flash threads have completed
>>>>>>> CHANGE (28cfd3 [AmebaFlash]update to v1.1.1.3)
