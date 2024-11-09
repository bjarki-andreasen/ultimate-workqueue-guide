/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

struct k_work_q k_sys_work_q;
static bool blinky_on;
static struct k_work_delayable blinky_dwork;

static void sample_blinky_handler(struct k_work *work)
{
	printk("LED %s\n", blinky_on ? "ON" : "OFF");
	blinky_on = !blinky_on;
	k_work_schedule(&blinky_dwork, K_SECONDS(1));
}

int main(void)
{
	k_work_init_delayable(&blinky_dwork, sample_blinky_handler);
	k_work_schedule(&blinky_dwork, K_SECONDS(1));
	k_work_queue_run(&k_sys_work_q);
}
