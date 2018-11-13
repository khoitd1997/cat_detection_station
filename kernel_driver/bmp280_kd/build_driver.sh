#!/bin/bash

source ../utils.sh

print_message "Building Driver"

# remove old driver first
chmod u+x remove_driver.sh
source remove_driver.sh

# install new driver
make
print_message "Installing New Driver"
sudo insmod bmp280_kd.ko