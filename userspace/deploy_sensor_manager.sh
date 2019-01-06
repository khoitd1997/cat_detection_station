#!/bin/bash

pi_address="pi@192.168.1.73"
# pi_address="pi@192.168.1.75"

scp -r  sensor_manager ${pi_address}:~
ssh -t ${pi_address} 'sudo modprobe htu21d_kd bmp280_kd; python3 sensor_manager/Sensor_Manager.py'
# python3 client.py 