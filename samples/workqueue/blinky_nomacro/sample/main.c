/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#define SAMPLE_SLEEP_MS 500

static void sample_thread_routine(void *arg0, void *arg1, void *arg2)
{
	bool on = true;

	while (1) {
		printk("LED0: %s\n", on ? "ON" : "OFF");
		on = !on;
		k_msleep(SAMPLE_SLEEP_MS);
	}
}

K_THREAD_DEFINE(
	sample_thread,
	1024,
	sample_thread_routine,
	NULL,
	NULL,
	NULL,
	(CONFIG_MAIN_THREAD_PRIORITY + 1),
	0,
	0
);

int main(void)
{
	/*
	 * We have nothing to use the main thread for
	 * Would be nice if we could reuse it maybe? (foreshadowing)
	 */
	return 0;
}
