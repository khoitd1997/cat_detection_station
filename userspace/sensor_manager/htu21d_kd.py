import iio
from iio_sensor import IIO_Sensor


class Htu21d_kd(IIO_Sensor):
    def __init__(self, ctx: iio.Context):
        IIO_Sensor.__init__(self, ctx, "htu21d_kd")

    def fill_report(self, report: dict):
        report["humidity"] = self.calc_final_value("humidityrelative")
