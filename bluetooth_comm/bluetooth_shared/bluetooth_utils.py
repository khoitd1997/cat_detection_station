import socket
import errno
from . import bluetooth_const


def make_package(data: bytearray)->bytes:
    lenDescriptor = str(len(data))
    if(len(lenDescriptor) > bluetooth_const.max_data_len):
        print("Got package too long")
        return None

    while(len(lenDescriptor) < bluetooth_const.max_data_len):
        lenDescriptor = "0" + lenDescriptor  # pad to a fixed length

    lenDescriptor = bytearray(lenDescriptor, 'utf8')
    lenDescriptor.extend(data)
    return bytes(lenDescriptor)


def send_data(sock, data: bytearray):
    totalSent = 0
    data = make_package(data)
    if data:
        while(totalSent < len(data)):
            totalSent += sock.send(data)
    else:
        raise ValueError("Data length is too long")


def recv_all(sock, max_size):
    # got it from here https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data
    fragments = []
    totalRecved = 0
    while True:
        try:
            chunk = sock.recv(max_size - totalRecved)
            if not chunk:
                break

            totalRecved += len(chunk)
            fragments.append(chunk)
            if totalRecved >= max_size:
                break
        except socket.error as error:  # certain network error means other side is done
            if(str(errno.ECONNRESET) in str(error)):
                break
            else:
                raise

    return bytes(b''.join(fragments))


def recv_data(sock):
    dataLen = int(recv_all(sock, bluetooth_const.max_data_len).decode("utf-8"))
    data = recv_all(sock, dataLen)
    return data