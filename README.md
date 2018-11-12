# cat_detection_station

Software for a station that detects a cat presence and then notify other people

## References

[SCCB vs I2C for the OV7670](http://e2e.ti.com/support/processors/f/791/t/6092?SCCB-vs-I2C)

[OV7670 Reference Kernel Driver](https://elixir.bootlin.com/linux/v4.1/source/drivers/media/i2c/ov7670.c)

[OV7670 Wiki](https://wiki.epfl.ch/prsoc/ov7670)

[I2C in Linux](https://elinux.org/Interfacing_with_I2C_Devices)

[I2C kernel driver](https://bootlin.com/pub/conferences/2018/elc/opdenacker-kernel-programming-device-model-i2c/kernel-programming-device-model-i2c.pdf)

[I2C kernel driver example](https://www.kernel.org/doc/Documentation/i2c/dev-interface)

## Hardware

- Raspbery pi 3: selected because to future proof in case we need to run complex image analysis algorithm
- OV7670: the rpi3 has plug and play camera but I want to use this to learn how to develop linux kernel driver also
- Weather sensor(probably will be BMP280): since the station would be outside, might as well monitor the weather at the same time

## Cat Detection Program

Probably utilize computer vision to detect cat presence. First step is to use simple finding contour around certain areas that correspond to the cat fur color, later steps would probably be to gather enough pictures to train a model that can recognize the cat

## OS

This project uses an rpi3 as the main board running a custom linux distro created by buildroot, it would be optimize to be as lightweight and fast as possible, it should include:

- Newest stable kernel(with real-time patch included if possible)
- Python 3(may be removed later if opencv c++ is used instead)
- Custom driver for the OV7670, and the temperature sensor, so it needs i2c support
- Either Bluetooth or wifi support for sending data