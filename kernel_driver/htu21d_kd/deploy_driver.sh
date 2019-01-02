#!/bin/bash

pi_address="pi@192.168.1.73"

scp -r  ../../kernel_driver ${pi_address}:~
# ; bash -l
ssh -t ${pi_address} 'cd ~/kernel_driver/htu21d_kd; chmod u+x build_driver.sh; ./build_driver.sh; sleep 1; ' 
# chmod u+x test_driver.sh; sudo ./test_driver.sh'