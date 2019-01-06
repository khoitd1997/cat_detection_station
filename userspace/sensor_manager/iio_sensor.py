import iio
from abc import ABCMeta, abstractmethod


class IIO_Sensor:
    def __init__(self, ctx: iio.Context, sensor_name: str):
        for dev in ctx.devices:
            if dev.name == sensor_name:
                self._dev = dev
                self._name = sensor_name
                self._channs = dev.channels
                if self.check_dev() == False:
                    raise ValueError("Device with name: " + sensor_name + " have invalid input channel")
                return None

        raise ValueError("Can't find sensor with name:" + sensor_name)

    @abstractmethod
    def fill_report(self, report: dict):
        pass

    def check_dev(self) -> bool:
        for chann in self._channs:
            if not chann.output:
                if "raw" not in chann.attrs and "input" not in chann.attrs:
                    return False
                return True

    def find_chann(self, chann_id: str)->iio.Channel:
        for chann in self._channs:
            if chann.id == chann_id:
                return chann
        return None

    def read_attr(self, chann: iio.Channel, attr: str)->str:
        try:
            return chann.attrs[attr].value
        except OSError as e:
        	print('Unable to read ' + attr + ': ' + e.strerror)

    def calc_final_value(self, chann_id: str)->float:
        chann = self.find_chann(chann_id)
        if chann:
            raw_val = 0
            scale = 1
            offset = 0

            if "raw" in chann.attrs:
                raw_val = float(self.read_attr(chann, "raw"))
            else:
                raw_val = float(self.read_attr(chann, "input"))

            if "scale" in chann.attrs:
                scale = float(self.read_attr(chann, "scale"))
            if "offset" in chann.attrs:
                offset = float(self.read_attr(chann, "offset"))

            return (raw_val + offset) * scale

        return None
