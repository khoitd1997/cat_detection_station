/*
 HTU21D Humidity Sensor Library
 By: Nathan Seidle
 SparkFun Electronics
 Date: September 22nd, 2013
 License: This code is public domain but you buy me a beer if you use this and we meet someday
 (Beerware license).

 Get humidity and temperature from the HTU21D sensor.

 This same library should work for the other similar sensors including the Si

 */

#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/sysfs.h>
#include <linux/types.h>

#include <linux/delay.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>

#define DRIVER_NAME "htu21d_kd"
#define MANUFACTURER "bosch"

#define HTU21D_ADDRESS 0x40  // Unshifted 7-bit I2C address for the sensor

#define HTU21D_TRIGGER_TEMP_MEASURE_HOLD 0xE3
#define HTU21D_TRIGGER_HUMD_MEASURE_HOLD 0xE5
#define HTU21D_TRIGGER_TEMP_MEASURE_NOHOLD 0xF3
#define HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD 0xF5
#define HTU21D_WRITE_USER_REG 0xE6
#define HTU21D_READ_USER_REG 0xE7
#define HTU21D_SOFT_RESET 0xFE

#define HTU21D_USER_REGISTER_RESOLUTION_MASK 0x81
#define DEFAULT_RESOLUTION HUMIDITY_12_TEMP_14

enum htu21d_resolution {
  HUMIDITY_12_TEMP_14 = 0x00,
  HUMIDITY_8_TEMP_12  = 0x01,
  HUMIDITY_10_TEMP_13 = 0x80,
  HUMIDITY_11_TEMP_11 = 0x81,
};

enum htu21d_kd_chan {
  TEMP,
  HUMIDITYRELATIVE,
};

struct htu21d_kd_data {
  struct i2c_client *    client;
  struct mutex           lock;
  enum htu21d_resolution res;
};

static unsigned long htu21d_get_meas_delayms(enum htu21d_resolution res, u8 type) {
  switch (res) {
    case HUMIDITY_12_TEMP_14:
      return (type == HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD) ? 15 : 16;
      break;
    case HUMIDITY_8_TEMP_12:
      return (type == HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD) ? 2 : 14;
      break;
    case HUMIDITY_10_TEMP_13:
      return (type == HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD) ? 4 : 15;
      break;
    case HUMIDITY_11_TEMP_11:
      return (type == HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD) ? 7 : 7;
      break;
  }
  return 0;
}

static enum htu21d_resolution htu21d_read_resolution(struct i2c_client *client) {
  int ret = i2c_smbus_read_byte_data(client, HTU21D_READ_USER_REG);
  if (ret < 0) {
    printk(KERN_DEBUG "htu21d: Error while reading resolution with errors: %d", ret);
    return ret;
  }
  return (enum htu21d_resolution)(ret & HTU21D_USER_REGISTER_RESOLUTION_MASK);
}

static int htu21d_write_resolution(struct i2c_client *client, enum htu21d_resolution config) {
  int ret = i2c_smbus_read_byte_data(client, HTU21D_READ_USER_REG);
  if (ret < 0) {
    printk(KERN_DEBUG "htu21d: while reading resolution inside write resolution with errors: %d",
           ret);
    return ret;
  }

  ret = i2c_smbus_write_byte_data(client,
                                  HTU21D_WRITE_USER_REG,
                                  config | ((ret & 0xff) & ~HTU21D_USER_REGISTER_RESOLUTION_MASK));
  if (ret < 0) { printk(KERN_DEBUG "htu21d Error while writing resolution with errors: %d", ret); }
  return ret;
}

static int htu21d_soft_reset(struct i2c_client *client) {
  int ret = i2c_smbus_write_byte(client, HTU21D_SOFT_RESET);
  if (ret < 0) { printk(KERN_ALERT "htu21d Error while doing soft reset with errors: %d", ret); }
  return ret;
}

