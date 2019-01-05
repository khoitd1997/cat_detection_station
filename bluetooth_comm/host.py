import bluetooth
import sys
import json
import logging
from PIL import Image
import threading
import errno
import socket
import io


from bluetooth_shared import bluetooth_const, bluetooth_utils


def get_weather_data(logger: logging.Logger)->dict:
    try:
        service_matches = bluetooth.find_service(address=bluetooth_const.cat_station_addr,  uuid=bluetooth_const.weather_service_uuid)
    except Exception as error:
        logger.error("Error trying to find weather service: " + str(error))
        return None

    if len(service_matches) == 0:
        logger.debug("Can't find any services that match weather service")
        return None

    match = None
    for con in service_matches:
        port = con["port"]
        name = con["name"]
        host = con["host"]
        if port != None and name == bluetooth_const.weather_service_name and host == bluetooth_const.cat_station_addr:
            match = con

    if match != None:
        sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

        try:
            sock.connect((host, port))
        except Exception as error:
            logger.error("Can't connect to weather service: " + str(error))
            return None

        print("connecting to \"%s\" on %s" % (name, host))
        data = json.loads(sock.recv(1024))
        if data != None:
            print("received[%s]" % data["temperature"])
        else:
            logger.debug("Didn't receive anything from weather service")
        sock.close()

        return data
    else:
        logger.debug("No services matched weather service")
        return None


def start_cat_alert_server(logger: logging.Logger):
    logger.info("Starting cat alert server")
    server_sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    try:
        server_sock.bind(("", 0))
        server_sock.listen(1)
    except Exception as error:
        logger.error("Cat alert server, error trying to bind and listen: " + str(error))
        return None

    logger.info("Cat alert server listening for bluetooth connections")
    bluetooth.advertise_service(server_sock, bluetooth_const.cat_alert_service_name, bluetooth_const.cat_alert_service_uuid)

    while(1):
        try:
            client_sock, address = server_sock.accept()
            logger.info("Cat alert server accepted connection from %s", address)

            if address[0] == bluetooth_const.cat_station_addr:
                logger.info("Handling connection from %s", address)
                data = bluetooth_utils.recv_all(client_sock, bluetooth_const.cat_picture_size)
                if bluetooth_const.data_separator in data:
                    dataList = data.split(bluetooth_const.data_separator)
                    cat_info = json.loads(dataList[0])
                    print(str(cat_info["catIsHere"]))
                    img = Image.open(io.BytesIO(dataList[1]))
                    img.show()
                else:
                    cat_info = json.loads(data)
                    print(str(cat_info["catIsHere"]))

            else:
                logger.warning("Received connections from unknown source")

        except Exception as error:
            logger.error("Problem communicating with clients: " + str(error))
            break
        finally:
            client_sock.close()

    bluetooth.stop_advertising(server_sock)
    server_sock.close()

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG, format=bluetooth_const.logger_format, filename='cat_host.log',
                    filemode='a')
    logger = logging.getLogger("Weather Client Logger")

    # only for debug purpose
    fh = logging.StreamHandler()
    fh.setLevel(logging.DEBUG)
    logger.addHandler(fh)

    try:
        t = threading.Thread(target=start_cat_alert_server, args=(logger,))
        t.start()
    except Exception as error:
        logger.error("Error starting threads: " + str(error))

    data = get_weather_data(logger)
    # start_cat_alert_server(logger)
