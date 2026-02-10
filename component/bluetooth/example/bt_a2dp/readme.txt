##################################################################################
#                                                                                #
#                                    A2DP demo                                   #
#                                                                                #
##################################################################################

GCC menuconfig 
~~~~~~~~~~~
1. BT Related:
    ./menuconfig.py --> CONFIG BT --> BT Example Demo --> BT A2DP
2. Audio Related:
    ./menuconfig.py --> CONFIG Application --> Audio Config --> Select Audio Interfaces (Mixer)
    ./menuconfig.py --> CONFIG Application --> Audio Config --> Third Party Lib --> Speex
3. GCC : use CMD "./build.py" to compile example

Test ATCMD
~~~~~~~~~~~
1. A2DP Source Demo
    1.1 enable: AT+BTDEMO=a2dp,src,1
    1.2 connect: AT+BTA2DP=conn,<bd_addr> //connect a2dp sink from a2dp source
    1.3 start audio transmission: AT+BTA2DP=start,<bd_addr>
2. A2DP Sink Demo
    2.1 enable: AT+BTDEMO=a2dp,snk,1
    2.2 connect:AT+BTA2DP=conn,<bd_addr> //connect a2dp source from a2dp sink
3. AVRCP Control from A2DP sink
    3.1 play: AT+BTAVRCP=play,<bd_addr> //a2dp source bd_addr
    3.2 pause: AT+BTAVRCP=pause,<bd_addr> //a2dp source bd_addr
    3.3 forward: AT+BTAVRCP=forward,<bd_addr> //a2dp source bd_addr
    3.4 backward: AT+BTAVRCP=backward,<bd_addr> //a2dp source bd_addr
    3.5 volume change req: AT+BTAVRCP=volume_change_req,<bd_addr>,volume //a2dp source bd_addr, volume from 0x00 to 0x7f