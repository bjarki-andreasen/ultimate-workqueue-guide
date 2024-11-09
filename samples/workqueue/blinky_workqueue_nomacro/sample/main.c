/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#define SAMPLE_SLEEP_MS 500

static struct k_work_delayable sample_dwork;
static bool sample_on = true;

static void sample_work_handler(struct k_work *work)
{
	printk("LED0: %s\n", sample_on ? "ON" : "OFF");
	sample_on = !sample_on;
	k_work_schedule(&sample_dwork, K_MSEC(SAMPLE_SLEEP_MS));
}

static int sample_work_init(void)
{
	k_work_init_delayable(&sample_dwork, sample_work_handler);
	k_work_schedule(&sample_dwork, K_MSEC(SAMPLE_SLEEP_MS));
	return 0;
}

SYS_INIT(sample_work_init, APPLICATION, 99);

int main(void)
{
	/*
	 * We have nothing to use the main thread for
	 * Would be nice if we could reuse it maybe? (foreshadowing)
	 */
	return 0;
}
