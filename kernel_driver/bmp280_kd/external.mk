 ################################################################################
#
# bmp280_kd
#
################################################################################

BMP280_KD_VERSION = 1.0
BMP280_KD_SITE = ~/cat_detection_station/kernel_driver/bmp280_kd
BMP280_KD_SITE_METHOD = local
BMP280_KD_LICENSE = GPLv2
BMP280_KD_NAME = bmp280_kd

$(eval $(kernel-module))
$(eval $(generic-package))