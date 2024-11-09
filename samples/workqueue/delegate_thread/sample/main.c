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
	static void GET_PROCESS_NAME(inst, work_handler)(void)				\
	{										\
		printk("hello from process work handler " STRINGIFY(inst) "\n");	\
	}

#define DEFINE_PROCESS_WORK_SEM(inst) \
	static K_SEM_DEFINE(GET_PROCESS_NAME(inst, sem), 0, 1);

#define DEFINE_PROCESS_THREAD_ROUTINE(inst)						\
	static void GET_PROCESS_NAME(inst, thread_routine)(void *arg0,			\
							   void *arg1,			\
							   void *arg2)			\
	{										\
		while (1) {								\
			(void)k_sem_take(&GET_PROCESS_NAME(inst, sem), K_FOREVER);	\
			GET_PROCESS_NAME(inst, work_handler)();				\
		}									\
	}

#define DEFINE_PROCESS_THREAD(inst)							\
	K_THREAD_DEFINE(								\
		GET_PROCESS_NAME(inst, thread),						\
		1024,									\
		GET_PROCESS_NAME(inst, thread_routine),					\
		NULL,									\
		NULL,									\
		NULL,									\
		(CONFIG_MAIN_THREAD_PRIORITY + 1),					\
		0,									\
		0									\
	);

#define DEFINE_PROCESS_INSTANCE(inst, _)						\
	DEFINE_PROCESS_WORK_HANDLER(inst)						\
	DEFINE_PROCESS_WORK_SEM(inst)							\
	DEFINE_PROCESS_THREAD_ROUTINE(inst)						\
	DEFINE_PROCESS_THREAD(inst)

LISTIFY(CONFIG_SAMPLE_INST_COUNT, DEFINE_PROCESS_INSTANCE, (), _);

#define DEFINE_PROCESS_K_SEM_GIVE(inst, _) \
	k_sem_give(&GET_PROCESS_NAME(inst, sem))

static void foo_irq_handler(void)
{
	LISTIFY(CONFIG_SAMPLE_INST_COUNT, DEFINE_PROCESS_K_SEM_GIVE, (;), _);
}

int main(void)
{
	/*
	 * The process threads are configured with lower prio than the "ISR thread"
	 * (main thread).
	 */
	foo_irq_handler();
	foo_irq_handler();
	foo_irq_handler();
}
