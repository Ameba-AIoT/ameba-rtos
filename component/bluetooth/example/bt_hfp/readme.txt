##################################################################################
#                                                                                #
#                                    HFP demo                                    #
#                                                                                #
##################################################################################

GCC menuconfig 
~~~~~~~~~~~
1. BT Related:
    ./menuconfig.py --> CONFIG BT --> BT Example Demo --> BT HFP
2. Audio Related:
    ./menuconfig.py --> CONFIG Application --> Audio Config --> Select Audio Interfaces (Mixer)
3. GCC : use CMD "./build.py" to compile example

Test ATCMD
~~~~~~~~~~~
1. HFP AG Demo
    1.1 enable: AT+BTDEMO=hfp,ag,1
    1.2 connect: AT+BTHFP=conn,<bd_addr> //connect hfp hf from ag
    1.3 call income: AT+BTHFP=call_income,<bd_addr>,<phone_number>,<call_type> //AT+BTHFP=call_income,00e04c8007e6,10086,0
2. HFP HF Demo
    2.1 enable: AT+BTDEMO=hfp,hf,1
    2.2 connect: AT+BTHFP=conn,<bd_addr> //connect hfp ag from hf
    2.3 call answer: AT+BTHFP=call_answer,<bd_addr>
    2.4 call terminate: AT+BTHFP=call_terminate,<bd_addr>