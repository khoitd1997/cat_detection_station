#!/bin/bash

source ../utils.sh

print_message "Removing Old Driver"
sudo rmmod bmp280_kd.ko