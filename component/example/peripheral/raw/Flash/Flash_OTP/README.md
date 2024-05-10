# Example Description

This example will erase a security register, write security register in `USER mode` and verify result by comparing transmit data with receive data.
If you wnat to test security register OTP function, set "#if 0" to "#if 1" and the example will lock the corresponding bit of security register,  erase the security register, write security register in `USER mode` and verify OTP result by comparing the receive data before OTP with the receive data after OTP.

This example does not support the loguart command response.

# HW Configuration

None

# SW Configuration

Replace the original main.c under sdk project (source\amebaxxx_gcc_project\project_core\src) with the main.c in the example directory. Then re-build project. 

For AmebaSmart, please compile it in CA32(ap) core. 

# Expected Result

ReceiveData[] is:
xx xx xx xx xx xx xx xx 
 ########Test security registers before OTP finished########

# Note

* User should set flash test start address (`Address`) carefully to avoid the address is not in security register address range.
* Once a lock bit is set to 1, the corresponding security register will be permanently locked.

# Supported IC

* AMEBASMART
* AMEBALITE
* W25Q256jv Nor Flash and GD25B64C Nor FLASh have been tested, other Flash with the same operation commands and flow also can be supported.