# Example Description

This example describes how to calculate SNR and THD+N of the data recorded by DMIC. 

# HW Configuration

    - Place speaker and microphone 10cm apart. 

# SW Configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Speaker plays a 1KHz sine wave, which is received by DMIC.

2. SNR and THD+N will be printed on the serial monitor:
    - `AUDIO SNR TEST PASS` if it exceeds the threshold
    - `AUDIO SNR TEST fAIL` if it does not exceed

# Note

1. The threshold of SNR and THD+N needs to be determined during testing. By default, threshold of SNR is set as "90", threshold of total harmonic distortion is set as "70":
    - "#define IS_SNR_NOT_90DB(x)	((x) < 90)" 
    - "#define IS_SNR_NOT_70DB(x)	((x) < 70)"

2. AmebaSmart only supports hp core and must enable Audio Framework.

3. Depending on the amplifier, AmebaDplus has different packages, and the DMIC's pins are different under different packages. AmebaDplus uses HT513 amplifier by default, if you want to use ALC1019, please change macro in main.c.

4. If you want to play and record at the same time, you need to use it together with the speaker thread. Make sure that the speaker thread has a higher priority than the recording thread. If you use it on Amebalite, please enable fix_bclk in two threads.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
