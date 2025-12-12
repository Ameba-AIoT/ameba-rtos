# Example Description

1. In this case, sha256 hmac will be used to calculate the digital of the message. 
   If the OTPkey is greater than 1, the engine uses the key passed in by the software. 
   When OTPkey is less than or equal to 1, the crypto engine will load the OTP key from efuse.
   To prevent errors caused by multi-core access to crypto simultaneously, a lock is added before CRYPTO_OTPKey_Init, and the lock is released after crypto calculation.

# HW Configuration

1. If you have programmed OTP physical mapping, the corresponding relationship of keynum is as follows, for specific instructions, please refer to the first section of the AN file in the crypto engine.
   		SHA HAMC	Secure				0	S_IPSEC_Key1
   										1	S_IPSEC_Key2
   					Non Secure			0	NS_IPSEC_Key1
   										1	NS_IPSEC_Key2

# SW Configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. The code will check the correctness of encryption, decryption. 
   	If it is correct, it will print success, and if it is incorrect, it will print failed and print incorrect results.

# Note

none

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
