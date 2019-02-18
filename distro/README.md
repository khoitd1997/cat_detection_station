# Distro README

The custom distro will be built using buildroot as this is a simple configuration so Yocto is not needed for now.

## References

https://stackoverflow.com/questions/11091623/python-packages-offline-installation (install pip package locally)

https://www.instructables.com/id/How-to-Transfer-Files-From-a-Raspberry-Pi-to-a-Win/ (transfer file via bluetooth)

https://tewarid.github.io/2014/10/29/bluetooth-on-raspberry-pi-with-buildroot.html (bluetooth with buildroot)

## Special Notes

**ALWAYS EJECT SD CARD AND POWEROFF PI PROPERLY**, failing to do so may result in things like the partition being read-only and the file was not completely saved

For ash shell, to poweroff:

```shell
poweroff -f
```

## Configurations

- Root passwords
- Enable gpu memory to be at least 128Mb(do this by setting the additional argument to the post-image script to be gpu_mem_1024=150)

## Packages

Defconfig saved as **cat_alert_defconfig**

Base config is raspberrypi3_defconfig since the libraries used for camera doesn't seem to support 64 bits that well

- Init System:
  - Busybox: as long as there is no need for fancier things, this should be fine
- Classifier and Vision: This category probably changes based on how well the development go
  - Python 3
  - OpenCV 3
- Communication:
  - rpi3 Bluetooth firmware
  - Bluez utils and hci support in kernel
- Bootloader:
  - U-boot: rpi3 is officially supported so shouldn't be too much work
- Kernel Module and DTS:
  - i2c, iio subsystem in kernel settings(as built-in not modules)
  - iio python bindings
  - mdev for device loading(remember to enable it)
  - Pi camera:
    -  select the extra option for rpi firmware, (no need for start_x = 1 in config.txt)
    -  enable the relevant options in Multimedia application inside kernel driver of kernel settings
  - dts support for kernel modules autoload
- Other stuffs:
  - libiio
  - glog for logging
- Debug Tools:
  - vcdbg: for debugging of dts and other rpi specific things
  - bash: just for initial debugging
  - nano
  - coreutils
  - dtc and its debugging tools
  - Logitech Unified Receiver Driver(for debugging using wireless keyboard)

Packages Being considered:

- Sudo and various user for security
- SeLinux
- swu update?
- google breakpad for crash report

## Build Optimizations

- Enable compiler cache
- Enable multi core compiling
- Use toolchain profile to prevent toolchain rebuild

## Rootfs

- Source files for reading pictures, reporting weather and generate logs

## dtb and overlays

Use the config.txt(if using built-in bootloader) to apply additional overlays on top of original dts. The i2c sensors will use pin 3 and 5, which belong to i2c1 subsystem.

The overlays can be added to the final boot image by adding to the post-image.sh file:

```shell
# post-image.sh
# ... other stuffs

# copy user overlays to final image
# compile dts files in external package path and move to final overlays
for file in ${BR2_EXTERNAL_KD_BR2_PATH}/**/*.dts
do
    filename="${file##*/}"
    filename="${filename%.*}"
    dtc -@ -I dts -O dtb -o "${filename}.dtbo" "${file}"
    mv "${filename}.dtbo" "${BINARIES_DIR}/rpi-firmware/overlays/"
    printf "\ndtoverlay=${filename}\n" >> "${BINARIES_DIR}/rpi-firmware/config.txt"
done

# ...
# genimage something
```

## Adding Kernel Modules

Use the external tree features along with kernel module infrastructure to compile the modules as regular packages

To add the kernel dir to the list of packages in menuconfig:

```shell
make BR2_EXTERNAL=/home/kd/cat_detection_station/kernel_driver menuconfig
```

## Build

```shell
make raspberrypi3_defconfig
make linux-menuconfig

## ...customize it
make menuconfig

## Build
make

# flash the final image to the sd card
make && sudo dd if=./output/images/sdcard.img of=/dev/sdd bs=2048 status=progress && sudo sync
```