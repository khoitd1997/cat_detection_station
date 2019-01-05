import socket
import errno


def send_all(sock, data):
    totalSent = 0
    data = bytes(data)
    while(totalSent < len(data)):
        totalSent += sock.send(data)


def recv_all(sock, max_size):
    # got it from here https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data
    fragments = []
    totalRecved = 0
    while True:
        try:
            chunk = sock.recv(max_size)
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