#!/bin/bash

source ../utils.sh

print_message "Building Driver"

# remove old driver first
chmod u+x remove_driver.sh
source remove_driver.sh

# install new driver
make clean
make
sudo ln -s ~/kernel_driver/bmp280_kd/bmp280_kd.ko /lib/modules/`uname -r`
sudo depmod -a
print_message "Installing New Driver"
sudo modprobe bmp280_kd