static int htu21d_read_data_no_hold(struct i2c_client *    client,
                                    u8                     type,
                                    enum htu21d_resolution res,
                                    int *                  output) {
  int           ret = 0;
  u8            readBuf[2];
  unsigned long delayms;

  struct i2c_msg msgs[] = {
      {
          .addr  = client->addr,
          .flags = I2C_M_RD,
          .len   = ARRAY_SIZE(readBuf),
          .buf   = readBuf,
      },
  };

  ret = i2c_smbus_write_byte(client, type);
  if (ret < 0) {
    printk(KERN_ALERT "htu21d Error while writing data no hold, errors: %d", ret);
    return ret;
  }

  delayms = htu21d_get_meas_delayms(res, type);
  msleep(delayms);
  ret = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
  if (ret < 0) {
    printk(KERN_WARNING "htu21d: Giving sensors additional sleep time");
    msleep(2);  // give it some more time
    ret = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
  }

  if (ret == ARRAY_SIZE(msgs)) {
    *output = ((readBuf[0] << 8) + (readBuf[1] & 0xfc)) & 0xffff;
    ret     = 0;
  }
  return ret;
}

static int htu21d_read_temp(struct htu21d_kd_data *data, int *output) {
  return htu21d_read_data_no_hold(
      data->client, HTU21D_TRIGGER_TEMP_MEASURE_NOHOLD, data->res, output);
}

static int htu21d_read_humidity(struct htu21d_kd_data *data, int *output) {
  return htu21d_read_data_no_hold(
      data->client, HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD, data->res, output);
}

static int htu21d_init(struct htu21d_kd_data *data) {
  int ret = htu21d_write_resolution(data->client, data->res);
  if (ret < 0) { printk(KERN_ALERT "htu21d: Error while initting device: %d", ret); }
  return ret;
}

static const struct iio_chan_spec htu21d_kd_channels[] = {
    [TEMP] =
        {
            .type               = IIO_TEMP,
            .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
        },
    [HUMIDITYRELATIVE] =
        {
            .type               = IIO_HUMIDITYRELATIVE,
            .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
        },
};

static int htu21d_kd_read_raw(
    struct iio_dev *indio_dev, struct iio_chan_spec const *chan, int *val, int *val2, long mask) {
  struct htu21d_kd_data *data = iio_priv(indio_dev);
  int                    ret  = 0;

  mutex_lock(&data->lock);
  switch (chan->type) {
    case IIO_TEMP:
      switch (mask) {
        case IIO_CHAN_INFO_RAW:
          ret = htu21d_read_temp(data, val);
          if (ret < 0) { goto out; }
          ret = IIO_VAL_INT;
          goto out;
          break;
      }
      break;

    case IIO_HUMIDITYRELATIVE:
      switch (mask) {
        case IIO_CHAN_INFO_RAW:
          ret = htu21d_read_humidity(data, val);
          if (ret < 0) { goto out; }
          ret = IIO_VAL_INT;
          goto out;
          break;
      }
      break;

    default:
      ret = -EINVAL;
  }

out:
  mutex_unlock(&data->lock);
  return ret;
}

// these numbers come from the datasheet formulas
static IIO_CONST_ATTR(in_temp_scale, "2.681227");
static IIO_CONST_ATTR(in_temp_offset, "-17473.03437");
static IIO_CONST_ATTR(in_humidityrelative_scale, "0.001907348633");
static IIO_CONST_ATTR(in_humidityrelative_offset, "-3145.728");

static struct attribute *htu21d_kd_attributes[] = {
    &iio_const_attr_in_temp_scale.dev_attr.attr,
    &iio_const_attr_in_temp_offset.dev_attr.attr,
    &iio_const_attr_in_humidityrelative_scale.dev_attr.attr,
    &iio_const_attr_in_humidityrelative_offset.dev_attr.attr,
    NULL,
};

