/**
 * Copyright (C) 2017 - 2018 Bosch Sensortec GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of the
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 * The information provided is believed to be accurate and reliable.
 * The copyright holder assumes no responsibility
 * for the consequences of use
 * of such information nor for any infringement of patents or
 * other rights of third parties which may result from its use.
 * No license is granted by implication or otherwise under any patent or
 * patent rights of the copyright holder.
 *
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

#define DRIVER_NAME "bmp280_kd"
#define MANUFACTURER "bosch"

#define BMP280_ID_REG 0xD0
#define BMP280_ID 0x58

#if !defined(UINT8_C) && !defined(INT8_C)
#define INT8_C(x) S8_C(x)
#define UINT8_C(x) U8_C(x)
#endif

#if !defined(UINT16_C) && !defined(INT16_C)
#define INT16_C(x) S16_C(x)
#define UINT16_C(x) U16_C(x)
#endif

#if !defined(INT32_C) && !defined(UINT32_C)
#define INT32_C(x) S32_C(x)
#define UINT32_C(x) U32_C(x)
#endif

#if !defined(INT64_C) && !defined(UINT64_C)
#define INT64_C(x) S64_C(x)
#define UINT64_C(x) U64_C(x)
#endif

#define BMP280_SPI_INTF UINT8_C(0)
#define BMP280_I2C_INTF UINT8_C(1)

/*! @name Return codes */
/*! @name Success code*/
#define BMP280_OK INT8_C(0)
/*! @name Error codes */
#define BMP280_E_NULL_PTR INT8_C(-1)
#define BMP280_E_DEV_NOT_FOUND INT8_C(-2)
#define BMP280_E_INVALID_LEN INT8_C(-3)
#define BMP280_E_COMM_FAIL INT8_C(-4)
#define BMP280_E_INVALID_MODE INT8_C(-5)

#define BMP280_CHIP_ID1 UINT8_C(0x56)
#define BMP280_CHIP_ID2 UINT8_C(0x57)
#define BMP280_CHIP_ID3 UINT8_C(0x58)

#define BMP280_I2C_ADDR_PRIM UINT8_C(0x76)
#define BMP280_I2C_ADDR_SEC UINT8_C(0x77)

#define BMP280_DIG_T1_LSB_ADDR UINT8_C(0x88)
#define BMP280_DIG_T1_MSB_ADDR UINT8_C(0x89)
#define BMP280_DIG_T2_LSB_ADDR UINT8_C(0x8A)
#define BMP280_DIG_T2_MSB_ADDR UINT8_C(0x8B)
#define BMP280_DIG_T3_LSB_ADDR UINT8_C(0x8C)
#define BMP280_DIG_T3_MSB_ADDR UINT8_C(0x8D)
#define BMP280_DIG_P1_LSB_ADDR UINT8_C(0x8E)
#define BMP280_DIG_P1_MSB_ADDR UINT8_C(0x8F)
#define BMP280_DIG_P2_LSB_ADDR UINT8_C(0x90)
#define BMP280_DIG_P2_MSB_ADDR UINT8_C(0x91)
#define BMP280_DIG_P3_LSB_ADDR UINT8_C(0x92)
#define BMP280_DIG_P3_MSB_ADDR UINT8_C(0x93)
#define BMP280_DIG_P4_LSB_ADDR UINT8_C(0x94)
#define BMP280_DIG_P4_MSB_ADDR UINT8_C(0x95)
#define BMP280_DIG_P5_LSB_ADDR UINT8_C(0x96)
#define BMP280_DIG_P5_MSB_ADDR UINT8_C(0x97)
#define BMP280_DIG_P6_LSB_ADDR UINT8_C(0x98)
#define BMP280_DIG_P6_MSB_ADDR UINT8_C(0x99)
#define BMP280_DIG_P7_LSB_ADDR UINT8_C(0x9A)
#define BMP280_DIG_P7_MSB_ADDR UINT8_C(0x9B)
#define BMP280_DIG_P8_LSB_ADDR UINT8_C(0x9C)
#define BMP280_DIG_P8_MSB_ADDR UINT8_C(0x9D)
#define BMP280_DIG_P9_LSB_ADDR UINT8_C(0x9E)
#define BMP280_DIG_P9_MSB_ADDR UINT8_C(0x9F)

