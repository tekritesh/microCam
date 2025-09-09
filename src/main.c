/*
* HM01B0 capture example – interrupt driven, DMA-ready version
*
* Copyright (c) 2024 Open Pixel Systems
* SPDX-License-Identifier: Apache-2.0
*/




#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>


#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/atomic.h>
#include <hal/nrf_power.h>
#include <hal/nrf_clock.h>



//#include <drivers/spi/spi_nrfx_spis.h>

#include <zephyr/drivers/pwm.h>




#include "HM01B0/HM01B0_def_values.h"
#include "HM01B0/HM01B0_CAPTURE.h"
#include "HM01B0/HM01B0_BLE.h"

struct k_fifo my_fifo_uart;
struct k_fifo my_fifo_ble;



int main(void)
{

    //printk("HM01B0 Capture Example\n");
    k_fifo_init(&my_fifo_uart);
    k_fifo_init(&my_fifo_ble);
    setup_cam_capture(&my_fifo_uart, &my_fifo_ble);
    setup_cam_capture_fifo(&my_fifo_uart);
    hm_ble_init(&my_fifo_ble);
    /* leave this on forever */
    //spispec.config = spi_cfg_pwr1;
    //hm_ble_init(&my_fifo);
    if (!device_is_ready(uart_dev)      ||
        !device_is_ready(dev_i2c.bus)   ||
        !device_is_ready(gpio0_dev)     ||
        !device_is_ready(spis_dev.bus)) {
        printk("Device not ready\n");
        return -ENODEV;
    }


    //run_peripheral_step(0);
/* --- CSN pin -------------------------------------------------------- */
    uint8_t addr_readback[2];
    uint8_t val;

    /* BIT_CONTROL should read back 0x20 for 1-bit serial mode. */
    addr_readback[0] = 0x03; addr_readback[1] = 0x50;
    if (i2c_write_read_dt(&dev_i2c, addr_readback, 2, &val, 1) == 0)
        printk("QVGA win (0x0350) = 0x%02X\n", val);

    /* OSC_CLK_DIV should read back 0x00 (MSB-first, ÷8, non-gated). */
    addr_readback[0] = 0x03; addr_readback[1] = 0x81;
    if (i2c_write_read_dt(&dev_i2c, addr_readback, 2, &val, 1) == 0)
        printk("BIN_RDOUT_Y (0x0381) = 0x%02X\n", val);

    /* TEST_PATTERN_MODE – should be 0x00 (disabled) */
    addr_readback[0] = 0x10; addr_readback[1] = 0x06;
    if (i2c_write_read_dt(&dev_i2c, addr_readback, 2, &val, 1) == 0)
        printk("IMG_MODE_SEL (0x1006) = 0x%02X\n", val);
    /* ------------------------- main loop ---------------------------------- */
    printk("Starting capture loop\n");
    stream_photos();
}
