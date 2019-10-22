import socket
import struct

HOST = '192.168.1.9'
PORT = 1507

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((HOST, PORT))

try:
    print(repr(sock.recv(1024)).split("'")[1])
    response = ''
    while "tries" not in response:
        try:
            a = int(input("Take a guess: "))
            if a < 0 or a > 65535:
                raise Exception

            sock.sendall(struct.pack("!I", a))
            response = repr(sock.recv(1000))

            print(response.split("'")[1])

        except Exception:
            print("Give an integer in [0, 65535] range")
            pass

except KeyboardInterrupt:
    pass