static const struct attribute_group htu21d_kd_attribute_group = {
    .attrs = htu21d_kd_attributes,
};

static const struct iio_info htu21d_kd_iio_info = {
    .attrs    = &htu21d_kd_attribute_group,
    .read_raw = htu21d_kd_read_raw,
};

static int htu21d_kd_probe(struct i2c_client *client, const struct i2c_device_id *id) {
  struct htu21d_kd_data *chip      = NULL;
  struct iio_dev *       indio_dev = NULL;
  int                    ret       = 0;

  printk(KERN_INFO "htu21d: Probe started");

  // managed device that will deleted automatically on remove
  indio_dev = devm_iio_device_alloc(&client->dev, sizeof(struct htu21d_kd_data));
  if (!indio_dev) { return -ENOMEM; }

  chip = iio_priv(indio_dev);
  mutex_init(&chip->lock);
  chip->client = client;
  chip->res    = DEFAULT_RESOLUTION;

  ret = htu21d_init(chip);  // get device ready and write default settings in
  if (ret < 0) {
    printk(KERN_ALERT "htu21d: Failed to init device");
    return ret;
  }

  /* this is only used for device removal purposes */
  i2c_set_clientdata(client, indio_dev);

  /* Establish that the iio_dev is a child of the i2c device */
  indio_dev->name         = id->name;
  indio_dev->dev.parent   = &client->dev;
  indio_dev->info         = &htu21d_kd_iio_info;
  indio_dev->channels     = htu21d_kd_channels;
  indio_dev->num_channels = ARRAY_SIZE(htu21d_kd_channels);
  indio_dev->modes        = INDIO_DIRECT_MODE;

  ret = devm_iio_device_register(indio_dev->dev.parent, indio_dev);
  if (ret) {
    printk(KERN_ALERT "htu21d: Failed at registering iio device");
    return ret;
  }

  printk(KERN_INFO "htu21d: Done Probing");
  return 0;
}

static int htu21d_kd_remove(struct i2c_client *client) {
  printk(KERN_INFO "Removing htu21d Driver");
  return 0;
}

static const struct of_device_id htu21d_kd_of_match[] = {
    {
        .compatible = MANUFACTURER "," DRIVER_NAME,
    },
    {},
};
MODULE_DEVICE_TABLE(of, htu21d_kd_of_match);

static const struct i2c_device_id htu21d_kd_id[] = {{DRIVER_NAME, 0}, {}};
MODULE_DEVICE_TABLE(i2c, htu21d_kd_id);

static struct i2c_driver htu21d_kd_driver = {.probe    = htu21d_kd_probe,
                                             .remove   = htu21d_kd_remove,
                                             .id_table = htu21d_kd_id,
                                             .driver   = {
                                                 .owner          = THIS_MODULE,
                                                 .name           = DRIVER_NAME,
                                                 .of_match_table = of_match_ptr(htu21d_kd_of_match),
                                             }};

static const struct i2c_board_info htu21d_kd_info __initdata = {
    I2C_BOARD_INFO(DRIVER_NAME, HTU21D_ADDRESS),
};

static int __init htu21d_kd_init(void) {
  printk(KERN_DEBUG "Initializing htu21d Driver");

  // #ifdef DEBUG
  struct i2c_adapter *adapter = i2c_get_adapter(1);
  if (NULL == adapter) {
    printk(KERN_DEBUG "Can't find adapter");
    return -ENODEV;
  }
  i2c_new_device(adapter, &htu21d_kd_info);
  // #endif

  return i2c_add_driver(&htu21d_kd_driver);
}
module_init(htu21d_kd_init);

static void __exit htu21d_kd_cleanup(void) { i2c_del_driver(&htu21d_kd_driver); };
module_exit(htu21d_kd_cleanup);

MODULE_AUTHOR("Khoi Trinh <khoidinhtrinh@gmail.com>");
MODULE_DESCRIPTION("htu21d kernel driver for the cat detector");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");