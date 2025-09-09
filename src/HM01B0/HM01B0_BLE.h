#ifndef HM01B0_BLE_H
#define HM01B0_BLE_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>

#include "HM01B0_def_values.h"

void hm_ble_init(struct k_fifo *fifo);
void streaming_photos_ble_fifo(void *p1, void *p2, void *p3);
extern void run_peripheral_step( uint16_t seconds);
/* Send a buffer over the BLE TX characteristic, chunked by MTU.
 * Returns 0 on success, or a negative error code (e.g. -ENOTCONN).
 */
extern void send_large_data(uint8_t *image);
void init_ble(void);


#endif /* HM01B0_BLE_H */
