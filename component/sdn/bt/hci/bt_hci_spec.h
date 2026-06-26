#ifndef __BT_HCI_SPEC_H__
#define __BT_HCI_SPEC_H__



#define BT_HCI_VERSION_1_0                      0
#define BT_HCI_VERSION_1_1                      1
#define BT_HCI_VERSION_1_2                      2
#define BT_HCI_VERSION_2_0                      3
#define BT_HCI_VERSION_2_1                      4
#define BT_HCI_VERSION_3_0                      5
#define BT_HCI_VERSION_4_0                      6
#define BT_HCI_VERSION_4_1                      7
#define BT_HCI_VERSION_4_2                      8
#define BT_HCI_VERSION_5_0                      9
#define BT_HCI_VERSION_5_1                      10
#define BT_HCI_VERSION_5_2                      11
#define BT_HCI_VERSION_5_3                      12
#define BT_HCI_VERSION_5_4                      13
#define BT_HCI_VERSION_6_0                      14

#define BT_HCI_H4_NONE                                          0x00    /* None of the known packet types */
#define BT_HCI_H4_CMD                                           0x01    /* HCI Command packet */
#define BT_HCI_H4_ACL                                           0x02    /* HCI ACL Data packet */
#define BT_HCI_H4_SCO                                           0x03    /* HCI Synchronous Data packet */
#define BT_HCI_H4_EVT                                           0x04    /* HCI Event packet */
#define BT_HCI_H4_ISO                                           0x05    /* HCI ISO Data packet */

/*============================= Error Codes =============================*/
#define BT_HCI_ERROR_SUCCESS                                    0x00
#define BT_HCI_ERROR_UNKNOWN_COMMAND                            0x01
#define BT_HCI_ERROR_UNKNOWN_CONN_ID                            0x02
#define BT_HCI_ERROR_HARDWARE_FAILURE                           0x03
#define BT_HCI_ERROR_PIN_OR_KEY_MISSING                         0x06
#define BT_HCI_ERROR_MEM_CAPACITY_EXCEEDED                      0x07
#define BT_HCI_ERROR_CONNECTION_TIMEOUT                         0x08
#define BT_HCI_ERROR_CONNECTION_ALREADY_EXIST                   0x0B
#define BT_HCI_ERROR_CMD_DISALLOWED                             0x0C
#define BT_HCI_ERROR_CONN_REJECTED_DUE_TO_LIMITED_RESOURCE      0x0D
#define BT_HCI_ERROR_UNSUPP_FEATURE_PARAM_VAL                   0x11
#define BT_HCI_ERROR_CMD_INVALID_PARAMETERS                     0x12
#define BT_HCI_ERROR_REMOTE_USER_TERMINATE_CONNECTION           0x13
#define BT_HCI_ERROR_CONN_TERMINATE_BY_LOCAL_HOST               0x16
#define BT_HCI_ERROR_UNSUPP_REMOTE_FEATURE                      0x1A
#define BT_HCI_ERROR_INVALID_LL_PARAM                           0x1E
#define BT_HCI_ERROR_UNSPECIFIC                                 0x1F
#define BT_HCI_ERROR_UNSUPP_LL_PARAM_VAL                        0x20
#define BT_HCI_ERROR_LL_RESPONSE_TIMEOUT                        0x22
#define BT_HCI_ERROR_LL_PORCEDURE_COLLISION                     0x23
#define BT_HCI_ERROR_LMP_PDU_NOT_ALLOWED                        0x24
#define BT_HCI_ERROR_INSTANT_PASSED                             0x28
#define BT_HCI_ERROR_DIFFERENT_TRANSACTION_COLLISION            0x2A
#define BT_HCI_ERROR_UNACCEPT_CONN_PARAM                        0x3B
#define BT_HCI_ERROR_ADVERTISING_TIMEOUT                        0x3C
#define BT_HCL_ERROR_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE   0x3D
#define BT_HCI_ERROR_CONNECTION_FAILED_TO_ESTABLISH             0x3E
#define BT_HCI_ERROR_UNKNOWN_ADVERTISING_IDENTIFIER             0x42
#define BT_HCI_ERROR_LIMIT_REACHED                              0x43
#define BT_HCI_ERROR_PACKET_TOO_LONG                            0x45


/*============================ HCI SUPPORTED COMMANDS DEFINE ===================*/
/*Reference core specification v6.0 */
#define COMMAND_00_HCI_INQUIRY                                                      BIT0
#define COMMAND_00_HCI_INQUIRY_CANCEL                                               BIT1
#define COMMAND_00_HCI_PERIODIC_INQUIRY_MODE                                        BIT2
#define COMMAND_00_HCI_EXIT_PERIODIC_INGUIRY_MODE                                   BIT3
#define COMMAND_00_HCI_CREATE_CONNECTION                                            BIT4
#define COMMAND_00_HCI_DISCONNECT                                                   BIT5
#define COMMAND_00_HCI_CREATE_CONNECTION_CANCEL                                     BIT7

#define COMMAND_01_HCI_ACCEPT_CONNECTION_REQUEST                                    BIT0
#define COMMAND_01_HCI_REJECT_CONNECTION_REQUEST                                    BIT1
#define COMMAND_01_HCI_LINK_KEY_REQUEST_REPLY                                       BIT2
#define COMMAND_01_HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY                              BIT3
#define COMMAND_01_HCI_PIN_CODE_REQUSET_REPLY                                       BIT4
#define COMMAND_01_HCI_PIN_CODE_REQUSET_NEGATIVE_REPLY                              BIT5
#define COMMAND_01_HCI_CHANGE_CONNECTION_PACKET_TYPE                                BIT6
#define COMMAND_01_HCI_AUTHENTICATION_REQUEST                                       BIT7

#define COMMAND_02_HCI_SET_CONNECTION_ENCRYPTION                                    BIT0
#define COMMAND_02_HCI_CHANGE_CONNECTION_LINK_KEY                                   BIT1
#define COMMAND_02_HCI_LINK_KEY_SELECTION                                           BIT2
#define COMMAND_02_HCI_REMOTE_NAME_REQUEST                                          BIT3
#define COMMAND_02_HCI_REMOTE_NAME_REQUEST_CANCEL                                   BIT4
#define COMMAND_02_HCI_READ_REMOTE_SUPPORTED_FEATURES                               BIT5
#define COMMAND_02_HCI_READ_REMOTE_EXTENDED_FEATURES                                BIT6
#define COMMAND_02_HCI_READ_REMOTE_VERSION_INFORMATION                              BIT7

#define COMMAND_03_HCI_READ_CLOCK_OFFSET                                            BIT0
#define COMMAND_03_HCI_READ_LMP_HANDLE                                              BIT1

#define COMMAND_04_HCI_HOLD_MODE                                                    BIT1
#define COMMAND_04_HCI_SNIFF_MODE                                                   BIT2
#define COMMAND_04_HCI_EXIT_SNIFF_MODE                                              BIT3
#define COMMAND_04_HCI_QOS_SETUP                                                    BIT6
#define COMMAND_04_HCI_ROLE_DISCOVERY                                               BIT7

#define COMMAND_05_HCI_SWITCH_ROLE                                                  BIT0
#define COMMAND_05_HCI_READ_LINK_POLICY_SETTINGS                                    BIT1
#define COMMAND_05_HCI_WRITE_LINK_POLICY_SETTINGS                                   BIT2
#define COMMAND_05_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS                            BIT3
#define COMMAND_05_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS                           BIT4
#define COMMAND_05_HCI_FLOW_SPECIFICATION                                           BIT5
#define COMMAND_05_HCI_SET_EVENT_MASK                                               BIT6
#define COMMAND_05_HCI_RESET                                                        BIT7

#define COMMAND_06_HCI_SET_EVENT_FILTER                                             BIT0
#define COMMAND_06_HCI_FLUSH                                                        BIT1
#define COMMAND_06_HCI_READ_PIN_TYPE                                                BIT2
#define COMMAND_06_HCI_WRITE_PIN_TYPE                                               BIT3
#define COMMAND_06_HCI_READ_STORED_LINK_KEY                                         BIT5
#define COMMAND_06_HCI_WRITE_STORED_LINK_KEY                                        BIT6
#define COMMAND_06_HCI_DELETE_STORED_LINK_KEY                                       BIT7

#define COMMAND_07_HCI_WRITE_LOCAL_NAME                                             BIT0
#define COMMAND_07_HCI_READ_LOCAL_NAME                                              BIT1
#define COMMAND_07_HCI_READ_CONNECTION_ACCEPT_TIMEOUT                               BIT2
#define COMMAND_07_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT                              BIT3
#define COMMAND_07_HCI_READ_PAGE_TIMEOUT                                            BIT4
#define COMMAND_07_HCI_WRITE_PAGE_TIMEOUT                                           BIT5
#define COMMAND_07_HCI_READ_SCAN_ENABLE                                             BIT6
#define COMMAND_07_HCI_WRITE_SCAN_ENABLE                                            BIT7

#define COMMAND_08_HCI_READ_PAGE_SCAN_ACTIVITY                                      BIT0
#define COMMAND_08_HCI_WRITE_PAGE_SCAN_ACTIVITY                                     BIT1
#define COMMAND_08_HCI_READ_INQUIRY_SCAN_ACTIVITY                                   BIT2
#define COMMAND_08_HCI_WRITE_INQUIRY_SCAN_ACTIVITY                                  BIT3
#define COMMAND_08_HCI_READ_AUTHENTICATION_ENABLE                                   BIT4
#define COMMAND_08_HCI_WRITE_AUTHENTICATION_ENABLE                                  BIT5

#define COMMAND_09_HCI_READ_CLASS_OF_DEVICE                                         BIT0
#define COMMAND_09_HCI_WRITE_CLASS_OF_DEVICE                                        BIT1
#define COMMAND_09_HCI_READ_VOICE_SETTING                                           BIT2
#define COMMAND_09_HCI_WRITE_VOICE_SETTING                                          BIT3
#define COMMAND_09_HCI_READ_AUTOMATIC_FLUSH_TIMEOUT                                 BIT4
#define COMMAND_09_HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT                                BIT5
#define COMMAND_09_HCI_READ_NUM_BROADCAST_RETRANSMISSIONS                           BIT6
#define COMMAND_09_HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS                          BIT7

#define COMMAND_10_HCI_READ_HOLD_MODE_ACTIVITY                                      BIT0
#define COMMAND_10_HCI_WRITE_HOLD_MODE_ACTIVITY                                     BIT1
#define COMMAND_10_HCI_READ_TRANSMIT_POWER_LEVEL                                    BIT2
#define COMMAND_10_HCI_READ_SYCHRONOUS_FLOW_CONTROL_ENABLE                          BIT3
#define COMMAND_10_HCI_WRITE_SYCHRONOUS_FLOW_CONTROL_ENABLE                         BIT4
#define COMMAND_10_HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL                          BIT5
#define COMMAND_10_HCI_HOST_BUFFER_SIZE                                             BIT6
#define COMMAND_10_HCI_HOST_NUMBER_OF_COMPLETED_PACKETS                             BIT7

#define COMMAND_11_HCI_READ_LINK_SUPERVISION_TIMEOUT                                BIT0
#define COMMAND_11_HCI_WRITE_LINK_SUPERVISION_TIMEOUT                               BIT1
#define COMMAND_11_HCI_READ_NUMBER_OF_SUPPORTED_IAC                                 BIT2
#define COMMAND_11_HCI_READ_CURRENT_IAC_LAP                                         BIT3
#define COMMAND_11_HCI_WRITE_CURRENT_IAC_LAP                                        BIT4

