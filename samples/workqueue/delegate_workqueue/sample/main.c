/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#define GET_PROCESS_NAME(inst, name) \
	_CONCAT_3(process_, name, inst)

#define DEFINE_PROCESS_WORK_HANDLER(inst)						\
	static void GET_PROCESS_NAME(inst, work_handler)(struct k_work *work)		\
	{										\
		printk("hello from process work handler " STRINGIFY(inst) "\n");	\
	}

#define DEFINE_PROCESS_WORK(inst)							\
	static K_WORK_DEFINE(								\
		GET_PROCESS_NAME(inst, work),						\
		GET_PROCESS_NAME(inst, work_handler)					\
	);

#define DEFINE_PROCESS_INSTANCE(inst, _)						\
	DEFINE_PROCESS_WORK_HANDLER(inst)						\
	DEFINE_PROCESS_WORK(inst)							\

LISTIFY(CONFIG_SAMPLE_INST_COUNT, DEFINE_PROCESS_INSTANCE, (), _);

#define DEFINE_K_WORK_SUBMIT(inst, _) \
	k_work_submit(&GET_PROCESS_NAME(inst, work))

static void foo_irq_handler(void)
{
	LISTIFY(CONFIG_SAMPLE_INST_COUNT, DEFINE_K_WORK_SUBMIT, (;), _);
}

int main(void)
{
	/*
	 * The work queue is configured with lower prio than the "ISR thread"
	 * (main thread). The work can be resubmitted, and will be run once
	 * when the work queue thread gets to run.
	 */
	foo_irq_handler();
	foo_irq_handler();
	foo_irq_handler();
}
