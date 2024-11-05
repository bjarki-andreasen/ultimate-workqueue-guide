/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

static void sample_work_handler(void)
{
	printk("hello from process work handler " STRINGIFY(inst) "\n");
}

static K_SEM_DEFINE(sample_sem, 0, 1);

static void sample_thread_routine(void *arg0, void *arg1, void *arg2)
{
	while (1) {
		(void)k_sem_take(&sample_sem, K_FOREVER);
		sample_work_handler();
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

static void foo_irq_handler(void)
{
	k_sem_give(&sample_sem);
}

int main(void)
{
	/*
	 * The process thread is configured with lower prio than the "ISR thread"
	 * (main thread). The work can be resubmitted, and will be run once
	 * when the work queue thread gets to run.
	 */
	foo_irq_handler();
	foo_irq_handler();
	foo_irq_handler();
}