#define COMMAND_12_HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION                          BIT1
#define COMMAND_12_HCI_LE_CS_READ_REMOTE_FAE_TABLE                                  BIT2
#define COMMAND_12_HCI_LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE                          BIT3
#define COMMAND_12_HCI_READ_INQUIRY_SCAN_TYPE                                       BIT4
#define COMMAND_12_HCI_WRITE_INQUIRY_SCAN_TYPE                                      BIT5
#define COMMAND_12_HCI_READ_INQUIRY_MODE                                            BIT6
#define COMMAND_12_HCI_WRITE_INQUIRY_MODE                                           BIT7

#define COMMAND_13_HCI_READ_PAGE_SCAN_TYPE                                          BIT0
#define COMMAND_13_HCI_WRITE_PAGE_SCAN_TYPE                                         BIT1
#define COMMAND_13_HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE                             BIT2
#define COMMAND_13_HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE                            BIT3

#define COMMAND_14_HCI_READ_LOCAL_VERSION_INFORMATION                               BIT3
#define COMMAND_14_HCI_READ_LOCAL_SUPPORTED_FEATURES                                BIT5
#define COMMAND_14_HCI_READ_LOCAL_EXTENDED_FEATURES                                 BIT6
#define COMMAND_14_HCI_READ_BUFFER_SIZE                                             BIT7

#define COMMAND_15_HCI_READ_BD_ADDR                                                 BIT1
#define COMMAND_15_HCI_READ_FAILED_CONTACK_COUNTER                                  BIT2
#define COMMAND_15_HCI_RESET_FAILED_CONTACK_COUNTER                                 BIT3
#define COMMAND_15_HCI_READ_LINK_QUALITY                                            BIT4
#define COMMAND_15_HCI_READ_RSSI                                                    BIT5
#define COMMAND_15_HCI_READ_AFH_CHANNEL_MAP                                         BIT6
#define COMMAND_15_HCI_READ_CLOCK                                                   BIT7

#define COMMAND_16_HCI_READ_LOOPBACK_MODE                                           BIT0
#define COMMAND_16_HCI_WRITE_LOOPBACK_MODE                                          BIT1
#define COMMAND_16_HCI_ENABLE_IMPLEMENTATION_UNDERTEST_MODE                         BIT2
#define COMMAND_16_HCI_SETUP_SYNCHRONOUS_CONNECTION_REQUEST                         BIT3
#define COMMAND_16_HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST                        BIT4
#define COMMAND_16_HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST                        BIT5
#define COMMAND_16_HCI_LE_CS_CREATE_CONFIG                                          BIT6
#define COMMAND_16_HCI_LE_CS_REMOVE_CONFIG                                          BIT7

#define COMMAND_17_HCI_READ_EXTENDED_INQUIRY_RESPONSE                               BIT0
#define COMMAND_17_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE                              BIT1
#define COMMAND_17_HCI_REFRESH_ENCRYPTION_KEY                                       BIT2
#define COMMAND_17_HCI_SNIFF_SUBRATING                                              BIT4
#define COMMAND_17_HCI_READ_SIMPLE_PAIRING_MODE                                     BIT5
#define COMMAND_17_HCI_WRITE_SIMPLE_PAIRING_MODE                                    BIT6
#define COMMAND_17_HCI_READ_LOCAL_OOB_DATA                                          BIT7

#define COMMAND_18_HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL                   BIT0
#define COMMAND_18_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL                           BIT1
#define COMMAND_18_HCI_READ_DEFAULT_ERRONEOUS_DATA_REPORTING                        BIT2
#define COMMAND_18_HCI_WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING                       BIT3
#define COMMAND_18_HCI_IO_CAPABILITY_REQUEST_REPLY                                  BIT7

#define COMMAND_19_HCI_USER_CONFIRMATION_REQUEST_REPLY                              BIT0
#define COMMAND_19_HCI_USER_CONFIRMATION_REQUEST_NAEGATIVE_REPLY                    BIT1
#define COMMAND_19_HCI_USER_PASSKEY_REQUEST_REPLY                                   BIT2
#define COMMAND_19_HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY                          BIT3
#define COMMAND_19_HCI_REMOTE_OOB_DATA_REQUEST_REPLY                                BIT4
#define COMMAND_19_HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE                              BIT5
#define COMMAND_19_HCI_ENHANCED_FLUSH                                               BIT6
#define COMMAND_19_HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY                       BIT7

#define COMMAND_20_HCI_SEND_KEYPRESS_NOTIFICATION                                   BIT2
#define COMMAND_20_HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY                         BIT3
#define COMMAND_20_HCI_READ_ENCRYPTION_KEY_SIZE                                     BIT4
#define COMMAND_20_HCI_LE_CS_READ_LOCAL_SUPPORTED_CAPABILITES                       BIT5
#define COMMAND_20_HCI_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITES                      BIT6
#define COMMAND_20_HCI_LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES             BIT7

#define COMMAND_22_HCI_SET_EVENT_MASK_PAGE_2                                        BIT2

#define COMMAND_23_HCI_READ_FLOW_CONTROL_MODE                                       BIT0
#define COMMAND_23_HCI_WRITE_FLOW_CONTROL_MODE                                      BIT1
#define COMMAND_23_HCI_READ_DATA_BLOCK_SIZE                                         BIT2
#define COMMAND_23_HCI_LE_CS_TEST                                                   BIT3
#define COMMAND_23_HCI_LE_S_TEST_END                                                BIT4

#define COMMAND_24_HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL                           BIT0
#define COMMAND_24_HCI_LE_CS_SECURITY_ENABLE                                        BIT1
#define COMMAND_24_HCI_READ_LE_HOST_SUPPORT                                         BIT5
#define COMMAND_24_HCI_WRITE_LE_HOST_SUPPORT                                        BIT6
#define COMMAND_24_HCI_LE_CS_SET_DEFAULT_SETTINGS                                   BIT7

#define COMMAND_25_HCI_LE_SET_EVENT_MASK                                            BIT0
#define COMMAND_25_HCI_LE_READ_BUFFER_SIZE_V1                                       BIT1
#define COMMAND_25_HCI_LE_READ_LOCAL_SUPPORTED_FEATURES_PAGE_0                      BIT2
#define COMMAND_25_HCI_LE_SET_RANDOM_ADDRESS                                        BIT4
#define COMMAND_25_HCI_LE_SET_ADVERTISING_PARAMETERS                                BIT5
#define COMMAND_25_HCI_LE_READ_ADVERTISING_PHYSICAL_CHANNEL_TX_POWER                BIT6
#define COMMAND_25_HCI_LE_SET_ADVERTISING_DATA                                      BIT7

#define COMMAND_26_HCI_LE_SET_RESPONSE_DATA                                         BIT0
#define COMMAND_26_HCI_LE_SET_ADVERTISING_ENABLE                                    BIT1
#define COMMAND_26_HCI_LE_SET_SCAN_PARAMETERS                                       BIT2
#define COMMAND_26_HCI_LE_SET_SCAN_ENABLE                                           BIT3
#define COMMAND_26_HCI_LE_CREATE_CONNECTION                                         BIT4
#define COMMAND_26_HCI_LE_CREATE_CONNECTION_CANCEL                                  BIT5
#define COMMAND_26_HCI_LE_READ_FILTER_ACCEPT_LIST_SIZE                              BIT6
#define COMMAND_26_HCI_LE_CLEAR_FILTER_ACCEPT_LIST                                  BIT7

#define COMMAND_27_HCI_LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST                          BIT0
#define COMMAND_27_HCI_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST                     BIT1
#define COMMAND_27_HCI_LE_CONNECTION_UPDATE                                         BIT2
#define COMMAND_27_HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION                           BIT3
#define COMMAND_27_HCI_LE_READ_CHANNEL_MAP                                          BIT4
#define COMMAND_27_HCI_LE_READ_REMOTE_FEATURES_PAGE_0                               BIT5
#define COMMAND_27_HCI_LE_ENCRYPT                                                   BIT6
#define COMMAND_27_HCI_LE_RAND                                                      BIT7

#define COMMAND_28_HCI_LE_ENABLE_ENCRYPTION                                         BIT0
#define COMMAND_28_HCI_LE_LONG_TERM_KEY_REQUEST_REPLY                               BIT1
#define COMMAND_28_HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY                      BIT2
#define COMMAND_28_HCI_LE_READ_SUPPORTED_STATES                                     BIT3
#define COMMAND_28_HCI_LE_RECEIVER_TEST_V1                                          BIT4
#define COMMAND_28_HCI_LE_TRANSMITTER_TEST_V1                                       BIT5
#define COMMAND_28_HCI_LE_TEST_END                                                  BIT6
#define COMMAND_28_HCI_LE_ENABLE_MONITORING_ADVERTISER                              BIT7

#define COMMAND_29_HCI_LE_CS_SET_CHANNEL_CLASSIFICATION                             BIT0
#define COMMAND_29_HCI_LE_CS_SET_PROCEDURE_PARAMETERS                               BIT1
#define COMMAND_29_HCI_LE_CS_PROCEDURE_ENABLE                                       BIT2
#define COMMAND_29_HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION                        BIT3
#define COMMAND_29_HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION                       BIT4
#define COMMAND_29_HCI_READ_LOCAL_SUPPORTED_CODEC_V1                                BIT5
#define COMMAND_29_HCI_SET_MWS_CHANNEL_PARAMETERS                                   BIT6
#define COMMAND_29_HCI_SET_EXTERNAL_FRAME_CONFIGURATION                             BIT7

#define COMMAND_30_HCI_SET_MWS_SIGNALING                                            BIT0
#define COMMAND_30_HCI_SET_MWS_TRANSPORT_LAYER                                      BIT1
#define COMMAND_30_HCI_SET_MWS_SCAN_FREQUENCY_TABLE                                 BIT2
#define COMMAND_30_HCI_GET_MWS_TRANSPORT_LAYER_CONFIGURATION                        BIT3
#define COMMAND_30_HCI_SET_MWS_PATTERN_CONFIGURATION                                BIT4
#define COMMAND_30_HCI_SET_TRIGGERED_CLOCK_CAPTURE                                  BIT5
#define COMMAND_30_HCI_TRUNCATED_PAGE                                               BIT6
#define COMMAND_30_HCI_TRUNCATED_PAGE_CANCEL                                        BIT7

#define COMMAND_31_HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST                      BIT0
#define COMMAND_31_HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE              BIT1
#define COMMAND_31_HCI_START_SYNCHRONIZATION_TRAIN                                  BIT2
#define COMMAND_31_HCI_RECEIVE_SYNCHRONIZATION_TRAIN                                BIT3
#define COMMAND_31_HCI_SET_RESERVED_LT_ADDR                                         BIT4
#define COMMAND_31_HCI_DELETE_RESERVED_LT_ADDR                                      BIT5
#define COMMAND_31_HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_DATA                 BIT6
#define COMMAND_31_HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS                        BIT7

#define COMMAND_32_HCI_WRITE_SYCHRONIZATION_TRAIN_PARAMETERS                        BIT0
#define COMMAND_32_HCI_REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY                       BIT1
#define COMMAND_32_HCI_READ_SECURE_CONNECTIONS_HOST_SUPPORT                         BIT2
#define COMMAND_32_HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT                        BIT3
#define COMMAND_32_HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT                           BIT4
#define COMMAND_32_HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT                          BIT5
#define COMMAND_32_HCI_READ_LOCAL_OOB_EXTENDED_DATA                                 BIT6
#define COMMAND_32_HCI_WRITE_SECURE_CONNECTIONS_TEST_MODE                           BIT7

