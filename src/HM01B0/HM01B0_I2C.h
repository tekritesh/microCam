#ifndef HM01B0I2C_H_
#define HM01B0I2C_H_

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include "HM01B0Regs.h"
#include "HM01B0_def_values.h"


static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C_NODE);
void init_cam(void);
void hm_i2c_write(uint16_t reg, uint8_t val);



#endif /* HM01B0I2C_H_ */