#define BMP280_CHIP_ID_ADDR UINT8_C(0xD0)
#define BMP280_SOFT_RESET_ADDR UINT8_C(0xE0)
#define BMP280_STATUS_ADDR UINT8_C(0xF3)
#define BMP280_CTRL_MEAS_ADDR UINT8_C(0xF4)
#define BMP280_CONFIG_ADDR UINT8_C(0xF5)
#define BMP280_PRES_MSB_ADDR UINT8_C(0xF7)
#define BMP280_PRES_LSB_ADDR UINT8_C(0xF8)
#define BMP280_PRES_XLSB_ADDR UINT8_C(0xF9)
#define BMP280_TEMP_MSB_ADDR UINT8_C(0xFA)
#define BMP280_TEMP_LSB_ADDR UINT8_C(0xFB)
#define BMP280_TEMP_XLSB_ADDR UINT8_C(0xFC)

#define BMP280_SLEEP_MODE UINT8_C(0x00)
#define BMP280_FORCED_MODE UINT8_C(0x01)
#define BMP280_NORMAL_MODE UINT8_C(0x03)

#define BMP280_SOFT_RESET_CMD UINT8_C(0xB6)

#define BMP280_ODR_0_5_MS UINT8_C(0x00)
#define BMP280_ODR_62_5_MS UINT8_C(0x01)
#define BMP280_ODR_125_MS UINT8_C(0x02)
#define BMP280_ODR_250_MS UINT8_C(0x03)
#define BMP280_ODR_500_MS UINT8_C(0x04)
#define BMP280_ODR_1000_MS UINT8_C(0x05)
#define BMP280_ODR_2000_MS UINT8_C(0x06)
#define BMP280_ODR_4000_MS UINT8_C(0x07)

#define BMP280_OS_NONE UINT8_C(0x00)
#define BMP280_OS_1X UINT8_C(0x01)
#define BMP280_OS_2X UINT8_C(0x02)
#define BMP280_OS_4X UINT8_C(0x03)
#define BMP280_OS_8X UINT8_C(0x04)
#define BMP280_OS_16X UINT8_C(0x05)

#define BMP280_FILTER_OFF UINT8_C(0x00)
#define BMP280_FILTER_COEFF_2 UINT8_C(0x01)
#define BMP280_FILTER_COEFF_4 UINT8_C(0x02)
#define BMP280_FILTER_COEFF_8 UINT8_C(0x03)
#define BMP280_FILTER_COEFF_16 UINT8_C(0x04)

#define BMP280_SPI3_WIRE_ENABLE UINT8_C(1)
#define BMP280_SPI3_WIRE_DISABLE UINT8_C(0)

#define BMP280_MEAS_DONE UINT8_C(0)
#define BMP280_MEAS_ONGOING UINT8_C(1)

#define BMP280_IM_UPDATE_DONE UINT8_C(0)
#define BMP280_IM_UPDATE_ONGOING UINT8_C(1)

#define BMP280_STATUS_IM_UPDATE_POS UINT8_C(0)
#define BMP280_STATUS_IM_UPDATE_MASK UINT8_C(0x01)
#define BMP280_STATUS_MEAS_POS UINT8_C(3)
#define BMP280_STATUS_MEAS_MASK UINT8_C(0x08)
#define BMP280_OS_TEMP_POS UINT8_C(5)
#define BMP280_OS_TEMP_MASK UINT8_C(0xE0)
#define BMP280_OS_PRES_POS UINT8_C(2)
#define BMP280_OS_PRES_MASK UINT8_C(0x1C)
#define BMP280_POWER_MODE_POS UINT8_C(0)
#define BMP280_POWER_MODE_MASK UINT8_C(0x03)
#define BMP280_STANDBY_DURN_POS UINT8_C(5)
#define BMP280_STANDBY_DURN_MASK UINT8_C(0xE0)
#define BMP280_FILTER_POS UINT8_C(2)
#define BMP280_FILTER_MASK UINT8_C(0x1C)
#define BMP280_SPI3_ENABLE_POS UINT8_C(0)
#define BMP280_SPI3_ENABLE_MASK UINT8_C(0x01)