#define COMMAND_33_HCI_READ_EXTENDED_PAGE_TIMEOUT                                   BIT0
#define COMMAND_33_HCI_WRITE_EXTENDED_PAGE_TIMEOUT                                  BIT1
#define COMMAND_33_HCI_READ_EXTENDED_INQUIRY_LENGTH                                 BIT2
#define COMMAND_33_HCI_WRITE_EXTENDED_INQUIRT_LENGTH                                BIT3
#define COMMAND_33_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY                 BIT4
#define COMMAND_33_HCI_LE_REMOTE_CONNECTION_PATRAMETER_REQUEST_NEGATIVE_REPLY       BIT5
#define COMMAND_33_HCI_LE_SET_DATA_LENGTH                                           BIT6
#define COMMAND_33_HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH                        BIT7

#define COMMAND_34_HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH                       BIT0
#define COMMAND_34_HCI_LE_READ_LOCAL_P_256_PUBLIC_KEY                               BIT1
#define COMMAND_34_HCI_LE_GENERATE_DHKEY_V1                                         BIT2
#define COMMAND_34_HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST                              BIT3
#define COMMAND_34_HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST                         BIT4
#define COMMAND_34_HCI_LE_CLEAR_RESOLVING_LIST                                      BIT5
#define COMMAND_34_HCI_LE_READ_RESOLVING_LIST_SIZE                                  BIT6
#define COMMAND_34_HCI_LE_READ_PEER_RESOLVABLE_ADDRESS                              BIT7

#define COMMAND_35_HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS                             BIT0
#define COMMAND_35_HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE                             BIT1
#define COMMAND_35_HCI_LE_SET_RESOLUABLE_PRIVATE_ADDRESS_TIMEOUT                    BIT2
#define COMMAND_35_HCI_LE_READ_MAXIMUM_DATA_LENGTH                                  BIT3
#define COMMAND_35_HCI_LE_READ_PH                                                   BIT4
#define COMMAND_35_HCI_LE_SET_DEFAULT_PHY                                           BIT5
#define COMMAND_35_HCI_LE_SET_PHY                                                   BIT6
#define COMMAND_35_HCI_LE_RECEIVER_TEST_V2                                          BIT7

#define COMMAND_36_HCI_LE_TRANSMITTER_TEST_V2                                       BIT0
#define COMMAND_36_HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS                        BIT1
#define COMMAND_36_HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETER_V1                     BIT2
#define COMMAND_36_HCI_LE_SET_EXTENDED_ADVERTISING_DATA                             BIT3
#define COMMAND_36_HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA                           BIT4
#define COMMAND_36_HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE                           BIT5
#define COMMAND_36_HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH                      BIT6
#define COMMAND_36_HCI_LE_READ_NUMBER_OF_SUPPORT_ADVERTISING_SET                    BIT7

#define COMMAND_37_HCI_LE_REMOVE_ADVERTISING_SET                                    BIT0
#define COMMAND_37_HCI_LE_CLEAR_ADVERTISING_SET                                     BIT1
#define COMMAND_37_HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETER_V1                     BIT2
#define COMMAND_37_HCI_LE_SET_PERIODIC_ADVERTISING_DATA                             BIT3
#define COMMAND_37_HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE                           BIT4
#define COMMAND_37_HCI_LE_SET_EXTENDED_SCAN_PARAMETERS                              BIT5
#define COMMAND_37_HCI_LE_SET_EXTENDED_SCAN_ENABLE                                  BIT6
#define COMMAND_37_HCI_LE_EXTENDED_CREATE_CONNECTION_V1                             BIT7

#define COMMAND_38_HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC                          BIT0
#define COMMAND_38_HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL                   BIT1
#define COMMAND_38_HCI_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC                       BIT2
#define COMMAND_38_HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISING_LIST                   BIT3
#define COMMAND_38_HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST               BIT4
#define COMMAND_38_HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST                            BIT5
#define COMMAND_38_HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE                        BIT6
#define COMMAND_38_HCI_LE_READ_TRANSMIT_POWER                                       BIT7

#define COMMAND_39_HCI_LE_READ_RF_COMPENSATION                                      BIT0
#define COMMAND_39_HCI_LE_WRITE_RF_COMPENSATION                                     BIT1
#define COMMAND_39_HCI_LE_SET_PRIVACY_MODE                                          BIT2
#define COMMAND_39_HCI_LE_RECEIVE_TEST_V3                                           BIT3
#define COMMAND_39_HCI_LE_TRANSMIT_TEST_V3                                          BIT4
#define COMMAND_39_HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMETERS                BIT5
#define COMMAND_39_HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE                    BIT6
#define COMMAND_39_HCI_LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE                     BIT7

#define COMMAND_40_HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMETERS                     BIT0
#define COMMAND_40_HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMETERS                    BIT1
#define COMMAND_40_HCI_LE_CONNECTION_CTE_REQUEST_ENABLE                             BIT2
#define COMMAND_40_HCI_LE_CONNECTION_CTE_RESPONSE_ENABLE                            BIT3
#define COMMAND_40_HCI_LE_READ_ANTENNA_INFORMATION                                  BIT4
#define COMMAND_40_HCI_LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE                   BIT5
#define COMMAND_40_HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER                        BIT6
#define COMMAND_40_HCI_LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER                    BIT7

#define COMMAND_41_HCI_LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS         BIT0
#define COMMAND_41_HCI_LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS BIT1
#define COMMAND_41_HCI_LE_GENERATE_DHKEY_V2                                         BIT2
#define COMMAND_41_HCI_READ_LOCAL_SIMPLE_PAIRING_OPTIONS                            BIT3
#define COMMAND_41_HCI_LE_MODIFY_SLEEP_CLOCK_ACCURACY                               BIT4
#define COMMAND_41_HCI_LE_READ_BUFFER_SIZE_V2                                       BIT5
#define COMMAND_41_HCI_LE_READ_ISO_TX_SYNC                                          BIT6
#define COMMAND_41_HCI_LE_SET_CIG_PARAMETERS                                        BIT7

#define COMMAND_42_HCI_LE_SET_CIG_PARAMETERS_TEST                                   BIT0
#define COMMAND_42_HCI_LE_CREATE_CIS                                                BIT1
#define COMMAND_42_HCI_LE_REMOVE_CIG                                                BIT2
#define COMMAND_42_HCI_LE_ACCEPT_CIS_REQUEST                                        BIT3
#define COMMAND_42_HCI_LE_REJECT_CIS_REQUEST                                        BIT4
#define COMMAND_42_HCI_LE_CREATE_BIG                                                BIT5
#define COMMAND_42_HCI_LE_CREATE_BIG_TEST                                           BIT6
#define COMMAND_42_HCI_LE_TERMINATE_BIG                                             BIT7

#define COMMAND_43_HCI_BIG_CREATE_SYNC                                              BIT0
#define COMMAND_43_HCI_LE_BIG_TERMINATE_SYNC                                        BIT1
#define COMMAND_43_HCI_LE_REQUEST_PEER_SCA                                          BIT2
#define COMMAND_43_HCI_LE_SETUP_ISO_DATA_PATH                                       BIT3
#define COMMAND_43_HCI_LE_REMOVE_ISO_DATA_PATH                                      BIT4
#define COMMAND_43_HCI_LE_ISO_TRANSMIT_TEST                                         BIT5
#define COMMAND_43_HCI_LE_ISO_RECEIVE_TEST                                          BIT6
#define COMMAND_43_HCI_LE_ISO_READ_TEST_COUNTERS                                    BIT7

#define COMMAND_44_HCI_LE_ISO_TEST_END                                              BIT0
#define COMMAND_44_HCI_LE_SET_HOST_FEATURE_V1                                       BIT1
#define COMMAND_44_HCI_LE_READ_ISO_LINK_QUALITY                                     BIT2
#define COMMAND_44_HCI_LE_ENHANCED_READ_TRANSMIT_POWER_LEVEL                        BIT3
#define COMMAND_44_HCI_LE_READ_REMOTE_TRANSMIT_POWER_LEVEL                          BIT4
#define COMMAND_44_HCI_LE_SET_PATH_LOSS_REPORTING_PARAMETERS                        BIT5
#define COMMAND_44_HCI_LE_SET_PATH_LOSS_REPORTING_ENABLE                            BIT6
#define COMMAND_44_HCI_LE_SET_TRANSMIT_POWER_REPORTING_ENABLE                       BIT7

#define COMMAND_45_HCI_LE_TRANSMITTER_TEST_V4                                       BIT0
#define COMMAND_45_HCI_SET_ECOSYSTEM_BASE_INTERVAL                                  BIT1
#define COMMAND_45_HCI_READ_LOCAL_SUPPORTED_CODECS_V2                               BIT2
#define COMMAND_45_HCI_READ_LOCAL_SUPPORTED_CODEC_CAPALITIES                        BIT3
#define COMMAND_45_HCI_READ_LOCAL_SUPPORTED_CONTROLLER_DELAY                        BIT4
#define COMMAND_45_HCI_CONFIGURE_DATA_PATH                                          BIT5
#define COMMAND_45_HCI_LE_SET_DATA_RELATED_ADDRESS_CHANGES                          BIT6
#define COMMAND_45_HCI_SET_MIN_ENCRYPTION_KEY_SIZE                                  BIT7

#define COMMAND_46_HCI_LE_SET_DEFAULT_SUBRATE_COMMAND                               BIT0
#define COMMAND_46_HCI_LE_SUBRATE_REQUEST_COMMAND                                   BIT1
#define COMMAND_46_HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS_V2                    BIT2
#define COMMAND_46_HCI_LE_SET_DECISION_DATA                                         BIT3
#define COMMAND_46_HCI_LE_DECISION_INSTRICTIONS                                     BIT4
#define COMMAND_46_HCI_LE_SET_PERIODIC_ADVERTISING_SIBEVENT_DATA                    BIT5
#define COMMAND_46_HCI_LE_SET_PERIODIC_ADVERTISING_RESPONSE_DATA                    BIT6
#define COMMAND_46_HCI_LE_SET_PERIODIC_SYNC_SUBEVENT                                BIT7

#define COMMAND_47_HCI_LE_EXTENDED_CREATE_CONNECTIONS_V2                            BIT0
#define COMMAND_47_HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS_V2                    BIT1
#define COMMAND_47_HCI_LE_READ_ALL_LOCAL_SUPPORTED_FEATURES                         BIT2
#define COMMAND_47_HCI_LE_READ_ALL_REMOTE_FEATURES                                  BIT3
#define COMMAND_47_HCI_LE_SET_HOST_FEATURE_V2                                       BIT4
#define COMMAND_47_HCI_LE_ADD_DEVICE_TO_MONITORED_ADVERTISERS_LIST                  BIT5
#define COMMAND_47_HCI_EL_REMOVE_DEVICE_FROM_MONITORED_ADVERTISERS_LIST             BIT6
#define COMMAND_47_HCI_LE_CLEAR_MONITORED_ADVERTISERS_LIST                          BIT7

#define COMMAND_48_HCI_LE_READ_MONITORED_ADVERTISERS_LIST_SIZE                      BIT0
#define COMMAND_48_HCI_LE_FRAME_SPACE_UPDATE                                        BIT1


//BT 6.0 Spec
#define BT_HCI_LE_EXT_ADV_PROP_CONNECTABLE                                  BIT0
#define BT_HCI_LE_EXT_ADV_PROP_SCANNABLE                                    BIT1
#define BT_HCI_LE_EXT_ADV_PROP_DIRECT                                       BIT2
#define BT_HCI_LE_EXT_ADV_PROP_HIGH_DUTY_DIRECT                             BIT3
#define BT_HCI_LE_EXT_ADV_PROP_LEGACY                                       BIT4
#define BT_HCI_LE_EXT_ADV_PROP_ANONYNOUS                                    BIT5
#define BT_HCI_LE_EXT_ADV_PROP_TX_POWER                                     BIT6
#define BT_HCI_LE_EXT_ADV_PROP_USE_DECISION_PDU                             BIT7
#define BT_HCI_LE_EXT_ADV_PROP_ADVA_DECISION_PDU                            BIT8
#define BT_HCI_LE_EXT_ADV_PROP_ADI_DECISION_PDU                             BIT9


