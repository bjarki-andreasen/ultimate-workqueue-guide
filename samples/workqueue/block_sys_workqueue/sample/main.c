/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

static struct k_work foo_work0;
static struct k_work foo_work1;
static struct k_sem foo_work1_done;

static void foo_work1_handler(struct k_work *work)
{
	printk("running work1\n");
	k_sem_give(&foo_work1_done);
	printk("leave work1\n");
}

static void foo_work_sync(void)
{
	int ret;

	printk("submit work1\n");
	k_work_submit(&foo_work1);

	printk("wait for work1 done\n");
	ret = k_sem_take(&foo_work1_done, K_SECONDS(10));
	if (ret < 0) {
		printk("timed out waiting for work1 done\n");
	} else {
		printk("work1 done\n");
	}
}

static void foo_work0_handler(struct k_work *work)
{
	printk("running work0\n");

	/*
	 * Doing any blocking work from a work handler is illegal.
	 */
	foo_work_sync();

	printk("leave work0\n");
}

static void foo_init(void)
{
	k_work_init(&foo_work0, foo_work0_handler);
	k_work_init(&foo_work1, foo_work1_handler);
	k_sem_init(&foo_work1_done, 0, 1);
}

int main(void)
{
	foo_init();
	k_work_submit(&foo_work0);
}
