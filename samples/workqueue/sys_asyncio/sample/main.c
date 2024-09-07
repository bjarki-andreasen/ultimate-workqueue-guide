/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <string.h>
#include <sample/sys/asyncio.h>

static struct sys_asyncio_ctx foo_asyncio_ctx;
static char foo_print_buf[16];
static size_t foo_print_count;

struct foo_printable {
	struct sys_asyncio_work work;
	char message[16];
};

static void foo_printable_work_handler(struct sys_asyncio_work *work)
{
	struct foo_printable *printable = CONTAINER_OF(work, struct foo_printable, work);

	strcpy(foo_print_buf, printable->message);
	foo_print_count++;
	printk("foo message #%u: %s\n", foo_print_count, foo_print_buf);
}

static struct foo_printable foo_printable_0;
static struct foo_printable foo_printable_1;

int main(void)
{
	sys_asyncio_init(&foo_asyncio_ctx);
	sys_asyncio_work_init(&foo_asyncio_ctx, &foo_printable_0.work, foo_printable_work_handler);
	sys_asyncio_work_init(&foo_asyncio_ctx, &foo_printable_1.work, foo_printable_work_handler);

	if (sys_asyncio_work_reserve(&foo_printable_0.work, K_NO_WAIT) < 0) {
		printk("reserve failed\n");
		return 0;
	}

	strcpy(foo_printable_0.message, "hello there 0");

	if (sys_asyncio_work_reserve(&foo_printable_1.work, K_NO_WAIT) < 0) {
		printk("reserve failed\n");
		return 0;
	}

	strcpy(foo_printable_1.message, "hello there 1");

	sys_asyncio_start(&foo_asyncio_ctx);

	if (sys_asyncio_work_schedule(&foo_printable_0.work, K_NO_WAIT) < 0) {
		printk("schedule failed\n");
		return 0;
	}

	if (sys_asyncio_work_schedule(&foo_printable_1.work, K_NO_WAIT) < 0) {
		printk("schedule failed\n");
		return 0;
	}

	k_msleep(1000);

	sys_asyncio_stop(&foo_asyncio_ctx);
	return 0;
}
