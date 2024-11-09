/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

int main(void)
{
	printk("The Incredibles reference\n");
	return 0;
}

static void main_wrapper(void *arg0, void *arg1, void *arg2)
{
	(void)main();
}

K_KERNEL_THREAD_DEFINE(main_thread0, CONFIG_MAIN_STACK_SIZE, main_wrapper, NULL, NULL, NULL,
		       CONFIG_MAIN_THREAD_PRIORITY, 0, 0);

K_KERNEL_THREAD_DEFINE(main_thread1, CONFIG_MAIN_STACK_SIZE, main_wrapper, NULL, NULL, NULL,
		       CONFIG_MAIN_THREAD_PRIORITY, 0, 0);

K_KERNEL_THREAD_DEFINE(main_thread2, CONFIG_MAIN_STACK_SIZE, main_wrapper, NULL, NULL, NULL,
		       CONFIG_MAIN_THREAD_PRIORITY, 0, 0);
