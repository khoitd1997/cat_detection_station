# Sensor manager

The sensor manager utilizes libiio python bindings to read iio sensors and prepare a weather report in a form that can be sent over bluetooth.

## Reference

https://wiki.analog.com/resources/tools-software/linux-software/libiio (building it from source)

## How to install python bindings for libiio

Buildroot will have options for this, the command below is for testing on regular distro.

The **libiio distributed with many distros are outdated** so they won't work with current python bindings, safest bet is to install from source.

```shell
# first follow their instructions to install from source

# python binding install
# must clone their repo first
cd libiio/bindings/python
cmake CMakeLists.txt
sudo python3 setup.py install
```

## Using local context

It has fancier feature like network context, but we only need local context, **make sure to modprobe the iio driver first** before trying the code.