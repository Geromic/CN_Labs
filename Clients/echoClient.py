import socket
import struct

HOST = '192.168.1.9'
PORT = 1507

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((HOST, PORT))

message = "Hello, server!"
sock.sendall(struct.pack(str(len(message)) + "s", message.encode("UTF-8")))
data = sock.recv(1000)

print("Received '" + repr(data) + "'.")
print("Connection ended.")




