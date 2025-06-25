# CUSTOMER IMPLEMENTATION SPEEXDSP EXAMPLE

1. cd <sdk>/source/<board>_gcc_project

2. Configure menuconfig

3.1 For AmebaLite and AmebaDPlus
< MENUCONFIG FOR KM4 CONFIG  --->
3.2 For AmebaSmart
< MENUCONFIG FOR CA32 CONFIG  --->

4.
    < Audio Config  --->
        Third Party Lib  --->
            [*] Speex

5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.



