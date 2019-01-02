#!/bin/bash

source ../utils.sh

print_message "Removing Old Driver"
sudo rmmod htu21d_kd.ko