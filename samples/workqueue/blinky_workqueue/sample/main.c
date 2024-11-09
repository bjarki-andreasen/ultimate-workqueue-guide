/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#define SAMPLE_SLEEP_MS 500

#define SAMPLE_WORK_DEFINE(inst, _)								\
	static struct k_work_delayable _CONCAT(sample_dwork, inst);				\
	static bool _CONCAT(sample_on, inst) = true;						\
												\
	static void _CONCAT(sample_work_handler, inst)(struct k_work *work)			\
	{											\
		printk("LED%u: %s\n", inst, _CONCAT(sample_on, inst) ? "ON" : "OFF");		\
		_CONCAT(sample_on, inst) = !_CONCAT(sample_on, inst);				\
		k_work_schedule(&_CONCAT(sample_dwork, inst), K_MSEC(SAMPLE_SLEEP_MS));		\
	}											\
												\
	static int _CONCAT(sample_work_init, inst)(void)					\
	{											\
		k_work_init_delayable(&_CONCAT(sample_dwork, inst),				\
				      _CONCAT(sample_work_handler, inst));			\
		k_work_schedule(&_CONCAT(sample_dwork, inst), K_MSEC(SAMPLE_SLEEP_MS));		\
		return 0;									\
	}											\
												\
	SYS_INIT(_CONCAT(sample_work_init, inst), APPLICATION, 99);

LISTIFY(CONFIG_SAMPLE_LED_COUNT, SAMPLE_WORK_DEFINE, (), _);

int main(void)
{
	/*
	 * We have nothing to use the main thread for
	 * Would be nice if we could reuse it maybe? (foreshadowing)
	 */
	return 0;
}
