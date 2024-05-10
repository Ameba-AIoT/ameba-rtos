# PSA ADAC Scenarios

| Scenario ID | Section    | Description                                                                                                                                                                                                                            | Test Cases |
|-------------|------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------|
| S_ADAC_001  | Commands   | Verify that the following commands are recognized by the target and do not return SDP_INVALID_COMMAND status.<br />1. Discovery<br/>2. Authentication Start<br/>3. Authentication Response<br/>4. Close Session<br/>5. Lock Debug<br/> | test_a002 <br/>  |
| S_ADAC_002  | Commands   | Send two authentication-start commands back-to-back and verify that the challenge vector obtained in the response is different. | test_a001 <br/>  |
| S_ADAC_003  | Commands   | Use discovery command to query the target for supported response fragment formats. Check the following constraints for the response: <br/>1. token_formats (Type ID 0x0100) TLV enlists token_adac (Type ID 0x200)<br/>2. cert_formats (Type ID 0x0101) TLV enlists cert_adac (Type ID 0x201)<br/>3. psa_lifecyle TLV indicates only one state.<br/>| test_a003 <br/>  |
| S_ADAC_004  | Commands   | Send discovery command with list of specified TLV entries, both in increasing and decreasing order. Verify that the target responds with SDP_SUCCESS status. | test_a004 <br/>  |
| S_ADAC_005  | Commands   | Send invalid response fragment as part of Authentication response command sequence and check that the target responds with SDP_FAILURE status.| test_a005 <br/>  |
| S_ADAC_006  | Commands   | Request two challenge sequences back-to-back. Send the ADAC token signed with the older challenge and verify that the target rejects the token by responding with SDP_FAILURE. Send the ADAC token with the latest issued challenge and check that the target rejects the updated token as well by responding with SDP_FAILURE status. | test_a005 <br/>  |
| S_ADAC_007  | Commands   | Send an authentication response command with a NULL payload and verify that the target responds with SDP_FAILURE status. | test_a006 <br/>  |
| S_ADAC_008  | Commands   | Verify that the target responds to an unrecognized ADAC command with SDP_INVALID_COMMAND status. | test_a007 <br/>  |
| S_ADAC_009  | Commands   | Verify that for a multiple fragment Authentication response request, the device responds with a SDP_MORE_DATA_NEEDED status code after each response fragment. | test_a001, test_a005 <br/>  |
| S_ADAC_010  | Commands   | Check that the Close Session command always returns an SDP_SUCCESS status. | test_a001 <br/>  |
| S_ADAC_011  | Constraints| In a certificate chain, provide a leaf certificate before a root certificate and verify that the target responds with an error. | test_a008 <br/>  |
| S_ADAC_012  | Constraints| Check that a certificate is rejected due to each of the following scope-limiting feature:<br/>1. Role<br/>2. Lifecycle<br/>| test_a008 <br/>  |
| S_ADAC_013  | Constraints| Check that if two or more distinct non-neutral values are present in the certificate chain, a failure response is obtained.| test_a009 <br/>  |
| S_ADAC_014  | Constraints| Request a permission value (perm_req) via authentication token which is denied due to a restrictive perm_mask field. Check that the authentication response is successful.| test_a002 <br/>  |
| S_ADAC_015  | Constraints| Request access to software partition with no software partition ID listed in the certificate chain. Check that success status is reported.| test_a010 <br/>  |
| S_ADAC_016  | Constraints| Verify that the reserved fields of TLV sequences read as 0 for  ADAC version 1.0 implementation.| test_a011 <br/>  |

## Note

1. The list of identified scenarios is not exhaustive and is subject to change.<br/>
2. Some of the listed scenarios could be descoped or classified as testing limitation.<br />

# License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.*
