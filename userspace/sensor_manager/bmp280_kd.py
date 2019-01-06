import iio
from iio_sensor import IIO_Sensor


class Bmp280_kd(IIO_Sensor):
    def __init__(self, ctx: iio.Context):
        IIO_Sensor.__init__(self, ctx, "bmp280_kd")

    def fill_report(self, report: dict):
        report["temperature"] = self.calc_final_value("temp")
        report["pressure"] = self.calc_final_value("pressure")