/*============================ SUPPORTED FEATURE DEFINE ===================*/
#ifndef BIT64
#define BIT64(i)   (1ULL << (i))
#endif

#define BT_LL_LE_FEATURE_ENCRYPTION                             BIT64(0)
#define BT_LL_LE_FEATURE_CONN_PARA_REQ_PROCEDURE                BIT64(1)
#define BT_LL_LE_FEATURE_EXT_REJECT_IND                         BIT64(2)
#define BT_LL_LE_FEATURE_PERIPHERAL_INIT_FEATURE_EXCHANGE       BIT64(3)
#define BT_LL_LE_FEATURE_LE_PING                                BIT64(4)
#define BT_LL_LE_FEATURE_LE_DATA_LENGTH_EXTENSION               BIT64(5)
#define BT_LL_LE_FEATURE_LL_PRIVACY                             BIT64(6)
#define BT_LL_LE_FEATURE_EXT_SCAN_FILTER_POLICY                 BIT64(7)
#define BT_LL_LE_FEATURE_LE_2M_PHY                              BIT64(8)
#define BT_LL_LE_FEATURE_STABLE_MODULATION_INDEX_TX             BIT64(9)
#define BT_LL_LE_FEATURE_STABLE_MODULATION_INDEX_RX             BIT64(10)
#define BT_LL_LE_FEATURE_LE_CODED_PHY                           BIT64(11)
#define BT_LL_LE_FEATURE_LE_EXTENDED_ADV                        BIT64(12)
#define BT_LL_LE_FEATURE_LE_PERIODIC_ADV                        BIT64(13)
#define BT_LL_LE_FEATURE_LE_CHANNEL_SEL_2                       BIT64(14)
#define BT_LL_LE_FEATURE_LE_POWER_CLASS_1                       BIT64(15)
#define BT_LL_LE_FEATURE_MIN_NUM_USED_CHANNEL                   BIT64(16)
#define BT_LL_LE_FEATURE_CONNECTION_CTE_REQUEST                 BIT64(17)
#define BT_LL_LE_FEATURE_CONNECTION_CTE_RESPONSE                BIT64(18)
#define BT_LL_LE_FEATURE_CONNECTIONLESS_CTE_TRANSMITTER         BIT64(19)
#define BT_LL_LE_FEATURE_CONNECTIONLESS_CTE_RECEIVER            BIT64(20)
#define BT_LL_LE_FEATURE_ANTENNA_SWITCH_AOD                     BIT64(21)
#define BT_LL_LE_FEATURE_ANTENNA_SWITCH_AOA                     BIT64(22)
#define BT_LL_LE_FEATURE_RECEIVE_CTE                            BIT64(23)
#define BT_LL_LE_FEATURE_PAST_SENDER                            BIT64(24)
#define BT_LL_LE_FEATURE_PAST_RECEIVER                          BIT64(25)
#define BT_LL_LE_FEATURE_SLEEP_CLOCK_ACCURANCY_UPDATE           BIT64(26)
#define BT_LL_LE_FEATURE_REMOTE_PUBLIC_KEY_VALIDATION           BIT64(27)
#define BT_LL_LE_FEATURE_CIS_CENTRAL                            BIT64(28)
#define BT_LL_LE_FEATURE_CIS_PERIPHERAL                         BIT64(29)
#define BT_LL_LE_FEATURE_ISO_BROADCASTER                        BIT64(30)
#define BT_LL_LE_FEATURE_SYNCHRONIZED_RECEIVER                  BIT64(31)
#define BT_LL_LE_FEATURE_CIS_HOST_SUPPORT                       BIT64(32)
#define BT_LL_LE_FEATURE_LE_POWER_CONTROL_REQUEST               BIT64(33)
#define BT_LL_LE_FEATURE_LE_POWER_CONTROL_RESPONSE              BIT64(34)
#define BT_LL_LE_FEATURE_LE_PATH_LOSS_MONITORING                BIT64(35)
#define BT_LL_LE_FEATURE_LE_PA_ADI_SUPPORT                      BIT64(36)
#define BT_LL_LE_FEATURE_CONNECTION_SUBRATING                   BIT64(37)
#define BT_LL_LE_FEATURE_CONNECTION_SUBRATING_HOST_SUPPORT      BIT64(38)
#define BT_LL_LE_FEATURE_CHANNEL_CLASSIFICATION                 BIT64(39)
#define BT_LL_LE_FEATURE_ADV_CODING_SEL                         BIT64(40)
#define BT_LL_LE_FEATURE_ADV_CODING_SEL_HOST_SUPPORT            BIT64(41)
#define BT_LL_LE_FEATURE_PAWR_ADVERTISER                        BIT64(42)
#define BT_LL_LE_FEATURE_PAWR_SCANNER                           BIT64(43)


#define LE_HCI_PHY_1M                                        0x01
#define LE_HCI_PHY_2M                                        0x02
#define LE_HCI_PHY_CODED                                     0x03

#define LE_HCI_PHY_BIT_1M       BIT(0)
#define LE_HCI_PHY_BIT_2M       BIT(1)
#define LE_HCI_PHY_BIT_CODED    BIT(2)
#define LE_HCI_PHY_BITS_MASK    (BIT(2) | BIT(1) | BIT(0))

#define BT_OGF(opcode)                                          ((opcode >> 10) & 0x3F)
#define BT_OCF(opcode)                                          ((opcode) & 0x3FF)

#define BT_OP(ogf, ocf)                                         ((ocf) | ((ogf) << 10))

/* Command Group Opcode */
#define BT_OGF_LINK_CTRL                                        0x01
#define BT_OGF_BASEBAND                                         0x03
#define BT_OGF_INFO                                             0x04
#define BT_OGF_STATUS                                           0x05
#define BT_OGF_LE                                               0x08
#define BT_OGF_VENDOR                                           0x3F

#define BT_ADDR_SIZE            6

struct bt_le_addr_t {
	uint8_t type;
	uint8_t addr[BT_ADDR_SIZE];
};

/* Link Control commands */
#define BT_HCI_OP_DISCONNECT                    BT_OP(BT_OGF_LINK_CTRL, 0x0006) /* 0x0406 */
struct bt_hci_cp_disconnect {
	uint16_t handle;
	uint8_t  reason;
} __attribute__((packed));

#define BT_HCI_OP_READ_REMOTE_VERSION_INFO      BT_OP(BT_OGF_LINK_CTRL, 0x001d) /* 0x041d */
struct bt_hci_cp_read_remote_version_info {
	uint16_t handle;
} __attribute__((packed));


/* Controller & Baseband commands */
#define BT_HCI_OP_SET_EVENT_MASK                BT_OP(BT_OGF_BASEBAND, 0x0001) /* 0x0c01 */
struct bt_hci_cp_set_event_mask {
	uint8_t  events[8];
} __attribute__((packed));

#define BT_HCI_OP_RESET                         BT_OP(BT_OGF_BASEBAND, 0x0003) /* 0x0c03 */

#define BT_HCI_OP_READ_LOCAL_VERSION_INFO       BT_OP(BT_OGF_INFO, 0x0001) /* 0x1001 */
struct bt_hci_rp_read_local_version_info {
	uint8_t  status;
	uint8_t  hci_version;
	uint16_t hci_revision;
	uint8_t  lmp_version;
	uint16_t manufacturer;
	uint16_t lmp_subversion;
} __attribute__((packed));

#define BT_HCI_OP_READ_SUPPORTED_COMMANDS       BT_OP(BT_OGF_INFO, 0x0002) /* 0x1002 */
struct bt_hci_rp_read_supported_commands {
	uint8_t  status;
	uint8_t  commands[64];
} __attribute__((packed));

#define BT_HCI_OP_READ_LOCAL_FEATURES           BT_OP(BT_OGF_INFO, 0x0003) /* 0x1003 */
struct bt_hci_rp_read_local_features {
	uint8_t  status;
	uint8_t  features[8];
} __attribute__((packed));

#define BT_HCI_OP_READ_BD_ADDR                  BT_OP(BT_OGF_INFO, 0x0009) /* 0x1009 */
struct bt_hci_rp_read_bd_addr {
	uint8_t   status;
	uint8_t   bdaddr[BT_ADDR_SIZE];
} __attribute__((packed));

#define BT_HCI_OP_READ_RSSI                     BT_OP(BT_OGF_STATUS, 0x0005) /* 0x1405 */
#define BT_HCI_RSSI_NOT_AVAILABLE               0X7F
struct bt_hci_cp_read_rssi {
	uint16_t handle;
} __attribute__((packed));

struct bt_hci_rp_read_rssi {
	uint8_t  status;
	uint16_t handle;
	int8_t   rssi;
} __attribute__((packed));


#define BT_HCI_OP_LE_SET_EVENT_MASK             BT_OP(BT_OGF_LE, 0x0001) /* 0x2001 */
struct bt_hci_cp_le_set_event_mask {
	uint8_t events[8];
} __attribute__((packed));

#define BT_HCI_OP_LE_READ_BUFFER_SIZE           BT_OP(BT_OGF_LE, 0x0002) /* 0x2002 */
struct bt_hci_rp_le_read_buffer_size {
	uint8_t  status;
	uint16_t le_max_len;
	uint8_t  le_max_num;
} __attribute__((packed));

