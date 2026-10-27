#include <gpio_led.h>

const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static struct k_work_delayable blink_work;
static bool led_is_on;

/* ============================================
 * Blink Timer Callback
 * ============================================ */
static void blink_timeout(struct k_work *work)
{
	led_is_on = !led_is_on;
	gpio_pin_set_dt(&led, (int)led_is_on);
	k_work_schedule(&blink_work, BLINK_INTERVAL);
}

/* ============================================
 * LED 초기 설정
 * ============================================ */
int blink_setup(void)
{
	if (!gpio_is_ready_dt(&led)) {
		printk("LED device not ready\n");
		return -EIO;
	}

	int err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (err) {
		printk("LED pin configuration failed (err %d)\n", err);
		return err;
	}

	k_work_init_delayable(&blink_work, blink_timeout);
	printk("LED setup complete\n");
	return 0;
}

/* ============================================
 * LED 깜빡임 시작
 * ============================================ */
void blink_start(void)
{
	printk("Start blinking LED...\n");
	led_is_on = false;
	gpio_pin_set_dt(&led, (int)led_is_on);
	k_work_schedule(&blink_work, BLINK_INTERVAL);
}

/* ============================================
 * LED 깜빡임 정지
 * ============================================ */
void blink_stop(void)
{
	struct k_work_sync work_sync;
	printk("Stop blinking LED.\n");

	k_work_cancel_delayable_sync(&blink_work, &work_sync);

	/* 정지 시 LED OFF 유지 */
	led_is_on = false;
	gpio_pin_set_dt(&led, (int)led_is_on);
}