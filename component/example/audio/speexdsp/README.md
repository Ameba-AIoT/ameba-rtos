# CUSTOMER IMPLEMENTATION SPEEXDSP EXAMPLE

1. cd <sdk>/source/<board>_gcc_project

2. make menuconfig

3.1 For AmebaLite and AmebaDPlus
< MENUCONFIG FOR KM4 CONFIG  --->
3.2 For AmebaSmart
< MENUCONFIG FOR CA32 CONFIG  --->

4.
    < Audio Config  --->
        Third Party Lib  --->
            [*] Speex

5. make EXAMPLE=speexdsp



