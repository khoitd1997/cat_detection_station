# Bluetooth for Cat Alert Station

Utilizes the bluez api to send data from the station back to main pc. The host and station code uses python3 since it integrates nicely with other parts(computer vision part also uses python and libiio has python bindings)

## References

https://people.csail.mit.edu/albert/bluez-intro/x502.html (c example of bluez)

https://people.csail.mit.edu/albert/bluez-intro/x290.html (py example of bluez)

https://stackoverflow.com/questions/1599459/optimal-lock-file-method (file locking in case working with c++ is needed)

https://stackoverflow.com/questions/7573282/how-do-i-lock-files-using-fopen (file locking reference)

## Getting bluez and Setting it up

```shell
sudo apt-get update
sudo apt-get install bluetooth libbluetooth-dev python3-dev
pip3 install pybluez
```

In order for the server to work properly, sd mode needs to be enabled:

```shell
sudo sdptool add SP

# then add -C after bluetoothd in this file
sudo nano /lib/systemd/system/bluetooth.service
```

The server also needs to be run as sudo to work

## Notes

Programming with bluez is pretty straightforward and looks almost identical to socket programming