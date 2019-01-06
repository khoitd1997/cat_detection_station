#!/usr/bin/env python
#
# Copyright (C) 2015 Analog Devices, Inc.
# Author: Paul Cercueil <paul.cercueil@analog.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

import iio
import time
import json
from bmp280_kd import Bmp280_kd
from htu21d_kd import Htu21d_kd


class Sensor_Manager:
    def __init__(self):
        self._ctx = iio.Context()
        if(self._ctx):
            self._bmp280_kd = Bmp280_kd(self._ctx)
            self._htu21d_kd = Htu21d_kd(self._ctx)

        raise RuntimeError("No iio context found")

    def get_report(self)->bytearray:
        report = {}
        self._bmp280_kd.fill_report(report)
        self._htu21d_kd.fill_report(report)
        report["timestamp"] = time.asctime(time.localtime(time.time()))

        return bytearray(json.dumps(report), 'utf8')
