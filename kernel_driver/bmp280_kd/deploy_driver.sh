#!/bin/bash

pi_address="pi@192.168.1.73"

scp -r  ../../kernel_driver ${pi_address}:~
ssh -t ${pi_address} 'cd ~/kernel_driver/bmp280_kd; chmod u+x build_driver.sh; ./build_driver.sh; bash -l'