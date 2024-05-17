The script file is used to send image to device based on http or https by OTA function.

Command : 
	python http_server.py

(1) Please ensure that the version of Python is at least v3.0. The test python version is v3.8.0.
(2) The image to be sent should be placed in the same directory as the script.
(3) You can edit http_server.py to change ip, port, certificate and key of server. You can choose to use HTTP or HTTPS by setting value of 'is_https'.
	 If 'is_https' is set to 0, it means that HTTP will be used and server_port should be set to 8082.
	 If 'is_https' is set to 1, it means that HTTPS will be used and server_port should be set to 443.
(4) If necessary, you can generate the server certificate and key as follows:
		openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem