#include <uart_bridge.h>

LOG_MODULE_REGISTER(uart_bridge_c, LOG_LEVEL_INF);

#define DATA_UART_NODE DT_NODELABEL(uart0)

static volatile bool tx_busy;
static uint8_t tx_buf[UART_FRAME_LEN];

const struct device *uart_dev;

/* 10-byte frame × 64 */
K_MSGQ_DEFINE(uart_tx_q, UART_FRAME_LEN, 64, 4);


static void uart_evt_cb(const struct device *dev,
                        struct uart_event *evt,
                        void *user_data)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(user_data);

    switch (evt->type) {

    case UART_TX_DONE:
        tx_busy = false;
        uart_tx_kick();
        break;
    case UART_TX_ABORTED:
        tx_busy = false;
        LOG_WRN("UART TX aborted");
        uart_tx_kick();
        break;

    default:
        break;
    }
}


void uart_tx_kick(void)
{
    if (tx_busy || !uart_dev) {
        return;
    }

    if (k_msgq_get(&uart_tx_q, tx_buf, K_NO_WAIT) != 0) {
        return;
    }

    tx_busy = true;

    int rc = uart_tx(uart_dev,
                     tx_buf,
                     UART_FRAME_LEN,
                     SYS_FOREVER_MS);

    if (rc != 0) {
        tx_busy = false;
        LOG_ERR("UART TX failed: rc=%d", rc);
    }
}


int uart_bridge_init(void)
{
    uart_dev = DEVICE_DT_GET(DATA_UART_NODE);

    if (!device_is_ready(uart_dev)) {
        return -ENODEV;
    }

    int rc = uart_callback_set(uart_dev,
                               uart_evt_cb,
                               NULL);

    if (rc) {
        return rc;
    }

    tx_busy = false;

    LOG_INF("UART bridge initialized");

    return 0;
}