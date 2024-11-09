/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#define SAMPLE_SLEEP_MS 500

#define GET_PROCESS_NAME(inst, name) \
	_CONCAT_3(process_, name, inst)

#define DEFINE_PROCESS_THREAD_ROUTINE(inst)						\
	static void GET_PROCESS_NAME(inst, thread_routine)(void *arg0,			\
							   void *arg1,			\
							   void *arg2)			\
	{										\
		bool on = true;								\
											\
		while (1) {								\
			printk("LED%u: %s\n", inst, on ? "ON" : "OFF");			\
			on = !on;							\
			k_msleep(SAMPLE_SLEEP_MS);					\
		}									\
											\
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
	DEFINE_PROCESS_THREAD_ROUTINE(inst)						\
	DEFINE_PROCESS_THREAD(inst)

LISTIFY(CONFIG_SAMPLE_LED_COUNT, DEFINE_PROCESS_INSTANCE, (), _);

int main(void)
{
	/*
	 * We have nothing to use the main thread for
	 * Would be nice if we could reuse it maybe? (foreshadowing)
	 */
	return 0;
}
