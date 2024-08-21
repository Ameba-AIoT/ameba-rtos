# CUSTOMER IMPLEMENTATION SPEEXDSP EXAMPLE

1. **Enable playback configurations**:
    * For player running in KR4:
        cd `amebalite_gcc_project`: `make menuconfig`
        < MENUCONFIG FOR KR4 CONFIG  --->
        [*] Enable Audio Framework
        (Mixer) Select Audio Interfaces
        < Audio Config  --->
        [*] Enable Media Framework,
        Demux  ---> choose the extractors,
        Codec  ---> choose the codecs.

    * For player running in KM4:
       cd `amebalite_gcc_project`: `make menuconfig`
        < MENUCONFIG FOR KM4 CONFIG  --->
        [*] Enable Audio Framework
        (Mixer) Select Audio Interfaces
        < Audio Config  --->
        [*] Enable Media Framework,
        Demux  ---> choose the extractors,
        Codec  ---> choose the codecs.

    The choices of Demux and Codec are as follows:
    support audio format : WAV (Demux WAV + Codec PCM)
    support audio format : MP3 (Demux MP3 + Codec MP3)
    support audio format : AAC (Demux AAC + Codec AAC)
    support audio format : MP4 or M4A (Demux M4A + Codec AAC)
    support audio format : FLAC (Demux FLAC)
    support audio format : OGG or OPUS (Demux OGG + Codec OPUS)

