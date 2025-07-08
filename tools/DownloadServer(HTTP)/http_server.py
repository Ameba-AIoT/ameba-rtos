from http.server import HTTPServer, SimpleHTTPRequestHandler
import ssl
import socket

is_https = 0
server_ip = '192.168.31.193'
server_port = 8082 
server_address = (server_ip, server_port)

httpd = HTTPServer(server_address, SimpleHTTPRequestHandler)
# TCP keepalive
httpd.socket.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
# 
if hasattr(socket, 'TCP_KEEPIDLE'):
    httpd.socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPIDLE, 1)
if hasattr(socket, 'TCP_KEEPINTVL'):
    httpd.socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPINTVL, 1)
if hasattr(socket, 'TCP_KEEPCNT'):
    httpd.socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPCNT, 1)

if is_https == 1:
    httpd.socket = ssl.wrap_socket(httpd.socket, server_side=True, certfile="./cert.pem", keyfile="./key.pem", ssl_version=ssl.PROTOCOL_TLSv1_2)

httpd.serve_forever()