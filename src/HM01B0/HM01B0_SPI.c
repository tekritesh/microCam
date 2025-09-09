#include "HM01B0_SPI.h"
#include <nrfx_gpiote.h>
#include <errno.h>
#include <zephyr/irq.h>
#include <soc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/drivers/spi.h>

static nrf_ppi_channel_t gate_ppi_ch;

/* Zephyr SPI DT spec for the SPIS node so call sites can use
 * spis_dev.bus (device) and &spis_dev.config (config) with spi_* APIs.
 */
struct spi_dt_spec spis_dev = SPI_DT_SPEC_GET(SPI_NODE, SPI_OP, 0);



static void scope_pin_init(void)
{
    /* Configure a GPIOTE task on PIN_OUT so we can scope SPIS END events */
    NRF_GPIO->PIN_CNF[PIN_OUT] =
        (GPIO_PIN_CNF_DIR_Output     << GPIO_PIN_CNF_DIR_Pos)  |
        (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos);

    /* Use GPIOTE channel GPIOTE_CH as a task to toggle PIN_OUT */
    NRF_GPIOTE->CONFIG[GPIOTE_CH] =
        (GPIOTE_CONFIG_MODE_Task          << GPIOTE_CONFIG_MODE_Pos) |
        (PIN_OUT                          << GPIOTE_CONFIG_PSEL_Pos)  |
        (GPIOTE_CONFIG_POLARITY_Toggle    << GPIOTE_CONFIG_POLARITY_Pos) |
        (GPIOTE_CONFIG_OUTINIT_Low        << GPIOTE_CONFIG_OUTINIT_Pos);

    static nrf_ppi_channel_t ch;
    nrfx_gppi_channel_alloc(&ch);
    nrfx_gppi_event_endpoint_setup(ch, nrf_spis_event_address_get(NRF_SPIS1, NRF_SPIS_EVENT_ACQUIRED ));
    nrfx_gppi_task_endpoint_setup(ch, nrf_gpiote_task_address_get(NRF_GPIOTE, (nrf_gpiote_task_t)(NRF_GPIOTE_TASK_OUT_0 + GPIOTE_CH)));
    nrfx_gppi_channels_enable(BIT(ch));
}

void gate_trigger_init(void)
{
    NRF_GPIO->PIN_CNF[PIN_GATE] =
        (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos)  |
        (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos)|
        (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos);

    /* Acquire on CSN rising (Lo->Hi) */
    NRF_GPIOTE->CONFIG[GPIOTE_CH_GATE] =
        (GPIOTE_CONFIG_MODE_Event      << GPIOTE_CONFIG_MODE_Pos) |
        (PIN_GATE                      << GPIOTE_CONFIG_PSEL_Pos) |
        (GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos);

    nrfx_gppi_channel_alloc(&gate_ppi_ch);
    nrfx_gppi_event_endpoint_setup(gate_ppi_ch,
        nrf_gpiote_event_address_get(NRF_GPIOTE, (nrf_gpiote_event_t)(NRF_GPIOTE_EVENT_IN_0 + GPIOTE_CH_GATE)));
    nrfx_gppi_task_endpoint_setup(gate_ppi_ch,
        nrf_spis_task_address_get(NRF_SPIS1, NRF_SPIS_TASK_ACQUIRE));
    nrfx_gppi_channels_enable(BIT(gate_ppi_ch));

}


static void release_trigger_init(void)
{
    /* Release on CSN falling (Hi->Lo) */
    static nrf_ppi_channel_t rel_ppi_ch;
    NRF_GPIOTE->CONFIG[GPIOTE_CH_HSYNC] =
        (GPIOTE_CONFIG_MODE_Event      << GPIOTE_CONFIG_MODE_Pos) |
        (PIN_GATE                      << GPIOTE_CONFIG_PSEL_Pos) |
        (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);

    nrfx_gppi_channel_alloc(&rel_ppi_ch);
    nrfx_gppi_event_endpoint_setup(rel_ppi_ch,
        nrf_gpiote_event_address_get(NRF_GPIOTE, (nrf_gpiote_event_t)(NRF_GPIOTE_EVENT_IN_0 + GPIOTE_CH_HSYNC)));
    nrfx_gppi_task_endpoint_setup(rel_ppi_ch,
        nrf_spis_task_address_get(NRF_SPIS1, NRF_SPIS_TASK_RELEASE));
    nrfx_gppi_channels_enable(BIT(rel_ppi_ch));
}

static void setup_GPIOTE_trigger(void)
{
    /* Release on CSN falling (Hi->Lo) */
    static nrf_ppi_channel_t rel_ppi_ch;
    NRF_GPIOTE->CONFIG[GPIOTE_CH_HSYNC] =
        (GPIOTE_CONFIG_MODE_Event      << GPIOTE_CONFIG_MODE_Pos) |
        (PIN_GATE                      << GPIOTE_CONFIG_PSEL_Pos) |
        (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);

    nrfx_gppi_channel_alloc(&rel_ppi_ch);
    nrfx_gppi_event_endpoint_setup(rel_ppi_ch,
        nrf_gpiote_event_address_get(NRF_GPIOTE, (nrf_gpiote_event_t)(NRF_GPIOTE_EVENT_IN_0 + GPIOTE_CH_HSYNC)));
    nrfx_gppi_task_endpoint_setup(rel_ppi_ch,
        nrf_spis_task_address_get(NRF_SPIS1, NRF_SPIS_TASK_RELEASE));
    nrfx_gppi_channels_enable(BIT(rel_ppi_ch));
}





void hm_spi_init(void)
{
    /* Keep the system in constant‑latency mode to avoid first‑byte corruption */
    NRF_POWER->TASKS_CONSTLAT = 1;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
    }
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

    scope_pin_init();
    /* Ensure SPIS ACQUIRE/RELEASE tasks are correctly gated by the LVLD/CSN
     * signal so each line transfer starts and ends cleanly.
     */
    gate_trigger_init();
    release_trigger_init();

    /* Configure nrfx SPIS1 in mode 3, MSB first. HM01B0 drives:
     *  - SCK  on PIN_PCLK_SCK
     *  - MOSI on PIN_D0_MOSI
     *  - CSN  on PIN_LVLD_CSN (LVLD)
     * MISO is unused.
     */

}
