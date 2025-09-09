#include "HM01B0_UART.h"

#define UART_THREAD_STACK_SZ 768

static struct k_fifo *the_new_fifo;

K_THREAD_STACK_DEFINE(uart_stack, UART_THREAD_STACK_SZ);
static struct k_thread uart_thread_data;

void send_frame_over_uart_binary(uint8_t *image)
{
static const char hdr[]  = "<FRAME>\n";
static const char tail[] = "</FRAME>\n";

for (int i = 0; i < sizeof hdr - 1; i++)  uart_poll_out(uart_dev, hdr[i]);
for (size_t i = 0; i < IMAGE_SIZE; i++)
{
    /* skip first and last byte of each line 
    size_t col = i % IMG_WIDTH;
    if (col == 0 || col == (IMG_WIDTH - 1)) {
        continue;
    }
    uart_poll_out(uart_dev, image[i]);
    */
   uart_poll_out(uart_dev, image[i]);
   if ((i & 0x7F) == 0x7F) {
       /* Periodically yield to let lower-priority threads (e.g. logger) run */
       k_yield();
   }
}
for (int i = 0; i < sizeof tail - 1; i++) uart_poll_out(uart_dev, tail[i]);
}


void setup_cam_capture_fifo(struct k_fifo *fifo)
{
    the_new_fifo = fifo;

    k_thread_create(&uart_thread_data, uart_stack, UART_THREAD_STACK_SZ,
                    streaming_photos_fifo, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
}

void streaming_photos_fifo(void *p1, void *p2, void *p3)
{
    while (1) {
        frame_item_t *item = k_fifo_get(the_new_fifo, K_FOREVER);
        if (item == NULL) {
            continue;
        }
        send_frame_over_uart_binary(item->data);
        if (item->consumed_sem) {
            k_sem_give(item->consumed_sem);
        }
        /* Always free the FIFO wrapper */
        k_free(item);
    }
}
