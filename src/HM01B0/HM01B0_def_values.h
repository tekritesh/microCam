/*
 * HM01B0Regs.h
 *
 *  Created on: July 25th, 2025
 *      Author: Elliott Ory
 */


#ifndef HM01B0DEFVALUES_H_
#define HM01B0DEFVALUES_H_
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#define SPI_OP (SPI_WORD_SET(8) | SPI_OP_MODE_SLAVE | SPI_TRANSFER_MSB)

#define PIN_OUT       28            /* debug pin A (ACQUIRED) */
#define PIN_OUT1       5            /* debug pin B (RELEASE)  */

#define OTHER_PINS       31            /* any free GPIO          */
#define GPIOTE_CH     0
/* Dedicated GPIOTE channels for scope/debug (avoid clashes with Zephyr GPIO IRQs
 * and the MCLK GPIOTE task used by the clock generator). The nRF52832 has 8 GPIOTE
 * channels (0..7). We let Zephyr's GPIO driver use the lower channels for pin
 * interrupts, the MCLK code uses task channel 5, and we keep scope/gate/release on
 * the upper channels.
 */
#define GPIOTE_CH_SCOPE_ACQ 6
#define GPIOTE_CH_SCOPE_REL 7
#define PPI_CH        0

#define IMG_WIDTH 160
#define IMG_HEIGHT 119
#define IMAGE_SIZE (IMG_WIDTH * IMG_HEIGHT)


#define PIN_MCLK 3
#define MCLK_TIMER_INSTANCE 3

/* Pin mapping – LVLD (HSYNC) is used as SPIS-CSN as well                    */
#define PIN_VSYNC        27          /* FVLD from HM01B0                     */
#define PIN_LVLD_CSN     7          /* LVLD → CSN to SPIS0                  */
#define PIN_PCLK_SCK      15          /* PCLK → SCK to SPIS0                  */
#define PIN_D0_MOSI       16          /* D0   → MOSI to SPIS0                 */

#define CAM_FRAME_VALID                   PIN_VSYNC
#define CAM_LINE_VALID                    PIN_LVLD_CSN
//#define CAM_INT                           NRF_GPIO_PIN_MAP(0, 9)   // Input
#define CAM_MCLK_IN_FROM_MCU              PIN_MCLK


#define CAM_D0          PIN_D0_MOSI

#define CAM_SPI_CS_OUT   PIN_LVLD_CSN

#define PIN_GATE          PIN_LVLD_CSN         /* start capture when high */
#define GPIOTE_CH_GATE   4
#define PPI_CH_GATE      1

#define PIN_PCLK          PIN_PCLK_SCK
#define GPIOTE_CH_PCLK    3
#define PPI_CH_PCLK       3

/* HSYNC → timer STOP bridge */
#define PIN_HSYNC         7
#define GPIOTE_CH_HSYNC   7              /* free GPIOTE channel (upper range) */
#define PPI_CH_REL        2              /* PPI channel for TASKS_STOP      */

/* Peripherals from DTS */
#define UART_NODE       DT_NODELABEL(uart0)
#define I2C_NODE        DT_ALIAS(hmm)
#define SPI_NODE        DT_NODELABEL(gendev)

/* nRF52832 RAM span that is reachable by EasyDMA (64 kB) */
#define DMA_RAM_START   0x20000000u
#define DMA_RAM_END     0x20010000u


/* FIFO item wrapper for passing image buffers through k_fifo.
 * The first field must be reserved for Zephyr's use.
 */
typedef struct frame_item {
    void *fifo_reserved;
    uint8_t *data;
    size_t len;
    /* Optional: if non-NULL, the consumer should k_sem_give() this when done
     * with the frame to let the producer know it can enqueue the next frame.
     */
    struct k_sem *consumed_sem;
} frame_item_t;






#endif /* HM01B0DEFVALUES_H_ */
