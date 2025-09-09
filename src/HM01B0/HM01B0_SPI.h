#ifndef HM01B0_SPI_H_
#define HM01B0_SPI_H_

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>
#include <hal/nrf_spis.h>
#include <nrfx_spis.h>
#include <zephyr/kernel.h>
#include <nrfx_gpiote.h>
#include <nrfx_ppi.h>
#include <helpers/nrfx_gppi.h>
#include "HM01B0_def_values.h"

/* Expose Zephyr SPI DT spec so call sites can pass driver, then spi_config */
extern struct spi_dt_spec spis_dev;

void hm_spi_init(void);
/* Register a semaphore that will be given from the SPIS transfer-done handler */
void hm_spi_set_line_sem(struct k_sem *sem);



/* Arm the SPIS RX for the next transfer (non-blocking).
 * Returns 0 on success, negative on error.
 */
int hm_spis_arm_rx(uint8_t *buf, size_t len);


#endif /* HM01B0_SPI_H_ */
