# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(HIMAX-ZEPHYR-INTEGRATION-TESTING)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/HM01B0/HM01B0_CLK.c)
target_sources(app PRIVATE src/HM01B0/HM01B0_I2C.c)
target_sources(app PRIVATE src/HM01B0/HM01B0_SPI.c)
target_sources(app PRIVATE src/HM01B0/HM01B0_CAPTURE.c)
target_sources(app PRIVATE src/HM01B0/HM01B0_BLE.c)
target_sources(app PRIVATE src/HM01B0/HM01B0_UART.c)
target_sources(app PRIVATE src/spis_preinit.c)
target_sources(app PRIVATE src/ble_manager.c)


