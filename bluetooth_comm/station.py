import bluetooth
import json
import threading
import logging
from PIL import Image
import time
import traceback

import cat_logger
from bluetooth_shared import bluetooth_const, bluetooth_utils


def start_weather_server(logger: logging.Logger)->None:
    logger.info("Starting weather server")
    server_sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    try:
        server_sock.bind(("", 0))
        server_sock.listen(1)
    except Exception as error:
        logger.error("Error trying to bind and listen: " + str(error))
        return

    logger.info("listening for bluetooth connections")
    bluetooth.advertise_service(server_sock, bluetooth_const.weather_service_name, bluetooth_const.weather_service_uuid)

    while(1):
        try:
            client_sock, address = server_sock.accept()
            logger.info("Accepted connection from %s", address)

            if address[0] == bluetooth_const.host_addr:
                logger.info("Handling connection from %s", address)
                resp = bluetooth_const.weather_info_prototype
                resp["temperature"] = 10.5
                resp["pressure"] = 9.6
                resp["humidity"] = 13.2
                client_sock.send(json.dumps(resp))  # make sure it sends everything
            else:
                logger.warning("Received connections from unknown source")

        except Exception as error:
            logger.error("Problem communicating with clients: " + str(error))
            break
        finally:
            client_sock.close()

    server_sock.close()
    bluetooth.stop_advertising(server_sock)


def send_cat_update(catIsHere: bool, pictureLocation: str, logger: logging.Logger)->None:
    logger.info("Starting process to send cat updates")
    try:
        service_matches = bluetooth.find_service(address=bluetooth_const.host_addr,  uuid=bluetooth_const.cat_alert_service_uuid)
    except Exception as error:
        logger.error("Error trying to find cat alert service: " + str(error))
        return

    if len(service_matches) == 0:
        logger.debug("Can't find any services that match cat alert service")
        return

    match = None
    for con in service_matches:
        port = con["port"]
        name = con["name"]
        host = con["host"]
        if port != None and name == bluetooth_const.cat_alert_service_name and host == bluetooth_const.host_addr:
            match = con

    if match != None:
        sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

        try:
            sock.connect((host, port))
        except Exception as error:
            logger.error("Can't connect to cat alert service: " + str(error))
            return

        try:
            data = bluetooth_const.cat_info_prototype
            data["catIsHere"] = catIsHere
            data["timestamp"] = time.asctime(time.localtime(time.time()))
            bluetooth_utils.send_data(sock, bytearray(json.dumps(data), 'utf8'))
            if catIsHere:
                bluetooth_utils.send_data(sock, bytearray(open(pictureLocation, "rb").read()))

        except Exception as error:
            logger.error("Error trying to send picture to host: " + str(error))
            traceback.print_exc()

        finally:
            sock.close()
    else:
        logger.debug("No services matched weather service")
        return


if __name__ == "__main__":
    cat_logger.setup_logger("cat_station.log")
    logger = cat_logger.get_logger()

    # try:
    #     t = threading.Thread(target=start_weather_server, args=(logger,))
    #     t.start()
    # except Exception as error:
    #     logger.error("Error starting threads: " + str(error))

    # time.sleep(40)
    send_cat_update(True, "test_cat_photo.jpg", logger)
