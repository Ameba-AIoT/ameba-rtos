# Example Description

1. In this case, use hardware ECDSA for signature and verification 
   otp_key : 0 for not use OTP key while 1 for key 0 and 2 for key 1.

This example does not support the loguart command response.

# HW Configuration
	none

# SW Configuration

1. How to compile :
   	(1) copy main.c under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx

   ​	(2) make and download new image.

# Expected Result

1. The code will check the correctness of encryption, decryption. 
   	If it is correct, it will print success, and if it is incorrect, it will print failed.

# Note

none

# Supported IC

* AmebaSmart
* AmebaLite