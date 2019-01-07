# Kernel Driver Documentation

The kernel drivers are compiled like a buildroot external package with the help of the kernel module infrastructure. As such files not usually seen with kernel modules(like external.mk) are present to make sure the external tree is valid.

The iio kernel driver uses the managed driver function as such, the memories are automatically freed and the device is automatically unregistered on removal.

## HTU21D Driver

I copied most of sparkfun macros for register definition [here][1], the rest is just implementation of the datasheet.

I don't implement all of the features, just enough to read/write in hold mode as well as adjust the resolution and reset the device, the sensors took more time to measure than I expected so there is a delay function in there, future implementation may utilize this delay to do other stuffs(like ask for a measurements, then do something else and get back to grab it).

The driver utilizes i2c and is part the iio subsystem, it will be loaded at boot if it's specified in the dts overlay.

Special thanks to Sparkfun repo for the macros and the reference, the i2c protocol specified in the datasheet wasn't accurate of what the sensors wanted and Sparkfun reference code was really helpful.

The HTU21D measures both temperature and humidity but due to the long delay time, I would probably use only the humidity and uses the temperature of the bmp280.

## BMP280 driver

I ported most of the code from [Bosch Github][2] and integrated it with the iio subsystem of the kernel, **THERE WAS A BUG** their uncomp number should have been s32(like the datasheet said) but it ended up being u32, which messed up a lot of things.

The driver utilizes i2c and is part the iio subsystem, it will be loaded at boot if it's specified in the dts overlay.

For the simple task of measuring data for weather station, some original API will be discarded as they are not needed.

[1]: https://github.com/sparkfun/SparkFun_HTU21D_Breakout_Arduino_Library/blob/master/src/SparkFunHTU21D.h
[2]: https://github.com/BoschSensortec/BMP280_driver#examples