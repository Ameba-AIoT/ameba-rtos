#include "platform_autoconf.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "wifi_fast_connect.h"


// get config arguments from wifi_eap_config.c
extern char *eap_target_ssid;
extern char *eap_identity;
extern char *eap_password;
extern const unsigned char *eap_ca_cert;
extern const unsigned char *eap_client_cert;
extern const unsigned char *eap_client_key;
extern int eap_ca_cert_len;
extern int eap_client_cert_len;
extern int eap_client_key_len;
extern char *eap_client_key_pwd;
extern int eap_start(char *method);

void example_eap_config(void)
{
	eap_target_ssid = "Test_eap";
	eap_identity = "guest2";
	eap_password = "test2";

	/*
		Set client cert is only used for EAP-TLS connection.
		If you are not using EAP-TLS method, no need to set eap_client_cert and eap_client_key value. (leave them to NULL value)
	*/

	/* Internal QC Radius Server: hostapd-2.5/hostapd/client.pem+client.key */
	eap_client_cert = (const unsigned char *)\
					  "-----BEGIN CERTIFICATE-----\r\n" \
					  "MIIC1TCCAb0CAQMwDQYJKoZIhvcNAQELBQAwgYsxCzAJBgNVBAYTAlRXMRAwDgYD\r\n" \
					  "VQQIDAdIc2luY2h1MRAwDgYDVQQHDAdIc2luY2h1MRAwDgYDVQQKDAdSZWFsdGVr\r\n" \
					  "MQwwCgYDVQQLDANTRDkxEDAOBgNVBAMMB1JlYWx0ZWsxJjAkBgkqhkiG9w0BCQEW\r\n" \
					  "F3JheW1vbmQubGVlQHJlYWx0ZWsuY29tMCAXDTIxMDgxNjAzMzAzOVoYDzIxMjEw\r\n" \
					  "NzIzMDMzMDM5WjBXMQswCQYDVQQGEwJDTjETMBEGA1UECAwKU29tZS1TdGF0ZTEh\r\n" \
					  "MB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMRAwDgYDVQQDDAdSZWFs\r\n" \
					  "dGVrMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDCv9TNqrJsSYoqq2n044Gz\r\n" \
					  "Kdd86hYPtg5M6y7JfVjL3WxSIi3qsYLIUGOgNaqdQ5Kx/aCPCvzimfoGcAgL4cax\r\n" \
					  "2c45qXPtPL18jd65kgAymQVRjw7ld5f4qFUIsu2TqSFQ4wETUmxOOm7xBXN50j7C\r\n" \
					  "MpL6bITavjqJexJ7ZC2FWQIDAQABMA0GCSqGSIb3DQEBCwUAA4IBAQCCuvBx98h0\r\n" \
					  "NsRc9c8FJh62k7CkHgWHPmdYypsy15dGkZUQucrhOH8xvaxqeJBFBBtABZlRssLj\r\n" \
					  "nOcqwE6wfT0geZrzfscOdidGqCg28yMSr9MmXk7jOKIE/FymdRT94lssFlUJrAVQ\r\n" \
					  "eyua9S/c2Cl+O2gVQRtrKKAM/ISkTxfnkqJjtF3NZPmF+I/FR4cAvhzphW/5l+Yt\r\n" \
					  "5BafPUHq19vblpUulveTZGzsS5lg0PCdK/I5LGC9WlSx7DON3VYpo4baxuzDP6AV\r\n" \
					  "da5EBI+4dE1Dj3zI2HBzuR/jnb9NDAmhHlo6SJ0BU9lb3rVOhkhwoDa6xYNq3P1t\r\n" \
					  "P9i+cdhnw30z\r\n" \
					  "-----END CERTIFICATE-----\r\n";
	eap_client_key = (const unsigned char *)\
					 "-----BEGIN RSA PRIVATE KEY-----\r\n" \
					 "MIICXgIBAAKBgQDCv9TNqrJsSYoqq2n044GzKdd86hYPtg5M6y7JfVjL3WxSIi3q\r\n" \
					 "sYLIUGOgNaqdQ5Kx/aCPCvzimfoGcAgL4cax2c45qXPtPL18jd65kgAymQVRjw7l\r\n" \
					 "d5f4qFUIsu2TqSFQ4wETUmxOOm7xBXN50j7CMpL6bITavjqJexJ7ZC2FWQIDAQAB\r\n" \
					 "AoGBAIWGjVtt++NvEIJtALrc+eJjf9GvleyzwF5QqcWQYb91U32E3cKabrb1FzmT\r\n" \
					 "UiV9XSkYYmtw9y5OsDSVv46WAGpiEMZe31lvJzSBxiq2KfFr0vWTPoehsqCcebKL\r\n" \
					 "6bs0TVOjTZy0EzRPGRCJyshnuacH6rr97UPPNSRaWSrTBC7hAkEA6bVDWLYgJDeG\r\n" \
					 "SvD2NRO5xVBjXpY4iE1KuSchl+IrXgMk01zQnHL8pFkUG7jWPqEIh199sInP9ehd\r\n" \
					 "ptY4aIBz3QJBANVTQ+n0DG9Vkk2rgFyLQa3RYsm6B4IbtadKKqpxUDd4ggpTHtHp\r\n" \
					 "6UGwoukA8Rz83W1XgAMdbqQgbOT875AhDa0CQERyXAkNPhidUm18aP9eiZfqVVAM\r\n" \
					 "ev4gdPQJK+OJz9222gA1PLu1SZzwod0Eo2qwSwkqxGsB0bBZn6QPNXIVvNkCQQCD\r\n" \
					 "S4wxIhPFMlWpmDSYwNzbrlOu9OBKI+PZ4rv/y14jxffDgwUlDWKTG7Qo1HuR69Ac\r\n" \
					 "zA3t3N+MBKkBBuwrOSzBAkEAigdWMq1wsaTXlvse07DP+AwQBtqonRDg+zsvkG94\r\n" \
					 "y4dFwzx0Dr3AjmDkeI5Qe1rncDkZgqt/eKt30yVFkyVUEg==\r\n" \
					 "-----END RSA PRIVATE KEY-----\r\n";
	eap_client_key_pwd = "";

	/*
		Verify server's certificate is an optional feature.
		If you want to use it please make sure ENABLE_EAP_SSL_VERIFY_SERVER in autoconf_eap.h is set to 1,
		and the eap_ca_cert is set correctly.
	*/
	eap_ca_cert = (const unsigned char *)\
				  "-----BEGIN CERTIFICATE-----\r\n" \
				  "MIIEpzCCA4+gAwIBAgIJAPvZaozpdfjkMA0GCSqGSIb3DQEBCwUAMIGTMQswCQYD\r\n" \
				  "VQQGEwJGUjEPMA0GA1UECBMGUmFkaXVzMRIwEAYDVQQHEwlTb21ld2hlcmUxFTAT\r\n" \
				  "BgNVBAoTDEV4YW1wbGUgSW5jLjEgMB4GCSqGSIb3DQEJARYRYWRtaW5AZXhhbXBs\r\n" \
				  "ZS5jb20xJjAkBgNVBAMTHUV4YW1wbGUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5MB4X\r\n" \
				  "DTE2MDMxNDExMjU0OVoXDTE2MDQxMzExMjU0OVowgZMxCzAJBgNVBAYTAkZSMQ8w\r\n" \
				  "DQYDVQQIEwZSYWRpdXMxEjAQBgNVBAcTCVNvbWV3aGVyZTEVMBMGA1UEChMMRXhh\r\n" \
				  "bXBsZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFhZG1pbkBleGFtcGxlLmNvbTEmMCQG\r\n" \
				  "A1UEAxMdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkwggEiMA0GCSqGSIb3\r\n" \
				  "DQEBAQUAA4IBDwAwggEKAoIBAQC9pireu0aCDLNfMaGv3vId7RXjUhQwSK0jV2Oc\r\n" \
				  "SyvlKWH3P/N+5kLrP2iL6SCzyETVDXZ0vOsAMjcBF0zHp16prXV0d51cTUqeWBb0\r\n" \
				  "I5UnGxleIuuOfSg8zLUJoBWZPqLv++eZ5WgOKHt7SXocjvg7TU5t/TMB0Y8OCz3H\r\n" \
				  "CW2vJ/XKMgMA9HDUu4g57cJu88i1JPRpyFaz/HIQBc7+UNb9z+q09uTZKWTmEMqi\r\n" \
				  "E2U0EEIs7EtbxnOze1/8C4XNlmztrEdwvu6UEBU/TFkUoh9M646NkkBK7wP9n9pv\r\n" \
				  "T0nPQRJiiCrICzVqUtlEi9lIKpbBSMbQ0KzrGF7lGTgm4rz9AgMBAAGjgfswgfgw\r\n" \
				  "HQYDVR0OBBYEFIVyecka74kvOKIW0BjlTc/B+a2NMIHIBgNVHSMEgcAwgb2AFIVy\r\n" \
				  "ecka74kvOKIW0BjlTc/B+a2NoYGZpIGWMIGTMQswCQYDVQQGEwJGUjEPMA0GA1UE\r\n" \
				  "CBMGUmFkaXVzMRIwEAYDVQQHEwlTb21ld2hlcmUxFTATBgNVBAoTDEV4YW1wbGUg\r\n" \
				  "SW5jLjEgMB4GCSqGSIb3DQEJARYRYWRtaW5AZXhhbXBsZS5jb20xJjAkBgNVBAMT\r\n" \
				  "HUV4YW1wbGUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5ggkA+9lqjOl1+OQwDAYDVR0T\r\n" \
				  "BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAZYHM26sxbKOckVqJJ1QY0U2QFlGP\r\n" \
				  "1GYd8v27znxdnRmSonDvv3GjFfhwoyDk0JUuxkK/33ikCxihrgoO/EQTY9BV2OpW\r\n" \
				  "qkB1PDtb3i5ZRNvfjmW0pVA4p+GmdTGaEE5pTlcVnorzVrUeFKaZakb+IDFYzmeF\r\n" \
				  "xp8B3Bb5wvinDligLOaJnSlgS8QeeIab9HZfaVTTuPmVK6zE6D54Y0dJPnykvDdE\r\n" \
				  "cGN0FC+migfilFjJgkDJ0r78nwes55L8zjoofiZuO03rrHww6ARc3v1jYzAufddk\r\n" \
				  "QTiZHgjlMQb2XXMmXLn8kBgoDnqkXFNe8j0h8uxIJSrjOoIyn1h1wvX5/w==\r\n" \
				  "-----END CERTIFICATE-----\r\n";

	eap_client_cert_len = strlen((const char *)eap_client_cert) + 1;
	eap_client_key_len = strlen((const char *)eap_client_key) + 1;
	eap_ca_cert_len = strlen((const char *)eap_ca_cert) + 1;
}

static void example_eap_thread(void *method)
{
	example_eap_config();
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: EAP\n");
	if (strcmp(method, "tls") == 0) {
		// tls must present client_cert, client_key
		eap_start("tls");
	} else if (strcmp(method, "peap") == 0) {
		eap_start("peap");
	} else if (strcmp(method, "ttls") == 0) {
		eap_start("ttls");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Invalid method\n");
	}

	rtos_task_delete(NULL);
}

void example_eap(char *method)
{
	if (rtos_task_create(NULL, ((const char *)"example_eap_thread"), example_eap_thread, method, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create failed\n", __FUNCTION__);
	}
}
