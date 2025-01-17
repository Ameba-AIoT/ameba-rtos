# Example Description

1. In this case, use hardware ECDSA for signature and verification 
   otp_key : 0 for not use OTP key while 1 for key 0 and 2 for key 1.

This example does not support the loguart command response.

# HW Configuration
	none

# SW Configuration

1. How to compile :
   	perform cmd: './build.py -a raw_ecdsa_test' in directory {sdk_path}/amebaXXX_gcc_project

# Expected Result

1. The code will check the correctness of encryption, decryption. 
   	If it is correct, it will print success, and if it is incorrect, it will print failed.

# Note

none

# Supported IC

* AmebaSmart
* AmebaLite