import socket
import struct

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

a = int(input('a='))
b = int(input('b='))

s.connect(("192.168.1.9", 1507))

s.send(struct.pack("!H", a))
s.send(struct.pack('!H', b))
c = s.recv(2)

# unpack the content read from the network into a short int
# and convert from network representation back to host
c = struct.unpack('!H', c)
print('a+b=' + c[0].__format__('d'))

s.close()