#define BT_HCI_OP_LE_READ_LOCAL_FEATURES        BT_OP(BT_OGF_LE, 0x0003) /* 0x2003 */
struct bt_hci_rp_le_read_local_features {
	uint8_t  status;
	uint8_t  features[8];
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_RANDOM_ADDRESS         BT_OP(BT_OGF_LE, 0x0005) /* 0x2005 */
struct bt_hci_cp_le_set_random_address {
	uint8_t random_addr[BT_ADDR_SIZE];
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_ADV_PARAM              BT_OP(BT_OGF_LE, 0x0006) /* 0x2006 */

#define BT_HCI_ADV_IND                          0x00
#define BT_HCI_ADV_DIRECT_IND_HIGH_DUTY         0x01
#define BT_HCI_ADV_SCAN_IND                     0x02
#define BT_HCI_ADV_NONCONN_IND                  0x03
#define BT_HCI_ADV_DIRECT_IND_LOW_DUTY          0x04

#define BT_LE_ADV_FP_NO_FILTER                  0x00
#define BT_LE_ADV_FP_FILTER_SCAN_REQ            0x01
#define BT_LE_ADV_FP_FILTER_CONN_IND            0x02
#define BT_LE_ADV_FP_FILTER_BOTH                0x03

struct bt_hci_cp_le_set_adv_param {
	uint16_t     min_interval;
	uint16_t     max_interval;
	uint8_t      type;
	uint8_t      own_addr_type;
	struct bt_le_addr_t direct_addr;
	uint8_t      channel_map;
	uint8_t      filter_policy;
} __attribute__((packed));

#define BT_HCI_OP_LE_READ_ADV_CHAN_TX_POWER     BT_OP(BT_OGF_LE, 0x0007) /* 0x2007 */
struct bt_hci_rp_le_read_adv_chan_tx_power {
	uint8_t status;
	int8_t tx_power;
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_ADV_DATA               BT_OP(BT_OGF_LE, 0x0008) /* 0x2008 */
struct bt_hci_cp_le_set_adv_data {
	uint8_t  len;
	uint8_t  data[31];
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_SCAN_RSP_DATA          BT_OP(BT_OGF_LE, 0x0009) /* 0x2009 */
struct bt_hci_cp_le_set_scan_rsp_data {
	uint8_t  len;
	uint8_t  data[31];
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_ADV_ENABLE             BT_OP(BT_OGF_LE, 0x000A) /* 0x200a */
struct bt_hci_cp_le_set_adv_enable {
	uint8_t  enable;
} __attribute__((packed));


#define BT_HCI_OP_LE_SET_SCAN_PARAM             BT_OP(BT_OGF_LE, 0x000B) /* 0x200b */
#define BT_HCI_LE_SCAN_TYPE_PASSIVE             0x00
#define BT_HCI_LE_SCAN_TYPE_ACTIVE              0x01

#define BT_HCI_LE_SCAN_FP_BASIC_NO_FILTER    0x00
#define BT_HCI_LE_SCAN_FP_BASIC_FILTER       0x01
#define BT_HCI_LE_SCAN_FP_EXT_NO_FILTER      0x02
#define BT_HCI_LE_SCAN_FP_EXT_FILTER         0x03

struct bt_hci_scan_phy {
	uint8_t  type;
	uint16_t interval;
	uint16_t window;
} __attribute__((packed));

struct bt_hci_cp_le_set_scan_param {
	struct bt_hci_scan_phy p;
	uint8_t  addr_type;
	uint8_t  filter_policy;
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_SCAN_ENABLE            BT_OP(BT_OGF_LE, 0x000c) /* 0x200c */
#define BT_HCI_LE_SCAN_DISABLE                  0x00
#define BT_HCI_LE_SCAN_ENABLE                   0x01

#define BT_HCI_LE_SCAN_FILTER_DUP_DISABLE       0x00
#define BT_HCI_LE_SCAN_FILTER_DUP_ENABLE        0x01
#define BT_HCI_LE_SCAN_FILTER_DUP_RESET_EACH_SCAN_PERI  0x02

struct bt_hci_cp_le_set_scan_enable {
	uint8_t  enable;
	uint8_t  filter_duplicates;
} __attribute__((packed));

#define BT_HCI_OP_LE_CREATE_CONN                BT_OP(BT_OGF_LE, 0x000d) /* 0x200d */

#define BT_HCI_LE_CREATE_CONN_FP_NO_FILTER      0x00
#define BT_HCI_LE_CREATE_CONN_FP_FILTER         0x01

struct bt_hci_cp_le_create_conn {
	uint16_t     scan_interval;
	uint16_t     scan_window;
	uint8_t      filter_policy;
	struct bt_le_addr_t peer_addr;
	uint8_t      own_addr_type;
	uint16_t     conn_interval_min;
	uint16_t     conn_interval_max;
	uint16_t     conn_latency;
	uint16_t     supervision_timeout;
	uint16_t     min_ce_len;
	uint16_t     max_ce_len;
} __attribute__((packed));

#define BT_HCI_OP_LE_CREATE_CONN_CANCEL         BT_OP(BT_OGF_LE, 0x000e) /* 0x200e */

#define BT_HCI_OP_LE_CLEAR_ALL_FAL              BT_OP(BT_OGF_LE, 0x0010) /* 0x2010 */

#define BT_HCI_OP_LE_ADD_DEV_TO_FAL              BT_OP(BT_OGF_LE, 0x0011) /* 0x2011 */
struct bt_hci_cp_le_add_dev_to_fal {
	uint8_t addr_type;
	uint8_t addr[BT_ADDR_SIZE];
} __attribute__((packed));

#define BT_HCI_OP_LE_REM_DEV_FROM_FAL            BT_OP(BT_OGF_LE, 0x0012) /* 0x2012 */
struct bt_hci_cp_le_rem_dev_from_fal {
	uint8_t addr_type;
	uint8_t addr[BT_ADDR_SIZE];
} __attribute__((packed));

#define BT_HCI_OP_LE_CONN_UPDATE                BT_OP(BT_OGF_LE, 0x0013) /* 0x2013 */
struct hci_cp_le_conn_update {
	uint16_t handle;
	uint16_t interval_min;
	uint16_t interval_max;
	uint16_t latency;
	uint16_t supervision_timeout;
	uint16_t min_ce_len;
	uint16_t max_ce_len;
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_HOST_CHAN_CLASSIF      BT_OP(BT_OGF_LE, 0x0014) /* 0x2014 */
struct bt_hci_cp_le_set_host_chan_classif {
	uint8_t  ch_map[5];
} __attribute__((packed));

#define BT_HCI_OP_LE_READ_CHAN_MAP              BT_OP(BT_OGF_LE, 0x0015) /* 0x2015 */
struct bt_hci_cp_le_read_chan_map {
	uint16_t handle;
} __attribute__((packed));
struct bt_hci_rp_le_read_chan_map {
	uint8_t  status;
	uint16_t handle;
	uint8_t  ch_map[5];
} __attribute__((packed));
#define BT_HCI_OP_LE_READ_REMOTE_FEATURES       BT_OP(BT_OGF_LE, 0x0016) /* 0x2016 */
struct bt_hci_cp_le_read_remote_features {
	uint16_t  handle;
} __attribute__((packed));

#define BT_HCI_OP_LE_ENCRYPT                    BT_OP(BT_OGF_LE, 0x0017) /* 0x2017 */
struct bt_hci_cp_le_encrypt {
	uint8_t  key[16];
	uint8_t  plaintext_data[16];
} __attribute__((packed));

struct bt_hci_rp_le_encrypt {
	uint8_t  status;
	uint8_t  encrypted_data[16];
} __attribute__((packed));

#define BT_HCI_OP_LE_RAND                       BT_OP(BT_OGF_LE, 0x0018) /* 0x2018 */
struct bt_hci_rp_le_rand {
	uint8_t  status;
	uint8_t  rand[8];
} __attribute__((packed));

#define BT_HCI_OP_LE_ENABLE_ENCRYPTION           BT_OP(BT_OGF_LE, 0x0019) /* 0x2019 */
struct bt_hci_cp_le_enable_encryption {
	uint16_t  handle;
	uint8_t   rand[8];
	uint8_t   enc_div[2];
	uint8_t   ltk[16];
} __attribute__((packed));

#define BT_HCI_OP_LE_LTK_REQ_REPLY              BT_OP(BT_OGF_LE, 0x001A) /* 0x201A */
struct bt_hci_cp_le_ltk_req_reply {
	uint16_t  handle;
	uint8_t   ltk[16];
} __attribute__((packed));

struct bt_hci_rp_le_ltk_req_reply {
	uint8_t  status;
	uint16_t handle;
} __attribute__((packed));

#define BT_HCI_OP_LE_LTK_REQ_NEG_REPLY          BT_OP(BT_OGF_LE, 0x001b) /* 0x201b */
struct bt_hci_cp_le_ltk_req_neg_reply {
	uint16_t handle;
} __packed;
struct bt_hci_rp_le_ltk_req_neg_reply {
	uint8_t  status;
	uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_READ_SUPP_STATES           BT_OP(BT_OGF_LE, 0x001c) /* 0x201c */
struct bt_hci_rp_le_read_supp_states {
	uint8_t  status;
	uint8_t  le_states[8];
} __packed;

#define BT_HCI_OP_LE_RECEIVER_TEST_V1                    BT_OP(BT_OGF_LE, 0x001D) /* 0x201D */
struct bt_hci_cp_le_receiver_test_v1 {
	uint8_t  rx_channel;
} __attribute__((packed));

#define BT_HCI_OP_LE_TRANSMITTER_TEST_V1                    BT_OP(BT_OGF_LE, 0x001E) /* 0x201E */
struct bt_hci_cp_le_transmitter_test_v1 {
	uint8_t  tx_channel;
	uint8_t  test_data_length;
	uint8_t  packet_payload;
} __attribute__((packed));

#define BT_HCI_OP_LE_TEST_END                    BT_OP(BT_OGF_LE, 0x001F) /* 0x201F */
struct bt_hci_rp_le_test_end {
	uint8_t  status;
	uint16_t num_packets;
} __attribute__((packed));

#define BT_HCI_OP_LE_CONN_PARAM_REQ_REPLY       BT_OP(BT_OGF_LE, 0x0020) /* 0x2020 */
struct bt_hci_cp_le_conn_param_req_reply {
	uint16_t handle;
	uint16_t interval_min;
	uint16_t interval_max;
	uint16_t latency;
	uint16_t timeout;
	uint16_t min_ce_len;
	uint16_t max_ce_len;
} __attribute__((packed));
struct bt_hci_rp_le_conn_param_req_reply {
	uint8_t  status;
	uint16_t handle;
} __attribute__((packed));

#define BT_HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY   BT_OP(BT_OGF_LE, 0x0021) /* 0x2021 */
struct bt_hci_cp_le_conn_param_req_neg_reply {
	uint16_t handle;
	uint8_t  reason;
} __attribute__((packed));
struct bt_hci_rp_le_conn_param_req_neg_reply {
	uint8_t  status;
	uint16_t handle;
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_DATA_LENGTH            BT_OP(BT_OGF_LE, 0x0022) /* 0x2022 */
struct bt_hci_cp_le_set_data_length {
	uint16_t  handle;
	uint16_t  tx_octets;
	uint16_t  tx_time;
} __attribute__((packed));

struct bt_hci_rp_le_set_data_length {
	uint8_t  status;
	uint16_t handle;
} __attribute__((packed));

#define BT_HCI_OP_LE_READ_SUGGESTED_DATALEN      BT_OP(BT_OGF_LE, 0x0023) /* 0x2023 */
struct bt_hci_rp_le_read_suggested_data_len {
	uint8_t  status;
	uint16_t max_tx_octets;
	uint16_t max_tx_time;
} __attribute__((packed));

#define BT_HCI_OP_LE_WRITE_SUGGESTED_DATALEN     BT_OP(BT_OGF_LE, 0x0024) /* 0x2024 */
struct bt_hci_cp_le_write_suggested_data_len {
	uint16_t max_tx_octets;
	uint16_t max_tx_time;
} __attribute__((packed));

#define BT_HCI_OP_LE_ADD_DEV_TO_RL              BT_OP(BT_OGF_LE, 0x0027) /* 0x2027 */
struct bt_hci_cp_le_add_dev_to_rl {
	uint8_t  id_type;
	uint8_t  id_addr[BT_ADDR_SIZE];
	uint8_t  peer_irk[16];
	uint8_t  local_irk[16];
} __attribute__((packed));

#define BT_HCI_OP_LE_REM_DEV_FROM_RL            BT_OP(BT_OGF_LE, 0x0028) /* 0x2028 */
struct bt_hci_cp_le_remove_dev_from_rl {
	uint8_t  id_type;
	uint8_t  id_addr[BT_ADDR_SIZE];
} __attribute__((packed));

#define BT_HCI_OP_LE_CLEAR_RL                   BT_OP(BT_OGF_LE, 0x0029) /* 0x2029 */
struct bt_hci_rp_le_clear_rl {
	uint8_t  status;
} __attribute__((packed));

#define BT_HCI_OP_LE_READ_RL_SIZE               BT_OP(BT_OGF_LE, 0x002a) /* 0x202a */
struct bt_hci_rp_le_read_rl_size {
	uint8_t  status;
	uint8_t  rl_size;
} __attribute__((packed));

#define BT_HCI_OP_LE_READ_LOCAL_RPA             BT_OP(BT_OGF_LE, 0x002c) /* 0x202c */
struct bt_hci_cp_le_read_local_rpa {
	struct bt_le_addr_t  peer_id_addr;
} __attribute__((packed));
struct bt_hci_rp_le_read_local_rpa {
	uint8_t    status;
	uint8_t    local_rpa[BT_ADDR_SIZE];
} __attribute__((packed));

#define BT_HCI_ADDR_RES_DISABLE                 0x00
#define BT_HCI_ADDR_RES_ENABLE                  0x01

#define BT_HCI_OP_LE_SET_ADDR_RES_ENABLE        BT_OP(BT_OGF_LE, 0x002d) /* 0x202d */
struct bt_hci_cp_le_set_addr_res_enable {
	uint8_t enable;
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_RPA_TIMEOUT            BT_OP(BT_OGF_LE, 0x002e) /* 0x202e */
struct bt_hci_cp_le_set_rpa_timeout {
	uint16_t rpa_timeout;
} __attribute__((packed));

#define BT_HCI_OP_LE_READ_MAX_DATALEN           BT_OP(BT_OGF_LE, 0x002f) /* 0x002f */
struct bt_hci_rp_le_read_max_datalen {
	uint8_t status;
	uint16_t max_tx_octets;
	uint16_t max_tx_time;
	uint16_t max_rx_octets;
	uint16_t max_rx_time;
} __attribute__((packed));

enum {
	LE_PHY_OPTIONS_NO_PREFER = 0,
	LE_PHY_OPTIONS_S2,
	LE_PHY_OPTIONS_S8,
};

#define BT_HCI_OP_LE_READ_PHY                   BT_OP(BT_OGF_LE, 0x0030) /* 0x2030 */
struct bt_hci_cp_le_read_phy {
	uint16_t  handle;
} __attribute__((packed));

struct bt_hci_rp_le_read_phy {
	uint8_t status;
	uint16_t handle;
	uint8_t tx_phy;
	uint8_t rx_phy;
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_DEFAULT_PHY            BT_OP(BT_OGF_LE, 0x0031) /* 0x2031 */
struct bt_hci_cp_le_set_default_phy {
	uint8_t all_phys;
	uint8_t tx_phy;
	uint8_t rx_phy;
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_PHY                    BT_OP(BT_OGF_LE, 0x0032) /* 0x2032 */
struct bt_hci_cp_le_set_phy {
	uint16_t handle;
	uint8_t all_phys;
	uint8_t tx_phy;
	uint8_t rx_phy;
	uint8_t phy_options;
} __attribute__((packed));

#define BT_HCI_OP_LE_RECEIVER_TEST_V2                    BT_OP(BT_OGF_LE, 0x0033) /* 0x2033 */
struct bt_hci_cp_le_receiver_test_v2 {
	uint8_t  rx_channel;
	uint8_t  phy;
	uint8_t  modulation_index;
} __attribute__((packed));

#define BT_HCI_OP_LE_TRANSMITTER_TEST_V2                    BT_OP(BT_OGF_LE, 0x0034) /* 0x2034 */
struct bt_hci_cp_le_transmitter_test_v2 {
	uint8_t  tx_channel;
	uint8_t  test_data_length;
	uint8_t  packet_payload;
	uint8_t  phy;
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_ADV_SET_RANDOM_ADDR                BT_OP(BT_OGF_LE, 0x0035) /* 0x2035 */
struct bt_hci_cp_le_set_adv_set_random_addr {
	uint8_t handle;
	uint8_t random_addr[BT_ADDR_SIZE];
} __attribute__((packed));

#define BT_HCI_OP_LE_READ_MAX_ADV_DATA_LEN                  BT_OP(BT_OGF_LE, 0x003A) /* 0x203A */
struct bt_hci_rp_le_read_max_adv_data_len {
	uint8_t  status;
	uint16_t max_adv_data_len;
}  __attribute__((packed));

#define BT_HCI_OP_LE_READ_NUM_ADV_SETS                      BT_OP(BT_OGF_LE, 0x003B) /* 0x203B */
struct bt_hci_rp_le_read_num_adv_sets {
	uint8_t  status;
	uint8_t  num_sets;
} __attribute__((packed));

#define BT_HCI_OP_CLEAR_ADV_SETS                            BT_OP(BT_OGF_LE, 0x003D) /* 0x203D */

#define BT_HCI_OP_LE_SET_EXT_SCAN_PARAM         BT_OP(BT_OGF_LE, 0x0041) /* 0x2041 */

#define BT_HCI_LE_EXT_SCAN_PHY_1M               BIT(0)
#define BT_HCI_LE_EXT_SCAN_PHY_2M               BIT(1)
#define BT_HCI_LE_EXT_SCAN_PHY_CODED            BIT(2)

struct bt_hci_cp_le_set_ext_scan_param {
	uint8_t  own_addr_type;
	uint8_t  filter_policy;
	uint8_t  phys;
	struct bt_hci_scan_phy p[0];
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_EXT_SCAN_ENABLE         BT_OP(BT_OGF_LE, 0x0042) /* 0x2042 */

struct bt_hci_ext_scan_enable {
	uint8_t  enable;
	uint8_t filter_duplicates;
	uint16_t duration;
	uint16_t period;
} __attribute__((packed));

#define BT_HCI_OP_LE_EXT_CREATE_CONN            BT_OP(BT_OGF_LE, 0x0043) /* 0x2043 */
#define BT_HCI_OP_LE_EXT_CREATE_CONN_V2         BT_OP(BT_OGF_LE, 0x0085) /* 0x2085 */
struct bt_hci_ext_conn_param {
	uint16_t scan_interval;
	uint16_t scan_window;
	uint16_t conn_interval_min;
	uint16_t conn_interval_max;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint16_t min_ce_len;
	uint16_t max_ce_len;
} __attribute__((packed));

struct bt_hci_cp_le_ext_create_conn {
	uint8_t      filter_policy;
	uint8_t      own_addr_type;
	uint8_t      peer_addr_type;
	uint8_t      peer_addr[6];
	uint8_t      init_phys;
	struct bt_hci_ext_conn_param phy_param[0];
} __attribute__((packed));

struct bt_hci_cp_le_ext_create_conn_v2 {
	uint8_t      adv_handle;
	uint8_t      subevent;
	uint8_t      filter_policy;
	uint8_t      own_addr_type;
	uint8_t      peer_addr_type;
	uint8_t      peer_addr[6];
	uint8_t      init_phys;
	struct bt_hci_ext_conn_param phy_param[0];
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_EXT_ADV_PARAM_V1       BT_OP(BT_OGF_LE, 0x0036) /* 0x2036 */
struct bt_hci_cp_le_set_ext_adv_param_v1 {
	uint8_t      handle;
	uint16_t     props;
	uint8_t      prim_min_interval[3];
	uint8_t      prim_max_interval[3];
	uint8_t      prim_channel_map;
	uint8_t      own_addr_type;
	struct bt_le_addr_t peer_addr;
	uint8_t      filter_policy;
	int8_t       tx_power;
	uint8_t      prim_adv_phy;
	uint8_t      sec_adv_max_skip;
	uint8_t      sec_adv_phy;
	uint8_t      sid;
	uint8_t      scan_req_notify_enable;
} __attribute__((packed));

struct bt_hci_rp_le_set_ext_adv_param {
	uint8_t status;
	int8_t  tx_power;
} __packed;

#define BT_HCI_LE_ADV_PHY_OPTION_NO_REQUIRED    0x00
#define BT_HCI_LE_ADV_PHY_OPTION_REQUIRE_S2     0x03
#define BT_HCI_LE_ADV_PHY_OPTION_REQUIRE_S8     0x04

#define BT_HCI_LE_EXT_ADV_OP_INTERM_FRAG        0x00
#define BT_HCI_LE_EXT_ADV_OP_FIRST_FRAG         0x01
#define BT_HCI_LE_EXT_ADV_OP_LAST_FRAG          0x02
#define BT_HCI_LE_EXT_ADV_OP_COMPLETE_DATA      0x03
#define BT_HCI_LE_EXT_ADV_OP_UNCHANGED_DATA     0x04

#define BT_HCI_LE_EXT_ADV_FRAG_ENABLED          0x00
#define BT_HCI_LE_EXT_ADV_FRAG_DISABLED         0x01

#define BT_HCI_OP_LE_SET_EXT_ADV_DATA           BT_OP(BT_OGF_LE, 0x0037) /* 0x2037 */
struct bt_hci_cp_le_set_ext_adv_data {
	uint8_t  handle;
	uint8_t  op;
	uint8_t  frag_pref;
	uint8_t  len;
	uint8_t  data[0];
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_EXT_SCAN_RSP_DATA      BT_OP(BT_OGF_LE, 0x0038) /* 0x2038 */
struct bt_hci_cp_le_set_ext_scan_rsp_data {
	uint8_t  handle;
	uint8_t  op;
	uint8_t  frag_pref;
	uint8_t  len;
	uint8_t  data[0];
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_EXT_ADV_ENABLE         BT_OP(BT_OGF_LE, 0x0039) /* 0x2039 */
struct bt_hci_ext_adv_set {
	uint8_t  handle;
	uint16_t duration;
	uint8_t  max_ext_adv_evts;
} __attribute__((packed));

struct bt_hci_cp_le_set_ext_adv_enable {
	uint8_t  enable;
	uint8_t  set_num;
	struct bt_hci_ext_adv_set s[0];
} __attribute__((packed));

#define BT_HCI_OP_LE_REMOVE_ADV_SET           BT_OP(BT_OGF_LE, 0x003C) /* 0x203C */
struct bt_hci_cp_le_remove_adv_set {
	uint8_t  handle;
} __attribute__((packed));

/* LE Periodic Advertising HCI Commands (BT 5.0) */
#define BT_HCI_LE_PA_PROP_TX_POWER              BIT(6)
#define BT_HCI_OP_LE_SET_PERIODIC_ADV_PARAM_V1  BT_OP(BT_OGF_LE, 0x003E) /* 0x203E */
struct bt_hci_cp_le_set_periodic_adv_param_v1 {
	uint8_t  handle;
	uint16_t min_interval;
	uint16_t max_interval;
	uint16_t props;
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_PERIODIC_ADV_DATA      BT_OP(BT_OGF_LE, 0x003F) /* 0x203F */
struct bt_hci_cp_le_set_periodic_adv_data {
	uint8_t  handle;
	uint8_t  op;
	uint8_t  len;
	uint8_t  data[0];
} __attribute__((packed));

#define BT_HCI_OP_LE_SET_PERIODIC_ADV_ENABLE    BT_OP(BT_OGF_LE, 0x0040) /* 0x2040 */
struct bt_hci_cp_le_set_periodic_adv_enable {
	uint8_t  enable;
	uint8_t  handle;
} __attribute__((packed));

#define BT_HCI_OP_LE_PERIODIC_ADV_CREATE_SYNC   BT_OP(BT_OGF_LE, 0x0044) /* 0x2044 */
struct bt_hci_cp_le_periodic_adv_create_sync {
	uint8_t  options;
	uint8_t  sid;
	uint8_t  adv_addr_type;
	uint8_t  adv_addr[BT_ADDR_SIZE];
	uint16_t skip;
	uint16_t sync_timeout;
	uint8_t  sync_cte_type;
} __attribute__((packed));

#define BT_HCI_OP_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL  BT_OP(BT_OGF_LE, 0x0045) /* 0x2045 */

#define BT_HCI_OP_LE_PERIODIC_ADV_TERMINATE_SYNC  BT_OP(BT_OGF_LE, 0x0046) /* 0x2046 */
struct bt_hci_cp_le_periodic_adv_terminate_sync {
	uint16_t sync_handle;
} __attribute__((packed));

/* Periodic Advertiser List management (Core Spec v5.4 Vol 4, Part E, 7.8.68-7.8.71) */
#define BT_HCI_OP_LE_ADD_PERIODIC_ADV_LIST         BT_OP(BT_OGF_LE, 0x0047) /* 0x2047 */
#define BT_HCI_OP_LE_REMOVE_PERIODIC_ADV_LIST      BT_OP(BT_OGF_LE, 0x0048) /* 0x2048 */
#define BT_HCI_OP_LE_CLEAR_PERIODIC_ADV_LIST       BT_OP(BT_OGF_LE, 0x0049) /* 0x2049 */
#define BT_HCI_OP_LE_READ_PERIODIC_ADV_LIST_SIZE   BT_OP(BT_OGF_LE, 0x004A) /* 0x204A */

struct bt_hci_cp_le_periodic_adv_list_entry {
	uint8_t  adv_addr_type;
	uint8_t  adv_addr[BT_ADDR_SIZE];
	uint8_t  sid;
} __attribute__((packed));

struct bt_hci_rp_le_read_periodic_adv_list_size {
	uint8_t  status;
	uint8_t  list_size;
} __attribute__((packed));


#define BT_HCI_LE_PRIVACY_MODE_NETWORK          0x00
#define BT_HCI_LE_PRIVACY_MODE_DEVICE           0x01

#define BT_HCI_OP_LE_SET_PRIVACY_MODE           BT_OP(BT_OGF_LE, 0x004e) /* 0x204e */
struct bt_hci_cp_le_set_privacy_mode {
	uint8_t  id_type;
	uint8_t  id_addr[BT_ADDR_SIZE];
	uint8_t mode;
} __attribute__((packed));

//Vendor HCI Command
#define BT_HCI_OP_VENDOR_READ_VENDOR_REG        BT_OP(BT_OGF_VENDOR, 0x0061) /*0xFC61*/
struct bt_hci_cp_vendor_read_vendor_reg {
	uint8_t type;
	uint32_t addr;
};

struct bt_hci_rp_vendor_read_vendor_reg {
	uint8_t status;
	uint32_t rdata;
};

#define BT_HCI_OP_VENDOR_WRITE_VENDOR_REG           BT_OP(BT_OGF_VENDOR, 0x62) /*0xFC62*/
struct bt_hci_cp_vendor_write_vendor_reg {
	uint8_t type;
	uint32_t addr;
	uint8_t wdata[0];
};

#define BT_HCI_OP_VENDOR_GET_ECO_VERSION           BT_OP(BT_OGF_VENDOR, 0x6D) /*0xFC6D*/
struct bt_hci_rp_vendor_get_eco_version {
	uint8_t status;
	uint8_t eco_version;
};

#define BT_HCI_OP_VENDOR_MP                         BT_OP(BT_OGF_VENDOR, 0xEB) /*0xFCEB*/

#define BT_CONTROLLER_MP_MODULE_PLATFORM            0x00
#define BT_CONTROLLER_MP_MODULE_PHY                 0x05
#define BT_CONTROLLER_MP_MODULE_XTAL                0x06
#define BT_CONTROLLER_MP_MODULE_BT_MAC              0x80
#define BT_CONTROLLER_MP_MODULE_BT_LEGACY           0x81
#define BT_CONTROLLER_MP_MODULE_BT_LE               0x82
#define BT_CONTROLLER_MP_MODULE_ZIGBEE              0x83

#define MODULE_PLATFORM_MP_RESET                    0x00
#define MODULE_PLATFORM_MP_WDG_RESET                0x01
#define MODULE_PLATFORM_MP_CMD_VERSION              0x03
#define MODULE_PHY_TX_POWER                         0x00
#define MODULE_PHY_THERMAL_DEFAULT                  0x02
#define MODULE_PHY_TX_POWER_TRACKING_ENABLE         0x03
#define MODULE_PHY_TXGAINK                          0x04
#define MODULE_PHY_TXGAIN_FLATK                     0x05
#define MODULE_PHY_PATH_LOSS                        0x06
#define MODULE_PHY_SINGLE_TONE                      0x07
#define MODULE_PHY_CONT_TX                          0x08
#define MODULE_PHY_PACKET_TX                        0x09
#define MODULE_PHY_PACKET_RX                        0x0A
#define MODULE_PHY_PACKET_TX_REPORT                 0x0B
#define MODULE_PHY_PACKET_RX_REPORT                 0x0C
#define MODULE_PHY_MODEM_DFT                        0x0D
#define MODULE_PHY_PHY_VERSION                      0x0E
#define MODULE_PHY_TXGAINK_AND_FLATK                0x0F
#define MODULE_PHY_CHANGE_CHANNEL                   0x10
#define MODULE_PHY_LBT                              0x14
#define MODULE_PHY_CONT_RX                          0x15
#define MODULE_PHY_RSSI                             0x17
#define MODULE_PHY_IQK_LOK                          0x18
#define MODULE_PHY_SWITCH_ANTENNA                   0x19
#define MODULE_XTAL_XTAL_VALUE                      0x00
#define MODULE_MAC_MP_RESET                         0x00
#define MODULE_MAC_SYNC_WORD_TRANS                  0x03
#define MODULE_LEGACY_HOPPING_TX                    0x00
#define MODULE_LEGACY_HOPPING_RX                    0x01
#define MODULE_LE_HOPPING_TX                        0x00
#define MODULE_ZIGBEE_MAC_INIT                      0x00
#define MODULE_ZIGBEE_MAC_RESET                     0x01
#define MODULE_ZIGBEE_MP_TX                         0x02
#define MODULE_ZIGBEE_MP_CONT_TX                    0x03
#define MODULE_ZIGBEE_TX_REPORT                     0x04
#define MODULE_ZIGBEE_MP_RX                         0x05
#define MODULE_ZIGBEE_RX_REPORT                     0x06
#define MODULE_ZIGBEE_LOOPBACK_TEST                 0x07
#define MODULE_ZIGBEE_ED_GAIN_CAL                   0x08
#define MODULE_ZIGBEE_REG_READ                      0x09
#define MODULE_ZIGBEE_REG_WRITE                     0x0A
#define MODULE_ZIGBEE_PHY_TX_01                     0x0B
#define MODULE_ZIGBEE_PHY_RX_06                     0x0C
#define MODULE_ZIGBEE_PHY_RX_08                     0x0D
#define MODULE_ZIGBEE_PHY_RX_TO_TX                      0x0E
#define MODULE_ZIGBEE_PHY_TX_TO_RX                      0x0F

enum bt_hci_vendor_tx_power_index {
	BT_HCI_VENDOR_TX_POWER_BR_1M,
	BT_HCI_VENDOR_TX_POWER_EDR_2M,
	BT_HCI_VENDOR_TX_POWER_EDR_3M,
	BT_HCI_VENDOR_TX_POWER_LE_1M,
	BT_HCI_VENDOR_TX_POWER_LE_2M,
	BT_HCI_VENDOR_TX_POWER_LE_1M_ADV,
	BT_HCI_VENDOR_TX_POWER_LE_2M_ADV,
	BT_HCI_VENDOR_TX_POWER_LE_1M_2402,
	BT_HCI_VENDOR_TX_POWER_LE_1M_2480,
	BT_HCI_VENDOR_TX_POWER_LE_2M_2402,
	BT_HCI_VENDOR_TX_POWER_LE_2M_2480,
	BT_HCI_VENDOR_TX_POWER_ZB,
	BT_HCI_VENDOR_TX_POWER_MAX,
};

enum bt_hci_vendor_mp_packet_type {
	BT_HCI_VENDOR_MP_PACKET_TYPE_1DH1 = 0x00,
	BT_HCI_VENDOR_MP_PACKET_TYPE_1DH3 = 0x01,
	BT_HCI_VENDOR_MP_PACKET_TYPE_1DH5 = 0x02,
	BT_HCI_VENDOR_MP_PACKET_TYPE_2DH1 = 0x03,
	BT_HCI_VENDOR_MP_PACKET_TYPE_2DH3 = 0x04,
	BT_HCI_VENDOR_MP_PACKET_TYPE_2DH5 = 0x05,
	BT_HCI_VENDOR_MP_PACKET_TYPE_3DH1 = 0x06,
	BT_HCI_VENDOR_MP_PACKET_TYPE_3DH3 = 0x07,
	BT_HCI_VENDOR_MP_PACKET_TYPE_3DH5 = 0x08,
	BT_HCI_VENDOR_MP_PACKET_TYPE_NULL = 0x0A,
};

struct bt_hci_cp_vendor_mp {
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t sub_parameter[0];
};

enum bt_hci_vendor_mp_payload_pattern {
	BT_HCI_VENDOR_MP_PAYLOAD_PATTERN_ALL_0 = 0x00,
	BT_HCI_VENDOR_MP_PAYLOAD_PATTERN_ALL_1 = 0x01,
	BT_HCI_VENDOR_MP_PAYLOAD_PATTERN_REPEAT_01010101 = 0x02,
	BT_HCI_VENDOR_MP_PAYLOAD_PATTERN_REPEAT_10101010 = 0x03,
	BT_HCI_VENDOR_MP_PAYLOAD_PATTERN_REPEAT_0_TO_F = 0x04,
	BT_HCI_VENDOR_MP_PAYLOAD_PATTERN_REPEAT_00001111 = 0x05,
	BT_HCI_VENDOR_MP_PAYLOAD_PATTERN_REPEAT_11110000 = 0x06,
	BT_HCI_VENDOR_MP_PAYLOAD_PATTERN_PRBS9 = 0x07,
};

struct bt_hci_rp_vendor_mp {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
};

struct bt_hci_rp_vendor_mp_mp_cmd_version {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t version;
};

struct bt_hci_rp_vendor_mp_get_tx_power {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t max_tx_power_br_1M;
	uint8_t max_tx_power_edr_2M;
	uint8_t max_tx_power_edr_3M;
	uint8_t max_tx_power_le_1M;
	uint8_t max_tx_power_le_2M;
};

struct bt_hci_rp_vendor_mp_get_thermal_default {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t thermal_default;
};

struct bt_hci_rp_vendor_mp_get_tx_power_track_enable {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t txpower_track_enable;
};

struct bt_hci_rp_vendor_mp_get_txgaink {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	int8_t txgaink;
};

struct bt_hci_rp_vendor_mp_get_txgain_flatk {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	int8_t txgain_flatk[4];
};

struct bt_hci_rp_vendor_mp_get_path_loss {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t path_loss;
};

struct bt_hci_rp_vendor_mp_modem_dft {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t dft_result[0];
};

struct bt_hci_rp_vendor_mp_packet_tx_report {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint32_t total_tx_packet;
};

struct bt_hci_rp_vendor_mp_packet_rx_report {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint32_t rx_packet;
	uint32_t rx_bits;
	uint32_t rx_error_bits;
	uint16_t ber;
	uint16_t cfo;
	uint8_t rx_rssi;
};

struct bt_hci_rp_vendor_mp_phy_version {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t phy_version;
};

struct bt_hci_rp_vendor_mp_get_txgaink_and_flatk {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	int8_t txgaink;
	int8_t txgain_flatk[4];
};

struct bt_hci_rp_vendor_mp_get_lbt {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t lbt_enable;
	uint8_t lbt_mode;
	uint8_t lbt_threshold_ant_out;
	uint8_t lbt_ant_gain;
};

struct bt_hci_rp_vendor_mp_get_xtal_value {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t xtal_cap;
};

struct bt_hci_rp_vendor_mp_sync_word_trans {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t sync_word[8];
};

struct bt_hci_rp_vendor_mp_zigbee_mp_tx {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t tx_state;
};

struct bt_hci_rp_vendor_mp_zigbee_tx_report {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t tx_state;
	uint32_t tx_pkt_cnt;
};

struct bt_hci_rp_vendor_mp_zigbee_mp_rx {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t rx_state;
};

struct bt_hci_rp_vendor_mp_zigbee_rx_report {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t rx_state;
	uint32_t rx_ok_cnt;
	uint32_t rx_err_cnt;
	int8_t rssi;
	uint8_t lqi;
};

struct bt_hci_rp_vendor_mp_zigbee_loopback_test {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	int8_t tx_gain_offset;
};

struct bt_hci_rp_vendor_mp_zigbee_ed_gain_cal {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	int8_t gain_offset;
};

struct bt_hci_rp_vendor_mp_zigbee_reg_read {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t reg_value[0];
};

struct bt_hci_rp_vendor_mp_zigbee_phy_rx_06 {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t ed_scan_state;
	int8_t ed_value;
};

struct bt_hci_rp_vendor_mp_zigbee_phy_rx_08 {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t cca_idle_count;
	uint8_t cca_busy_count;
};

struct bt_hci_rp_vendor_mp_zigbee_phy_tx_to_rx {
	uint8_t status;
	uint8_t module_id;
	uint8_t subcmd;
	uint8_t tx_status;
	uint8_t tx_retry_count;
};

#define BT_HCI_OP_VENDOR_READ_RADIO_REG_PI          BT_OP(BT_OGF_VENDOR, 0x14B) /*0xFD4B*/
struct bt_hci_cp_vendor_read_radio_reg_pi {
	uint8_t offset;
	uint8_t page;
};

struct bt_hci_rp_vendor_read_radio_reg_pi {
	uint8_t status;
	uint8_t rdata[3];
};

#define BT_HCI_OP_VENDOR_WRITE_RADIO_REG_PI         BT_OP(BT_OGF_VENDOR, 0x14C) /*0xFD4C*/
struct bt_hci_cp_vendor_write_radio_reg_pi {
	uint8_t offset;
	uint8_t page;
	uint16_t wdata;
};

#define BT_HCI_OP_VENDOR_MP_PHY_CMD             BT_OP(BT_OGF_VENDOR, 0x187) /*0xFD87*/
#define HCI_VENDOR_MP_PHY_REG_CMD_VERSION       0x00
#define HCI_VENDOR_MP_PHY_REG_CMD               0x03
#define HCI_VENDOR_MP_READ_THERMOMETER          0x05

struct bt_hci_cp_vendor_phy {
	uint8_t subcmd;
	uint8_t sub_parameter[0];
};

struct bt_hci_rp_vendor_mp_phy_reg_cmd_version {
	uint8_t version;
	uint8_t phy_id;
};

struct bt_hci_rp_vendor_mp_phy_write_reg {
	uint8_t status;
	uint8_t subcmd;
};

struct bt_hci_rp_vendor_mp_phy_read_reg {
	uint8_t status;
	uint8_t subcmd;
	uint16_t value;
};

struct bt_hci_rp_vendor_mp_read_thermometer {
	uint8_t status;
	uint8_t thermometer_bt0;
	uint8_t thermal_now_bt0;
	uint8_t thermal_celsius_bt0;
	uint8_t thermometer_bt1;
	uint8_t thermal_now_b1;
	uint8_t thermal_celsius_bt1;
};

#define BT_HCI_OP_VENDOR_READ_RTK_CHIP_ID           BT_OP(BT_OGF_VENDOR, 0x006F) /*0xFC6F*/
struct bt_hci_rp_vendor_read_rtk_chip_id_info {
	uint8_t status;
	uint8_t chip_id;
};



//HCI EVENT
#define BT_HCI_EVT_DISCONNECT_COMPLETE             0x05
struct bt_hci_evt_disconnect_complete {
	uint8_t  status;
	uint16_t handle;
	uint8_t  reason;
} __attribute__((packed));

#define BT_HCI_EVT_ENC_ENABLED_OFF                       0x00
#define BT_HCI_EVT_ENC_ENABLED_ON_E0_BREDR_AESCCM_LE     0x01
#define BT_HCI_EVT_ENC_ENABLED_ON_AESCCM_BREDR           0x02

#define BT_HCI_EVT_ENCRYPTION_CHANGE            0x08
struct bt_hci_evt_encryption_change {
	uint8_t  status;
	uint16_t handle;
	uint8_t  enc_enabled;
} __attribute__((packed));

#define BT_HCI_EVT_CMD_COMPLETE                 0x0E
struct bt_hci_evt_cmd_complete {
	uint8_t  ncmd;
	uint16_t opcode;
} __attribute__((packed));

struct bt_hci_evt_cc_status {
	uint8_t  status;
} __attribute__((packed));

#define BT_HCI_EVT_CMD_STATUS                   0x0F
struct bt_hci_evt_cmd_status {
	uint8_t  status;
	uint8_t  ncmd;
	uint16_t opcode;
} __attribute__((packed));

struct bt_hci_handle_count {
	uint16_t handle;
	uint16_t count;
} __attribute__((packed));

#define BT_HCI_EVT_NUM_COMPLETED_PACKETS        0x13
struct bt_hci_evt_num_completed_packets {
	uint8_t  num_handles;
	struct   bt_hci_handle_count hc[0];
} __attribute__((packed));

#define BT_HCI_EVT_ENCRYPTION_KEY_REFRESH       0x30
struct bt_hci_evt_encryption_key_refresh {
	uint8_t  status;
	uint16_t handle;
} __attribute__((packed));

#define BT_HCI_EVT_LE_META_EVENT                0x3E

#define BT_HCI_EVT_LE_CONNECTION_COMPLETE             0x01
struct bt_hci_evt_le_connnection_complete {
	uint8_t subevent;
	uint8_t status;
	uint16_t handle;
	uint8_t role;
	struct bt_le_addr_t peer_addr;
	uint16_t interval;
	uint16_t latency;
	uint16_t supversion_timeout;
	uint8_t clock_accuracy;
} __attribute__((packed));

#define BT_HCI_EVT_LE_ADVERTISING_REPORT        0x02
struct bt_hci_evt_le_advertising_info {
	uint8_t evt_type;
	struct bt_le_addr_t addr;
	uint8_t length;
	uint8_t data[0];
} __attribute__((packed));
struct bt_hci_evt_le_advertising_report {
	uint8_t subevent;
	uint8_t num_reports;
	struct bt_hci_evt_le_advertising_info adv_info[0];
} __attribute__((packed));

#define BT_HCI_EVT_LE_CONN_UPDATE_COMPLETE             0x03
struct bt_hci_evt_le_conn_update_complete {
	uint8_t subevent;
	uint8_t status;
	uint16_t handle;
	uint16_t conn_interval;
	uint16_t latency;
	uint16_t supervision_timeout;
} __attribute__((packed));

#define BT_HCI_EVT_LE_READ_REMOTE_FEATURES_COMPLETE             0x04
struct bt_hci_evt_le_read_remote_features_complete {
	uint8_t subevent;
	uint8_t status;
	uint16_t handle;
	uint8_t features[8];
} __attribute__((packed));

#define BT_HCI_EVT_LE_LTK_REQUEST                      0x05
struct bt_hci_evt_le_ltk_request {
	uint8_t subevent;
	uint16_t handle;
	uint8_t rand[8];
	uint8_t enc_div[2];
} __attribute__((packed));

#define BT_HCI_EVT_LE_CONN_PARAM_REQ                   0x06
struct bt_hci_evt_le_conn_param_req {
	uint8_t subevent;
	uint16_t handle;
	uint16_t interval_min;
	uint16_t interval_max;
	uint16_t latency;
	uint16_t timeout;
} __attribute__((packed));

#define BT_HCI_EVT_LE_DATA_LENGTH_CHANGE               0x07
struct bt_hci_evt_le_data_len_change {
	uint8_t subevent;
	uint16_t handle;
	uint16_t max_tx_octets;
	uint16_t max_tx_time;
	uint16_t max_rx_octets;
	uint16_t max_rx_time;
} __attribute__((packed));

#define BT_HCI_EVT_LE_ENHANCED_CONNECTION_COMPLETE         0x0A
struct bt_hci_evt_le_enhanced_connection_complete {
	uint8_t subevent;
	uint8_t status;
	uint16_t handle;
	uint8_t role;
	struct bt_le_addr_t peer_addr;
	uint8_t local_rpa[BT_ADDR_SIZE];
	uint8_t peer_rpa[BT_ADDR_SIZE];
	uint16_t interval;
	uint16_t latency;
	uint16_t supversion_timeout;
	uint8_t clock_accuracy;
} __attribute__((packed));

#define BT_HCI_EVT_LE_DIRECT_ADV_REPORT         0x0B
struct bt_hci_evt_le_direct_adv_info {
	uint8_t subevent;
	uint8_t evt_type;
	struct bt_le_addr_t addr;
	struct bt_le_addr_t dir_addr;
	int8_t rssi;
} __attribute__((packed));

struct bt_hci_evt_le_direct_adv_report {
	uint8_t subevent;
	uint8_t num_reports;
	struct bt_hci_evt_le_direct_adv_info direct_adv_info[0];
} __attribute__((packed));

#define BT_HCI_EVT_LE_PHY_UPDATE_COMPLETE                  0x0C
struct bt_hci_evt_le_phy_update_complete {
	uint8_t subevent;
	uint8_t status;
	uint16_t handle;
	uint8_t tx_phy;
	uint8_t rx_phy;
} __attribute__((packed));

#define BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT    0x0D

/* LE Periodic Advertising HCI Events */
#define BT_HCI_EVT_LE_PA_SYNC_ESTABLISHED       0x0E
struct bt_hci_evt_le_pa_sync_established {
	uint8_t  subevent;
	uint8_t  status;
	uint16_t sync_handle;
	uint8_t  sid;
	struct bt_le_addr_t adv_addr;
	uint8_t  adv_phy;
	uint16_t pa_interval;
	uint8_t  adv_clk_accuracy;
} __attribute__((packed));

#define BT_HCI_EVT_LE_PA_REPORT                 0x0F
struct bt_hci_evt_le_pa_report {
	uint8_t  subevent;
	uint16_t sync_handle;
	uint8_t  tx_power;
	int8_t   rssi;
	uint8_t  cte_type;
	uint8_t  data_status;
	uint8_t  data_len;
	uint8_t  data[0];
} __attribute__((packed));

#define BT_HCI_EVT_LE_PA_SYNC_LOST              0x10
struct bt_hci_evt_le_pa_sync_lost {
	uint8_t  subevent;
	uint16_t sync_handle;
} __attribute__((packed));


#define BT_HCI_EVT_LE_ADVERTISING_SET_TERMINATED    0x12
struct bt_hci_evt_le_adv_set_terminated {
	uint8_t subevent;
	uint8_t status;
	uint8_t adv_handle;
	uint16_t conn_handle;
	uint8_t num_cmpl_ext_adv_evts;
} __attribute__((packed));

#define BT_HCI_LE_ADV_EVT_TYPE_CONN                 BIT(0)
#define BT_HCI_LE_ADV_EVT_TYPE_SCAN                 BIT(1)
#define BT_HCI_LE_ADV_EVT_TYPE_DIRECT               BIT(2)
#define BT_HCI_LE_ADV_EVT_TYPE_SCAN_RSP             BIT(3)
#define BT_HCI_LE_ADV_EVT_TYPE_LEGACY               BIT(4)

#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS(ev_type) (((ev_type) >> 5) & 0x03)
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_COMPLETE   0
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_PARTIAL    1
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_INCOMPLETE 2
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_RX_FAILED  0xFF

/* Advertising Coding Selection extended advertising report PHY values.
 * Only used when Kconfig BT_EXT_ADV_CODING_SELECTION is enabled.
 */
#define BT_HCI_LE_ADV_EVT_PHY_1M                0x01
#define BT_HCI_LE_ADV_EVT_PHY_2M                0x02
#define BT_HCI_LE_ADV_EVT_PHY_CODED_S8          0x03
#define BT_HCI_LE_ADV_EVT_PHY_CODED_S2          0x04

#define BT_HCI_LE_ADV_TX_POWER_NO_PREF          0x7F

struct bt_hci_evt_le_ext_advertising_info {
	uint16_t     evt_type;
	struct bt_le_addr_t addr;
	uint8_t      prim_phy;
	uint8_t      sec_phy;
	uint8_t      sid;
	int8_t       tx_power;
	int8_t       rssi;
	uint16_t     interval;
	struct bt_le_addr_t direct_addr;
	uint8_t      length;
	uint8_t      data[0];
} __attribute__((packed));
struct bt_hci_evt_le_ext_advertising_report {
	uint8_t subevent;
	uint8_t num_reports;
	struct bt_hci_evt_le_ext_advertising_info adv_info[0];
} __attribute__((packed));

#endif
