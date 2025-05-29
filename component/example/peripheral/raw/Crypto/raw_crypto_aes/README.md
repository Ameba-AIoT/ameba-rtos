# Example Description

1. In this case, AES ECB will be used to calculate the encryption and decryption of 32 bytes plaintext.
   When the keynum parameter is greater than 3, use the key passed in by the software.
   When the keynum is less than or equal to 3, crypto will load the otp key from the efuse.
   To prevent errors caused by multi-core access to crypto simultaneously, a lock is added before CRYPTO_OTPKey_Init, and the lock is released after rtl_crypto_aes_xxx_encrypt or rtl_crypto_aes_xxx_decrypt.

# HW Configuration

1. If you have programmed OTP physical mapping, the corresponding relationship of keynum is as follows, for specific instructions, please refer to the first section of the AN file in the crypto engine.
   		AES			Secure			0	S_IPSEC_Key1
   									1	S_IPSEC_Key2
   									2	RSIP_AES_key1
   									3	RSIP_AES_key2
   					Non Secure		0	NS_IPSEC_Key1
   									1	NS_IPSEC_Key2
   									2	RSIP_AES_key1
   									3	RSIP_AES_key2

# SW Configuration

1. How to compile :
   	perform cmd: './build.py -a raw_crypto_aes' in directory {sdk_path}/amebaXXX_gcc_project

# Expected Result

1. The code will check the correctness of encryption, decryption. 
   	If it is correct, it will print success, and if it is incorrect, it will print failed and print incorrect results.

# Note

none

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
