 ################################################################################
#
# htu21d_kd
#
################################################################################

HTU21D_KD_VERSION = 1.0
HTU21D_KD_SITE = ~/cat_detection_station/kernel_driver/htu21d_kd
HTU21D_KD_SITE_METHOD = local
HTU21D_KD_LICENSE = GPLv2
HTU21D_KD_NAME = htu21d_kd

$(eval $(kernel-module))
$(eval $(generic-package))