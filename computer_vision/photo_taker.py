# used for taking picture as source for training cv
# got most from here: https://www.raspberrypi.org/forums/viewtopic.php?t=67109

import os
import time
import datetime

TOTAL_PICS = 400
INTERVAL = 180
# INTERVAL = 8
PICTURE_DIR = "/home/pi/cat_pics_" + datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')

imageCount = 0
while imageCount < TOTAL_PICS:
    imageNumber = str(imageCount).zfill(7)
    if not os.path.exists(PICTURE_DIR):
        os.makedirs(PICTURE_DIR)
    os.system("raspistill --nopreview -o " + PICTURE_DIR + "/image%s.jpg"%(imageNumber))
    imageCount += 1
    time.sleep(INTERVAL - 6)