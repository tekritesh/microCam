#include "HM01B0_BLE.h"

extern struct bt_conn *default_conn;


#define BLE_THREAD_STACK_SZ 768

static struct k_fifo *the_fifo;

/* BLE manager thread (handles advertising/connection state) */
K_THREAD_STACK_DEFINE(ble_mgr_stack, BLE_THREAD_STACK_SZ);
static struct k_thread ble_mgr_thread_data;

/* BLE streaming thread (drains FIFO and notifies over BLE) */
K_THREAD_STACK_DEFINE(ble_stream_stack, BLE_THREAD_STACK_SZ);
static struct k_thread ble_stream_thread_data;

/* Forward for manager entry */
static void ble_manager_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    /* Run BLE peripheral state machine forever */
    run_peripheral_step(0);
}

void hm_ble_init(struct k_fifo *fifo)
{
    the_fifo = fifo;

    /* Initialize BLE stack and start advertising */
    init_ble();

    /* Start BLE core (advertising/connection/MTU updates) */
    k_thread_create(&ble_mgr_thread_data, ble_mgr_stack, BLE_THREAD_STACK_SZ,
                    ble_manager_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);

    /* Start BLE streaming worker (pulls frames from FIFO and sends) */
    k_thread_create(&ble_stream_thread_data, ble_stream_stack, BLE_THREAD_STACK_SZ,
                    streaming_photos_ble_fifo, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
}

void streaming_photos_ble_fifo(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    while (1) {
        frame_item_t *item = k_fifo_get(the_fifo, K_FOREVER);
        if (item == NULL) {
            continue;
        }

        send_large_data(item->data);

        if (item->consumed_sem) {
            k_sem_give(item->consumed_sem);
        }
        /* Always free the FIFO wrapper */
        k_free(item);
    }
}
