# Example Description

1. In this case, sha256 hmac will be used to calculate the digital of the message. 
   If the OTPkey is greater than 1, the engine uses the key passed in by the software. 
   When OTPkey is less than or equal to 1, the crypto engine will load the OTP key from efuse.
   To prevent errors caused by multi-core access to crypto simultaneously, a lock is added before CRYPTO_OTPKey_Init, and the lock is released after crypto calculation.

This example does not support the loguart command response.

# HW Configuration

1. If you have programmed OTP physical mapping, the corresponding relationship of keynum is as follows, for specific instructions, please refer to the first section of the AN file in the crypto engine.
   		SHA HAMC	Secure				0	S_IPSEC_Key1
   										1	S_IPSEC_Key2
   					Non Secure			0	NS_IPSEC_Key1
   										1	NS_IPSEC_Key2

# SW Configuration

1. How to compile :
   	perform cmd: './build.py -a raw_crypto_hmac' in directory {sdk_path}/amebaXXX_gcc_project

# Expected Result

1. The code will check the correctness of encryption, decryption. 
   	If it is correct, it will print success, and if it is incorrect, it will print failed and print incorrect results.

# Note

none

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus