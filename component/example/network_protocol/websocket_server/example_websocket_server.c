#include "lwip_netconf.h"
#include "websocket/ws_server_msg.h"

#define USE_WSS    0

#if USE_WSS
static const char *mbedtls_test_srv_crt =
	"-----BEGIN CERTIFICATE-----\r\n" \
	"MIIDozCCAosCFBgRPzXVfYv/sHMIS+k+Eni4zySPMA0GCSqGSIb3DQEBCwUAMIGN\r\n" \
	"MQswCQYDVQQGEwJDTjEQMA4GA1UECAwHSmlhbmdTdTEPMA0GA1UEBwwGU3VaaG91\r\n" \
	"MRAwDgYDVQQKDAdSZWFsc2lsMQ8wDQYDVQQLDAZzZXJ2ZXIxFDASBgNVBAMMC3Nl\r\n" \
	"cnZlcl90ZXN0MSIwIAYJKoZIhvcNAQkBFhNzZXJ2ZXJfdGVzdEAxMjMuY29tMB4X\r\n" \
	"DTIzMDYwNjA3MTI0M1oXDTMzMDYwMzA3MTI0M1owgY0xCzAJBgNVBAYTAkNOMRAw\r\n" \
	"DgYDVQQIDAdKaWFuZ1N1MQ8wDQYDVQQHDAZTdVpob3UxEDAOBgNVBAoMB1JlYWxz\r\n" \
	"aWwxDzANBgNVBAsMBnNlcnZlcjEUMBIGA1UEAwwLc2VydmVyX3Rlc3QxIjAgBgkq\r\n" \
	"hkiG9w0BCQEWE3NlcnZlcl90ZXN0QDEyMy5jb20wggEiMA0GCSqGSIb3DQEBAQUA\r\n" \
	"A4IBDwAwggEKAoIBAQCvaMzdXDPJm/O1jIpwh5NIL3aKZCKtZlVqkGEakzdJoomR\r\n" \
	"J2loYtxcwCtdCDPazskEG9O3qa7SVpgKYOzdtyFz/ETvwB5pG88tmpquibvUw6Ef\r\n" \
	"9fq1e3pXZagFbbqljL9N/RQnyjrZ96YLMY5UIdqunRvZFNeI3PoagDcrhiDGHomh\r\n" \
	"891x+UURLKVnvgpQkzwvbV8LuY60L+0SVSjlpXD9G/4pZqOrze5Up3+7tXjyGmZT\r\n" \
	"FdQ8VaXrYolRDcV4+mpsM9BRExcJCZmwEIhzFIYJyg9J2HVKUeJppBHYDUom2wKa\r\n" \
	"c0QsLZB8ZdfUSBeuRXERSBXbibKHMnbmr+DP1SNnAgMBAAEwDQYJKoZIhvcNAQEL\r\n" \
	"BQADggEBAJdmTyHbLlhrGG3vCjw6pVHxokalYSTqdXug7z1MqG80dx6KNFqoi98/\r\n" \
	"Yh049JFgtwbv94ioMm80BF+i12xGGkpFJlhUyFZUU1cBgw1FpR/Kby4il+Klajr8\r\n" \
	"ygP0wOe8vlEPUXosAq6sHjrvfhQR+3W2T5cqtujoSCh0wlgHcgyTDMZUA7cP83CQ\r\n" \
	"SeTjNEPLQhYOYRfqux3SO4Wt6NXyptS6XouxnGRODbsdFJxoTtJgQubpozeDyV+/\r\n" \
	"3YDkEMQkwqe4d3LaoBWVdYcVyEl5s/XLdcum3SL2BHNZo7cnIZmQve50YyIHUrHa\r\n" \
	"lNdugeECgJtLVjRFOwryPz8Yzw1NrVc=\r\n" \
	"-----END CERTIFICATE-----\r\n";


static const char *mbedtls_test_srv_key =
	"-----BEGIN PRIVATE KEY-----\r\n" \
	"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCvaMzdXDPJm/O1\r\n" \
	"jIpwh5NIL3aKZCKtZlVqkGEakzdJoomRJ2loYtxcwCtdCDPazskEG9O3qa7SVpgK\r\n" \
	"YOzdtyFz/ETvwB5pG88tmpquibvUw6Ef9fq1e3pXZagFbbqljL9N/RQnyjrZ96YL\r\n" \
	"MY5UIdqunRvZFNeI3PoagDcrhiDGHomh891x+UURLKVnvgpQkzwvbV8LuY60L+0S\r\n" \
	"VSjlpXD9G/4pZqOrze5Up3+7tXjyGmZTFdQ8VaXrYolRDcV4+mpsM9BRExcJCZmw\r\n" \
	"EIhzFIYJyg9J2HVKUeJppBHYDUom2wKac0QsLZB8ZdfUSBeuRXERSBXbibKHMnbm\r\n" \
	"r+DP1SNnAgMBAAECggEAPXpMLxtFNViY4p0G2mjSCH9GGzv8YWzX7Bj9R5yoSIEU\r\n" \
	"mRDIo2Le3A0oRZt93ScTkgKUwi7RDHx4Hk8ZRZ4NNgMJQEREBE/QXanto9W3RNtp\r\n" \
	"6fNC8v97gg0phfmO7L33QMS+HzIETWe8RPkR6s/QvtlmrL9eyWO1rrsYWwNQEk+E\r\n" \
	"hou5a+vnTSlA1Row4nDtpVbpYaVRFChabtdaBe0hPPYZ8RYnKnM8byh0hHwnPPwB\r\n" \
	"PUodfnlm9l0TssZ/gDP0cAvdO6ENDapQ26AGEcQr3StKX6RQQeIv8oBa5nF1OOMH\r\n" \
	"O5klGRHZiAszIsp4x0jJYTZuBMxNfTaxn7yl0Eyf1QKBgQC3AzpGo6Q2lq8MehWH\r\n" \
	"92bNWLf48J1bPtL4PXvN+NOVvK2HWOgz2bxT6a5jp7q6DgkSFsCuK8PGZbsMDcer\r\n" \
	"wy4F/qniya4iDdVR4w3u6o5ADHv9AP/ArckmsT9qPjbo1zsUqmQ9H3FBA0UQw105\r\n" \
	"7wxT48SS4tbkIpg0Lgv9j5Id8wKBgQD1XVEE/GcnZgFOVzKU8VGoMc8WCyorRceF\r\n" \
	"NFJD+sQrFXaBqBjVjvsycJSSg2m6K1r38zR05NIIAQ26r4q1EQ5QjlMbZDphVScz\r\n" \
	"iK4fVuoa7oTk7gtbJva/5F5rh8zr65Owz3XXBzgjYkWu15GMNKpCJBH9TH7pMQPR\r\n" \
	"moodI82dvQKBgQCorQXiI2LvmTQfaMo9de2/J5zwiTxO4/loKKrZ7RN+N9csHupt\r\n" \
	"F6p3ILLFUMtOPi9oB6ZxIQ8g5ZRfCLhWe98IUW5dAwtGgb9x6LZn1LFlINyKFIik\r\n" \
	"GsiBq+3JyyuqsOzYIC+hEkC87fmS+K0M/2E6VHxR8VakD6p+tU4B1niStwKBgDvg\r\n" \
	"Vtopoh6qNN1iSBSywyORbCDL/rFqqOLnvr+aHW8KIM5JMC5FYyWO9aK/cPieheeG\r\n" \
	"MSHpiRus7iXFmsrO67ha0wsXLWv9NMDNtPgu5JhH5qRbNug6nPJ9D1GcZTSZW4RF\r\n" \
	"TFN6DogL/kdJkdouK/JSDRXMKhhL/suSs+uihhXdAoGAR/PIacSkqcpJahSpdVy6\r\n" \
	"+gAJqF0gvz062mA4N1TXENGNQ3XMpiE0ZRObVq5EjVyJEbrAihJRCCk6bD2Y8yRz\r\n" \
	"rf579kGgUFacJQaTwg8T5emGcB1lGE96YFOSMlktqT6dEj+J2SIG6gTnGZFtaPCB\r\n" \
	"+30jNsypEKumV3E3O7KrQaA=\r\n" \
	"-----END PRIVATE KEY-----\r\n";


static const char *mbedtls_test_ca_crt =
	"-----BEGIN CERTIFICATE-----\r\n" \
	"MIIDozCCAosCFG8LuhxSx/IA0zALJmeB5JUV7jmiMA0GCSqGSIb3DQEBCwUAMIGN\r\n" \
	"MQswCQYDVQQGEwJDTjEQMA4GA1UECAwHSmlhbmdTdTEPMA0GA1UEBwwGU3VaaG91\r\n" \
	"MRAwDgYDVQQKDAdSZWFsc2lsMQ8wDQYDVQQLDAZjbGllbnQxFDASBgNVBAMMC2Ns\r\n" \
	"aWVudF90ZXN0MSIwIAYJKoZIhvcNAQkBFhNjbGllbnRfdGVzdEAxMjMuY29tMB4X\r\n" \
	"DTIzMDYwNjA3MTk0NloXDTMzMDYwMzA3MTk0NlowgY0xCzAJBgNVBAYTAkNOMRAw\r\n" \
	"DgYDVQQIDAdKaWFuZ1N1MQ8wDQYDVQQHDAZTdVpob3UxEDAOBgNVBAoMB1JlYWxz\r\n" \
	"aWwxDzANBgNVBAsMBmNsaWVudDEUMBIGA1UEAwwLY2xpZW50X3Rlc3QxIjAgBgkq\r\n" \
	"hkiG9w0BCQEWE2NsaWVudF90ZXN0QDEyMy5jb20wggEiMA0GCSqGSIb3DQEBAQUA\r\n" \
	"A4IBDwAwggEKAoIBAQCsTIOSoYt6mO+CPE4Lg1dk8E6k18k4yyRrt72l6PU4da6q\r\n" \
	"CBUiuVE/GY76BzoUhR2I7aHjUiY7g5dja/SdNzsdC0jxBax782DcZ/Z2keawq2Sr\r\n" \
	"xsxIs65d0d6mmqiua11lO4uvT2LD5T5WHGwJV/pRYsBmc0Nmtm65gSLspwm5ydgA\r\n" \
	"jP5ZcfGrAwvL8vyX0sbDlBKhsZO4nMv6I17Kfn6RFaXYeuf7gXu476QAqxkTFgOJ\r\n" \
	"4e+wp9XkH98atdB8wrKk3CXp0Q5VLV+Ml6CS77DNuS9cS5ffVUwemoJNpOg8Ew97\r\n" \
	"Ys6cDRyE6AINUMTXuyicamDM8mkbFOZwRBdkkMIpAgMBAAEwDQYJKoZIhvcNAQEL\r\n" \
	"BQADggEBAAJYPqTxLqrQeTzQ3C9gvueH/30COVIz8q5bzipUafePbEy46ynpwuDc\r\n" \
	"OMVJGqdXb/orO0SEAacj7AXI49W07HRfU5uSu/OjdTa+CGEkabMF+oYYswrAOq5d\r\n" \
	"JjSXUmAn5VWjqJT4lzIegZENrYsTc1uPSg1mIitpOew4KU8LpnGbXNFiZB/8Y5TN\r\n" \
	"5rd3PD7OklBNLBcDf7/hY5PXPy2a7nrpA6Cd2fHULFfTGVxQIbCW+oDXXTjLGeuT\r\n" \
	"ky9y9L1KOHnIIl1T3kM40gSMWfAGP1bANUYhojB4zK+T2PuzsBon9vgzJylhqoOF\r\n" \
	"l7c6sZbFeWhh2E/rMWbat23ALMH0LI8=\r\n" \
	"-----END CERTIFICATE-----\r\n";
#endif

void echo_data(ws_conn *conn, int data_len, enum opcode_type opcode)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n>>>>>> Receiving: %s with length: %d from %d\r\n", conn->receivedData, (int)data_len, conn->sock);
	if (opcode == TEXT_FRAME) {
		ws_server_sendText((char *)conn->receivedData, (size_t)data_len, 0, conn);
	} else if (opcode == BINARY_FRAME) {
		ws_server_sendBinary(conn->receivedData, (size_t)data_len, 0, conn);
	}
}

static void example_wsserver_thread(void *param)
{
	(void)param;

	// Delay to check successful WiFi connection and obtain of an IP address
	while (LwIP_Check_Connectivity(0) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n\r\n\r\n>>>>>>>>>>>>>>>websocket server example<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");

#if USE_WSS
	if (ws_server_setup_cert(mbedtls_test_srv_crt, mbedtls_test_srv_key, mbedtls_test_ca_crt) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\nERROR: ws_server_setup_cert\r\n");
		goto exit;
	}
#endif
	ws_server_dispatch(echo_data);

#if USE_WSS
	if (ws_server_start(443, 5, 4096, WS_SERVER_SECURE_TLS) != 0) {
#else
	if (ws_server_start(80, 5, 4096, WS_SERVER_SECURE_NONE) != 0) {
#endif
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\nERROR: ws_server_start\r\n");
	}
#if USE_WSS
exit:
#endif
	rtos_task_delete(NULL);
}

void example_wsserver(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_wsserver_thread"), example_wsserver_thread, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\n%s rtos_task_create(example_wsserver_thread) failed\r\n", __FUNCTION__);
	}
}