#define BMP280_DIG_T1_LSB_POS UINT8_C(0)
#define BMP280_DIG_T1_MSB_POS UINT8_C(1)
#define BMP280_DIG_T2_LSB_POS UINT8_C(2)
#define BMP280_DIG_T2_MSB_POS UINT8_C(3)
#define BMP280_DIG_T3_LSB_POS UINT8_C(4)
#define BMP280_DIG_T3_MSB_POS UINT8_C(5)
#define BMP280_DIG_P1_LSB_POS UINT8_C(6)
#define BMP280_DIG_P1_MSB_POS UINT8_C(7)
#define BMP280_DIG_P2_LSB_POS UINT8_C(8)
#define BMP280_DIG_P2_MSB_POS UINT8_C(9)
#define BMP280_DIG_P3_LSB_POS UINT8_C(10)
#define BMP280_DIG_P3_MSB_POS UINT8_C(11)
#define BMP280_DIG_P4_LSB_POS UINT8_C(12)
#define BMP280_DIG_P4_MSB_POS UINT8_C(13)
#define BMP280_DIG_P5_LSB_POS UINT8_C(14)
#define BMP280_DIG_P5_MSB_POS UINT8_C(15)
#define BMP280_DIG_P6_LSB_POS UINT8_C(16)
#define BMP280_DIG_P6_MSB_POS UINT8_C(17)
#define BMP280_DIG_P7_LSB_POS UINT8_C(18)
#define BMP280_DIG_P7_MSB_POS UINT8_C(19)
#define BMP280_DIG_P8_LSB_POS UINT8_C(20)
#define BMP280_DIG_P8_MSB_POS UINT8_C(21)
#define BMP280_DIG_P9_LSB_POS UINT8_C(22)
#define BMP280_DIG_P9_MSB_POS UINT8_C(23)
#define BMP280_CALIB_DATA_SIZE UINT8_C(24)

