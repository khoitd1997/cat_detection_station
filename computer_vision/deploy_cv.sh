#!/bin/bash
# used for deploying bluetooth code to the cat alert station

pi_address="pi@192.168.1.73"
# pi_address="pi@192.168.1.75"

scp -r  photo_taker.py ${pi_address}:~
# ssh -t ${pi_address} 'pkill python3'
# python3 client.py 