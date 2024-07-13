/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

static void foo_process_work_handler(struct k_work *work)
{
	printk("hello from process work handler\n");
}

static void foo_irq_handler(void)
{
	k_work_submit(&process_work);
}

static void foo_init(void)
{
	k_work_init(&process_work, foo_process_work_handler);
}

int main(void)
{
	foo_init();

	/*
	 * The work queue is configured with lower prio than the "ISR thread"
	 * (main thread). The work can be resubmitted, and will be run once
	 * when the work queue thread gets to run.
	 */
	foo_irq_handler();
	foo_irq_handler();
	foo_irq_handler();
}