#define BMP280_GET_BITS(bitname, x) ((x & bitname##_MASK) >> bitname##_POS)
#define BMP280_SET_BITS(regvar, bitname, val) \
  ((regvar & ~bitname##_MASK) | ((val << bitname##_POS) & bitname##_MASK))
#define BMP280_SET_BITS_POS_0(reg_data, bitname, data) \
  ((reg_data & ~(bitname##_MASK)) | (data & bitname##_MASK))
#define BMP280_GET_BITS_POS_0(bitname, reg_data) (reg_data & (bitname##_MASK))

typedef s32 (*bmp280_com_fptr_t)(
    struct i2c_client *client, u8 dev_id, u8 reg_addr, u8 *data, u8 len);

typedef void (*bmp280_delay_fptr_t)(unsigned int period);

struct bmp280_calib_param {
  u16 dig_t1;
  s16 dig_t2;
  s16 dig_t3;
  u16 dig_p1;
  s16 dig_p2;
  s16 dig_p3;
  s16 dig_p4;
  s16 dig_p5;
  s16 dig_p6;
  s16 dig_p7;
  s16 dig_p8;
  s16 dig_p9;
  s32 t_fine;
};

struct bmp280_config {
  u8 os_temp;
  u8 os_pres;
  u8 odr;
  u8 filter;
  u8 spi3w_en;
};

struct bmp280_status {
  u8 measuring;
  u8 im_update;
};

struct bmp280_uncomp_data {
  s32 uncomp_temp;
  s32 uncomp_press;
};

struct bmp280_kd_data {
  u8                  chip_id;
  u8                  dev_id;
  u8                  intf;
  bmp280_com_fptr_t   read;
  bmp280_com_fptr_t   write;
  bmp280_delay_fptr_t delay_ms;

  struct bmp280_calib_param calib_param;
  struct bmp280_config      conf;
  struct i2c_client *       client;
  struct mutex              lock;
};

static s8 null_ptr_check(const struct bmp280_kd_data *dev) {
  s8 rslt;

  if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_ms == NULL)) {
    /* Null-pointer found */
    rslt = BMP280_E_NULL_PTR;
  } else {
    rslt = BMP280_OK;
  }

  return rslt;
}

static void interleave_data(const u8 *reg_addr, u8 *temp_buff, const u8 *reg_data, u8 len) {
  u8 index;

  for (index = 1; index < len; index++) {
    temp_buff[(index * 2) - 1] = reg_addr[index];
    temp_buff[index * 2]       = reg_data[index];
  }
}

static s32 i2c_read_wrapper(struct i2c_client *client, u8 dev_id, u8 reg_addr, u8 *data, u8 len) {
  int ret;

  if (len == 1) {
    ret = i2c_smbus_read_byte_data(client, reg_addr);
    if (ret < 0) { return BMP280_E_COMM_FAIL; }
    data[0] = ret;
  } else {
    ret = i2c_smbus_read_i2c_block_data(client, reg_addr, len, data);
    if (ret < 0) { return BMP280_E_COMM_FAIL; }
  }

  return BMP280_OK;
}

static s32 i2c_write_wrapper(struct i2c_client *client, u8 dev_id, u8 reg_addr, u8 *data, u8 len) {
  u8  addr;
  int i;
  int ret;

  for (i = 0; i < len; i += 2) {
    addr = (0 == i) ? reg_addr : data[i - 1];
    ret  = i2c_smbus_write_byte_data(client, addr, data[i]);
    if (ret != 0) {
      printk(KERN_DEBUG "problem writing inside write wrapper");
      return BMP280_E_COMM_FAIL;
    }
  }

  (void)dev_id;
  return BMP280_OK;
}

static s8 bmp280_get_regs(u8 reg_addr, u8 *reg_data, u8 len, const struct bmp280_kd_data *dev) {
  s8 rslt;

  rslt = null_ptr_check(dev);

  if ((rslt == BMP280_OK) && (reg_data != NULL)) {
    /* Mask the register address' MSB if interface selected is SPI */
    if (dev->intf == BMP280_SPI_INTF) reg_addr = reg_addr | 0x80;

    rslt = dev->read(dev->client, dev->dev_id, reg_addr, reg_data, len);
    /* Check for communication error and mask with an internal error code */
    if (rslt != BMP280_OK) rslt = BMP280_E_COMM_FAIL;
  } else {
    rslt = BMP280_E_NULL_PTR;
  }

  return rslt;
}

static s8 bmp280_set_regs(u8 *                         reg_addr,
                          const u8 *                   reg_data,
                          u8                           len,
                          const struct bmp280_kd_data *dev) {
  s8  rslt;
  u8  temp_buff[8]; /* Typically not to write more than 4 registers */
  u16 temp_len;
  u8  reg_addr_cnt;

  if (len > 4) len = 4;

  rslt = null_ptr_check(dev);

  if ((rslt == BMP280_OK) && (reg_addr != NULL) && (reg_data != NULL)) {
    if (len != 0) {
      temp_buff[0] = reg_data[0];
      /* Mask the register address' MSB if interface selected is SPI */
      if (dev->intf == BMP280_SPI_INTF) {
        /* Converting all the reg address into proper SPI write address
         i.e making MSB(R/`W) bit 0 */
        for (reg_addr_cnt = 0; reg_addr_cnt < len; reg_addr_cnt++)
          reg_addr[reg_addr_cnt] = reg_addr[reg_addr_cnt] & 0x7F;
      }

      /* Burst write mode */
      if (len > 1) {
        /* Interleave register address w.r.t data for burst write*/
        interleave_data(reg_addr, temp_buff, reg_data, len);
        temp_len = ((len * 2) - 1);
      } else {
        temp_len = len;
      }

      rslt = dev->write(dev->client, dev->dev_id, reg_addr[0], temp_buff, temp_len);
      /* Check for communication error and mask with an internal error code */
      if (rslt != BMP280_OK) rslt = BMP280_E_COMM_FAIL;
    } else {
      rslt = BMP280_E_INVALID_LEN;
    }
  } else {
    rslt = BMP280_E_NULL_PTR;
  }

  return rslt;
}

static s8 get_calib_param(struct bmp280_kd_data *dev) {
  s32 rslt;
  u8  temp[BMP280_CALIB_DATA_SIZE] = {0};

  rslt = null_ptr_check(dev);

  if (rslt == BMP280_OK) {
    rslt = bmp280_get_regs(BMP280_DIG_T1_LSB_ADDR, temp, BMP280_CALIB_DATA_SIZE, dev);

    if (rslt == BMP280_OK) {
      printk(KERN_DEBUG "Setting calib param");
      dev->calib_param.dig_t1 =
          (u16)(((u16)temp[BMP280_DIG_T1_MSB_POS] << 8) | ((u16)temp[BMP280_DIG_T1_LSB_POS]));
      dev->calib_param.dig_t2 =
          (s16)(((s16)temp[BMP280_DIG_T2_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_T2_LSB_POS]));
      dev->calib_param.dig_t3 =
          (s16)(((s16)temp[BMP280_DIG_T3_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_T3_LSB_POS]));
      dev->calib_param.dig_p1 =
          (u16)(((u16)temp[BMP280_DIG_P1_MSB_POS] << 8) | ((u16)temp[BMP280_DIG_P1_LSB_POS]));
      dev->calib_param.dig_p2 =
          (s16)(((s16)temp[BMP280_DIG_P2_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_P2_LSB_POS]));
      dev->calib_param.dig_p3 =
          (s16)(((s16)temp[BMP280_DIG_P3_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_P3_LSB_POS]));
      dev->calib_param.dig_p4 =
          (s16)(((s16)temp[BMP280_DIG_P4_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_P4_LSB_POS]));
      dev->calib_param.dig_p5 =
          (s16)(((s16)temp[BMP280_DIG_P5_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_P5_LSB_POS]));
      dev->calib_param.dig_p6 =
          (s16)(((s16)temp[BMP280_DIG_P6_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_P6_LSB_POS]));
      dev->calib_param.dig_p7 =
          (s16)(((s16)temp[BMP280_DIG_P7_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_P7_LSB_POS]));
      dev->calib_param.dig_p8 =
          (s16)(((s16)temp[BMP280_DIG_P8_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_P8_LSB_POS]));
      dev->calib_param.dig_p9 =
          (s16)(((s16)temp[BMP280_DIG_P9_MSB_POS] << 8) | ((s16)temp[BMP280_DIG_P9_LSB_POS]));
    }
  }

  return rslt;
}

static s8 bmp280_soft_reset(const struct bmp280_kd_data *dev) {
  s8 rslt         = null_ptr_check(dev);
  u8 reg_addr     = BMP280_SOFT_RESET_ADDR;
  u8 soft_rst_cmd = BMP280_SOFT_RESET_CMD;

  if (rslt == BMP280_OK) {
    rslt = bmp280_set_regs(&reg_addr, &soft_rst_cmd, 1, dev);

    /* As per the datasheet, startup time is 2 ms. */
    dev->delay_ms(2);
  }

  return rslt;
}

static s8 conf_sensor(u8 mode, const struct bmp280_config *conf, struct bmp280_kd_data *dev) {
  s8 rslt        = null_ptr_check(dev);
  u8 temp[2]     = {0, 0};
  u8 reg_addr[2] = {BMP280_CTRL_MEAS_ADDR, BMP280_CONFIG_ADDR};

  if ((rslt == BMP280_OK) && (conf != NULL)) {
    rslt = bmp280_get_regs(BMP280_CTRL_MEAS_ADDR, temp, 2, dev);
    if (rslt == BMP280_OK) {
      /* Here the intention is to put the device to sleep
       * within the shortest period of time
       */
      rslt = bmp280_soft_reset(dev);

      if (rslt == BMP280_OK) {
        temp[0] = BMP280_SET_BITS(temp[0], BMP280_OS_TEMP, conf->os_temp);
        temp[0] = BMP280_SET_BITS(temp[0], BMP280_OS_PRES, conf->os_pres);
        temp[1] = BMP280_SET_BITS(temp[1], BMP280_STANDBY_DURN, conf->odr);
        temp[1] = BMP280_SET_BITS(temp[1], BMP280_FILTER, conf->filter);
        temp[1] = BMP280_SET_BITS_POS_0(temp[1], BMP280_SPI3_ENABLE, conf->spi3w_en);

        rslt = bmp280_set_regs(reg_addr, temp, 2, dev);

        if (rslt == BMP280_OK) {
          dev->conf = *conf;
          if (mode != BMP280_SLEEP_MODE) {
            /* Write only the power mode register in a separate write */

            temp[0] = BMP280_SET_BITS_POS_0(temp[0], BMP280_POWER_MODE, mode);
            rslt    = bmp280_set_regs(reg_addr, temp, 1, dev);
          }
        }
      } else {
        printk(KERN_DEBUG "Failed to soft reset device with code: %d", rslt);
      }
    } else {
      printk(KERN_DEBUG "Failed to get register in conf_sensor with code: %d", rslt);
    }
  } else {
    rslt = BMP280_E_NULL_PTR;
  }

  return rslt;
}

static s8 bmp280_get_config(struct bmp280_config *conf, struct bmp280_kd_data *dev) {
  s8 rslt    = null_ptr_check(dev);
  u8 temp[2] = {0, 0};

  if ((rslt == BMP280_OK) && (conf != NULL)) {
    rslt = bmp280_get_regs(BMP280_CTRL_MEAS_ADDR, temp, 2, dev);

    if (rslt == BMP280_OK) {
      conf->os_temp  = BMP280_GET_BITS(BMP280_OS_TEMP, temp[0]);
      conf->os_pres  = BMP280_GET_BITS(BMP280_OS_PRES, temp[0]);
      conf->odr      = BMP280_GET_BITS(BMP280_STANDBY_DURN, temp[1]);
      conf->filter   = BMP280_GET_BITS(BMP280_FILTER, temp[1]);
      conf->spi3w_en = BMP280_GET_BITS_POS_0(BMP280_SPI3_ENABLE, temp[1]);

      dev->conf = *conf;
    }
  } else {
    rslt = BMP280_E_NULL_PTR;
  }

  return rslt;
}

static s8 bmp280_set_config(const struct bmp280_config *conf, struct bmp280_kd_data *dev) {
  return conf_sensor(BMP280_SLEEP_MODE, conf, dev);
}

static s8 bmp280_init(struct bmp280_kd_data *dev) {
  s8 rslt;
  /* Maximum number of tries before timeout */
  u8 try_count = 5;

  printk(KERN_DEBUG "Init started");

  rslt = null_ptr_check(dev);

  if (rslt == BMP280_OK) {
    while (try_count) {
      rslt = bmp280_get_regs(BMP280_CHIP_ID_ADDR, &dev->chip_id, 1, dev);

      /* Check for chip id validity */
      if ((rslt == BMP280_OK) &&
          (dev->chip_id == BMP280_CHIP_ID1 || dev->chip_id == BMP280_CHIP_ID2 ||
           dev->chip_id == BMP280_CHIP_ID3)) {
        rslt = bmp280_soft_reset(dev);
        printk(KERN_DEBUG "Done Resetting devices");
        if (rslt == BMP280_OK) { rslt = get_calib_param(dev); }
        break;
      }

      /* Wait for 10 ms */
      dev->delay_ms(10);
      --try_count;
    }

    /* Chip id check failed, and timed out */
    if (!try_count) {
      rslt = BMP280_E_DEV_NOT_FOUND;
      printk(KERN_DEBUG "Device not found by init");
    }

    if (rslt == BMP280_OK) {
      /* Set values to default */
      dev->conf.filter   = BMP280_FILTER_OFF;
      dev->conf.os_pres  = BMP280_OS_NONE;
      dev->conf.os_temp  = BMP280_OS_NONE;
      dev->conf.odr      = BMP280_ODR_0_5_MS;
      dev->conf.spi3w_en = BMP280_SPI3_WIRE_DISABLE;
    }
  } else {
    printk(KERN_DEBUG "NULL ptr in init");
  }

  return rslt;
}

static s8 bmp280_get_status(struct bmp280_status *status, const struct bmp280_kd_data *dev) {
  s8 rslt;
  u8 temp;

  rslt = null_ptr_check(dev);

  if ((rslt == BMP280_OK) && (status != NULL)) {
    rslt = bmp280_get_regs(BMP280_STATUS_ADDR, &temp, 1, dev);

    status->measuring = BMP280_GET_BITS(BMP280_STATUS_MEAS, temp);
    status->im_update = BMP280_GET_BITS_POS_0(BMP280_STATUS_IM_UPDATE, temp);
  } else {
    rslt = BMP280_E_NULL_PTR;
  }

  return rslt;
}

static s8 bmp280_get_power_mode(u8 *mode, const struct bmp280_kd_data *dev) {
  s8 rslt;
  u8 temp;

  rslt = null_ptr_check(dev);

  if ((rslt == BMP280_OK) && (mode != NULL)) {
    rslt = bmp280_get_regs(BMP280_CTRL_MEAS_ADDR, &temp, 1, dev);
    if (rslt < 0) { printk(KERN_DEBUG "Error getting power register"); }
    *mode = BMP280_GET_BITS_POS_0(BMP280_POWER_MODE, temp);
    printk(KERN_DEBUG "finished getting power mode");

  } else {
    rslt = BMP280_E_NULL_PTR;
  }

  return rslt;
}

static s8 bmp280_set_power_mode(u8 mode, struct bmp280_kd_data *dev) {
  s8 rslt;

  rslt = null_ptr_check(dev);

  if (rslt == BMP280_OK) { rslt = conf_sensor(mode, &dev->conf, dev); }

  return rslt;
}

static s8 bmp280_get_uncomp_data(struct bmp280_uncomp_data *  uncomp_data,
                                 const struct bmp280_kd_data *dev) {
  s8 rslt;
  u8 temp[6] = {0};

  rslt = null_ptr_check(dev);

  if ((rslt == BMP280_OK) && (uncomp_data != NULL)) {
    rslt = bmp280_get_regs(BMP280_PRES_MSB_ADDR, temp, 6, dev);
    uncomp_data->uncomp_press =
        (s32)((((u32)(temp[0])) << 12) | (((u32)(temp[1])) << 4) | ((u32)temp[2] >> 4));

    uncomp_data->uncomp_temp =
        (s32)((((s32)(temp[3])) << 12) | (((s32)(temp[4])) << 4) | (((s32)(temp[5])) >> 4));
  } else {
    rslt = BMP280_E_NULL_PTR;
  }

  return rslt;
}

/*!
 * @brief This API is used to get the compensated temperature from
 * uncompensated temperature. This API uses 32 bit integers.
 */
static s32 bmp280_comp_temp_32bit(s32 uncomp_temp, struct bmp280_kd_data *dev, s32 *output_val) {
  s32 var1;
  s32 var2;
  s8  rslt;

  rslt = null_ptr_check(dev);

  if (rslt == BMP280_OK && NULL != output_val) {
    var1 = ((((uncomp_temp >> 3) - ((s32)dev->calib_param.dig_t1 << 1))) *
            ((s32)dev->calib_param.dig_t2)) >>
           11;
    var2 = (((((uncomp_temp >> 4) - ((s32)dev->calib_param.dig_t1)) *
              ((uncomp_temp >> 4) - ((s32)dev->calib_param.dig_t1))) >>
             12) *
            ((s32)dev->calib_param.dig_t3)) >>
           14;

    dev->calib_param.t_fine = var1 + var2;
    *output_val             = (dev->calib_param.t_fine * 5 + 128) >> 8;
  } else {
    printk(KERN_DEBUG "NULL pointer in comp temp");
    return BMP280_E_NULL_PTR;
  }

  return BMP280_OK;
}

/*!
 * @brief This API is used to get the compensated pressure from
 * uncompensated pressure. This API uses 32 bit integers.
 */
static s32 bmp280_comp_pres_32bit(s32                          uncomp_pres,
                                  const struct bmp280_kd_data *dev,
                                  u32 *                        output_val) {
  s32 var1;
  s32 var2;
  u32 pressure = 0;
  s8  rslt;

  rslt = null_ptr_check(dev);

  if (rslt == BMP280_OK && NULL != output_val) {
    var1 = (((s32)dev->calib_param.t_fine) >> 1) - (s32)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((s32)dev->calib_param.dig_p6);
    var2 = var2 + ((var1 * ((s32)dev->calib_param.dig_p5)) << 1);
    var2 = (var2 >> 2) + (((s32)dev->calib_param.dig_p4) << 16);
    var1 = (((dev->calib_param.dig_p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) +
            ((((s32)dev->calib_param.dig_p2) * var1) >> 1)) >>
           18;
    var1     = ((((32768 + var1)) * ((s32)dev->calib_param.dig_p1)) >> 15);
    pressure = (((u32)(((s32)1048576) - uncomp_pres) - (var2 >> 12))) * 3125;

    /* Avoid exception caused by division with zero */
    if (var1 != 0) {
      /* Check for overflows against UINT32_MAX/2; if pres is left-shifted by 1
       */
      if (pressure < 0x80000000)
        pressure = (pressure << 1) / ((u32)var1);
      else
        pressure = (pressure / (u32)var1) * 2;

      var1 =
          (((s32)dev->calib_param.dig_p9) * ((s32)(((pressure >> 3) * (pressure >> 3)) >> 13))) >>
          12;
      var2     = (((s32)(pressure >> 2)) * ((s32)dev->calib_param.dig_p8)) >> 13;
      pressure = (u32)((s32)pressure + ((var1 + var2 + dev->calib_param.dig_p7) >> 4));
    } else {
      pressure = 0;
    }
    *output_val = pressure;
  }

  return rslt;
}

enum bmp280_kd_chan {
  TEMP,
  PRESSURE,
};

static const struct iio_chan_spec bmp280_kd_channels[] = {
    [TEMP] =
        {
            .type                     = IIO_TEMP,
            .info_mask_separate       = BIT(IIO_CHAN_INFO_PROCESSED),
            .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
        },
    [PRESSURE] =
        {
            .type               = IIO_PRESSURE,
            .info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED),
        },
};

static int bmp280_kd_read_raw(
    struct iio_dev *indio_dev, struct iio_chan_spec const *chan, int *val, int *val2, long mask) {
  printk(KERN_DEBUG "bmp280_kd reading raw");

  struct bmp280_kd_data *data = iio_priv(indio_dev);
  s32                    ret  = 0;

  mutex_lock(&data->lock);
  switch (chan->type) {
    case IIO_TEMP:
      switch (mask) {
        case IIO_CHAN_INFO_PROCESSED: {
          struct bmp280_uncomp_data uncomp_data = {};
          ret                                   = bmp280_get_uncomp_data(&uncomp_data, data);
          if (ret < 0) { goto out; }
          ret = bmp280_comp_temp_32bit(uncomp_data.uncomp_temp, data, val) ? -ENXIO : IIO_VAL_INT;
          goto out;

        } break;

        case IIO_CHAN_INFO_SCALE:
          *val = 10;
          ret  = IIO_VAL_INT;
          goto out;
          break;
      }
      break;

    case IIO_PRESSURE:
      switch (mask) {
        case IIO_CHAN_INFO_PROCESSED: {
          struct bmp280_uncomp_data uncomp_data = {};
          ret                                   = bmp280_get_uncomp_data(&uncomp_data, data);
          if (ret < 0) { goto out; }
          ret = bmp280_comp_pres_32bit(uncomp_data.uncomp_press, data, val) ? -ENXIO : IIO_VAL_INT;
          goto out;
          break;
        }
      }

    default:
      ret = -EINVAL;
  }

out:
  mutex_unlock(&data->lock);
  return ret;
}

static struct attribute *bmp280_kd_attributes[] = {
    NULL,
};

static const struct attribute_group bmp280_kd_attribute_group = {
    .attrs = bmp280_kd_attributes,
};

static const struct iio_info bmp280_kd_iio_info = {
    .attrs    = &bmp280_kd_attribute_group,
    .read_raw = bmp280_kd_read_raw,
};

static int bmp280_kd_probe(struct i2c_client *client, const struct i2c_device_id *id) {
  struct bmp280_kd_data *chip;
  struct iio_dev *       indio_dev;
  s32                    ret;

  printk(KERN_INFO "bmp280_kd probe started");

  chip = NULL;
  ret  = 0;

  indio_dev = devm_iio_device_alloc(&client->dev, sizeof(struct bmp280_kd_data));
  if (!indio_dev) { return -ENOMEM; }

  chip = iio_priv(indio_dev);
  if (NULL == chip) { printk(KERN_DEBUG "chip is NULL in probe"); }
  mutex_init(&chip->lock);
  chip->client   = client;
  chip->dev_id   = 0;
  chip->intf     = BMP280_I2C_INTF;
  chip->read     = i2c_read_wrapper;
  chip->write    = i2c_write_wrapper;
  chip->delay_ms = msleep;

  // configure bmp settings
  ret                 = bmp280_init(chip);
  chip->conf.os_temp  = BMP280_OS_2X;
  chip->conf.os_pres  = BMP280_OS_16X;
  chip->conf.odr      = BMP280_ODR_62_5_MS;
  chip->conf.filter   = BMP280_FILTER_COEFF_4;
  chip->conf.spi3w_en = BMP280_SPI3_WIRE_DISABLE;
  bmp280_set_config(&chip->conf, chip);
  bmp280_set_power_mode(BMP280_NORMAL_MODE, chip);

  /* this is only used for device removal purposes */
  i2c_set_clientdata(client, indio_dev);

  /* Establish that the iio_dev is a child of the i2c device */
  indio_dev->name         = id->name;
  indio_dev->dev.parent   = &client->dev;
  indio_dev->info         = &bmp280_kd_iio_info;
  indio_dev->channels     = bmp280_kd_channels;
  indio_dev->num_channels = ARRAY_SIZE(bmp280_kd_channels);
  indio_dev->modes        = INDIO_DIRECT_MODE;

  ret = devm_iio_device_register(indio_dev->dev.parent, indio_dev);
  if (ret) {
    printk(KERN_ALERT "Failed at registering iio device");
    return ret;
  }

  printk(KERN_INFO "Done Probing bmp280");
  return 0;
}

static int bmp280_kd_remove(struct i2c_client *client) {
  printk(KERN_INFO "Removing bmp280 Driver");
  return 0;
}

static const struct of_device_id bmp280_kd_of_match[] = {
    {
        .compatible = MANUFACTURER "," DRIVER_NAME,
    },
    {},
};
MODULE_DEVICE_TABLE(of, bmp280_kd_of_match);

static const struct i2c_device_id bmp280_kd_id[] = {{DRIVER_NAME, 0}, {}};
MODULE_DEVICE_TABLE(i2c, bmp280_kd_id);

static struct i2c_driver bmp280_kd_driver = {.probe    = bmp280_kd_probe,
                                             .remove   = bmp280_kd_remove,
                                             .id_table = bmp280_kd_id,
                                             .driver   = {
                                                 .owner          = THIS_MODULE,
                                                 .name           = DRIVER_NAME,
                                                 .of_match_table = of_match_ptr(bmp280_kd_of_match),
                                             }};

static const struct i2c_board_info bmp280_kd_info __initdata = {
    I2C_BOARD_INFO(DRIVER_NAME, BMP280_I2C_ADDR_SEC),
};

static int __init bmp280_kd_init(void) {
  printk(KERN_INFO "Initting bmp280 Driver");

#ifdef DEBUG
  printk(KERN_DEBUG "Trying to create new I2C devices");
  struct i2c_adapter *adapter = NULL;
  // create new devices, will log an error if already exists one but otw will work
  adapter = i2c_get_adapter(1);
  if (NULL == adapter) {
    printk(KERN_DEBUG "Can't find adapter");
  } else {
    i2c_new_device(adapter, &bmp280_kd_info);
  }
#endif

  return i2c_add_driver(&bmp280_kd_driver);
}
module_init(bmp280_kd_init);

static void __exit bmp280_kd_cleanup(void) { i2c_del_driver(&bmp280_kd_driver); };
module_exit(bmp280_kd_cleanup);

MODULE_AUTHOR("Khoi Trinh <khoidinhtrinh@gmail.com>");
MODULE_DESCRIPTION("BMP280 kernel driver for the cat detector");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");