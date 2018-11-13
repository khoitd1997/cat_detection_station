/*
 * drivers/media/i2c/ad5820.c
 *
 * AD5820 DAC driver for camera voice coil focus.
 *
 * Copyright (C) 2008 Nokia Corporation
 * Copyright (C) 2007 Texas Instruments
 * Copyright (C) 2016 Pavel Machek <pavel@ucw.cz>
 *
 * Contact: Tuukka Toivonen <tuukkat76@gmail.com>
 *	    Sakari Ailus <sakari.ailus@iki.fi>
 *
 * Based on af_d88.c by Texas Instruments.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/err.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>

#include "bmp280_kd_info.h"

struct bmp280_kd_data {
  struct mutex access_lock;
};

static int bmp280_kd_suspend(struct device *dev) { return 0; }

static int bmp280_kd_resume(struct device *dev) { return 0; }

static int bmp280_kd_probe(struct i2c_client *client, const struct i2c_device_id *devid) {
  printk(KERN_DEBUG "Probing bmp280");
  struct i2c_adapter *adapter = client->adapter;
  s32                 id      = 0;

  if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
    printk(KERN_DEBUG "Bus doesn't support byte operation");
    return -ENODEV;
  }

  dev_info(&adapter->dev, "Detecting devices");

  id = i2c_smbus_read_byte_data(client, BMP280_ID_REG);
  if (id < 0) { return id; }

  printk(KERN_DEBUG "Found id %d", id);
  if (BMP280_ID != id) {
    dev_err(&adapter->dev, "Can't find BMP280");
    return -ENODEV;
  } else {
    dev_info(&adapter->dev, "Found BMP280");
  }

  return 0;
}

static int bmp280_kd_remove(struct i2c_client *client) {
  printk(KERN_DEBUG "Removing bmp280 Driver");
  return 0;
}

// i2c_smbus_read_byte_data(client, reg);
// i2c_smbus_write_byte_data(client, reg, value);

static const struct i2c_device_id bmp280_kd_id[] = {{DRIVER_NAME, 0}, {}};
MODULE_DEVICE_TABLE(i2c, bmp280_kd_id);

static const struct dev_pm_ops bmp280_kd_pm_ops = {.suspend = bmp280_kd_suspend,
                                                   .resume  = bmp280_kd_resume};

static struct i2c_driver bmp280_kd_driver = {
    .probe    = bmp280_kd_probe,
    .remove   = bmp280_kd_remove,
    .id_table = bmp280_kd_id,
    .driver   = {.pm = &bmp280_kd_pm_ops, .owner = THIS_MODULE, .name = DRIVER_NAME}};

static const struct i2c_board_info bmp280_kd_info __initdata = {
    I2C_BOARD_INFO(DRIVER_NAME, BMP280_ADDR),
};

static int __init bmp280_kd_init(void) {
  struct i2c_adapter *adapter = NULL;

  printk(KERN_DEBUG "Initializing bmp280 Driver");

  // create new devices, will log an error if already exists one but otw will work
  adapter = i2c_get_adapter(1);
  if (NULL == adapter) { printk(KERN_DEBUG "Can't find adapter"); }
  i2c_new_device(adapter, &bmp280_kd_info);

  return i2c_add_driver(&bmp280_kd_driver);
}
module_init(bmp280_kd_init);

static void __exit bmp280_kd_cleanup(void) { i2c_del_driver(&bmp280_kd_driver); };
module_exit(bmp280_kd_cleanup);

MODULE_AUTHOR("Khoi Trinh");
MODULE_DESCRIPTION("BMP280 kernel driver for the cat detector");
MODULE_LICENSE("GPL");