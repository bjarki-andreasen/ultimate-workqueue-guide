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

/*
 * This handler will be called by the system workqueue thread,
 */
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
	/*
	 * Initialize the asyncio context. This is done only once. The function is not threadsafe
	 * and is typically called by init.c on startup.
	 */
	sys_asyncio_init(&foo_asyncio_ctx);

	/*
	 * Initialize all work items which belong to the asyncio context. This function is not
	 * threadsafe and is typically called by init.c on startup, after sys_asyncio_init().
	 *
	 * We can add as many items as we wish to the context, even targetting the same handler.
	 * We can compare this to putting multiple messages of the same type to a message queue.
	 */
	sys_asyncio_work_init(&foo_asyncio_ctx, &foo_printable_0.work, foo_printable_work_handler);
	sys_asyncio_work_init(&foo_asyncio_ctx, &foo_printable_1.work, foo_printable_work_handler);

	/* When the context and the work items belonging to said context have been initialized, we
	 * can start the asyncio context. No work is executed when we start the context. We are
	 * simply dictating that work may now be scheduled.
	 */
	sys_asyncio_start(&foo_asyncio_ctx);

	/*
	 * Before modifying the shared member (.message) of the asyncio work item, we must reserve
	 * it. If this function returns 0, we have successfully reserved the work. Once reserved,
	 * we are certain that the work is not scheduled, meaning the system workqueue thread will
	 * not be accessing the data. We are also ensured that no other thread can access the data
	 * as the item remains reserved until the item has been scheduled and the work has been
	 * executed.
	 */
	if (sys_asyncio_work_reserve(&foo_printable_0.work, K_NO_WAIT) < 0) {
		printk("reserve failed\n");
		return 0;
	}

	/* Safely modify the message */
	strcpy(foo_printable_0.message, "hello there 0");

	/*
	 * Once we have modified the work item's data, we can schedule it. After we call schedule,
	 * we are no longer allowed to modify or in any way access the data of the work item. The
	 * ownership of the data of the work item is passed to the system workqueue thread. After
	 * the work item has been executed, we can try to reserve the work item again, at which
	 * point we get ownership of the data again.
	 */
	if (sys_asyncio_work_schedule(&foo_printable_0.work, K_NO_WAIT) < 0) {
		printk("schedule failed\n");
		return 0;
	}

	/*
	 * We can schedule multiple work items concurrently, since we are passing a different
	 * context to the foo_printable_work_handler() every time.
	 */
	if (sys_asyncio_work_reserve(&foo_printable_1.work, K_NO_WAIT) < 0) {
		printk("reserve failed\n");
		return 0;
	}

	strcpy(foo_printable_1.message, "hello there 1");

	if (sys_asyncio_work_schedule(&foo_printable_1.work, K_NO_WAIT) < 0) {
		printk("schedule failed\n");
		return 0;
	}

	k_msleep(1000);

	/*
	 * We can stop the asyncio context at any time safely. Once stopped, we are ensured
	 * no more work will be scheduled, and all work which was scheduled stopped.
	 */
	sys_asyncio_stop(&foo_asyncio_ctx);

	/*
	 * We can now safely reset variables which where used by foo_printable_work_handler().
	 */
	memset(foo_print_buf, 0, sizeof(foo_print_buf));
	foo_print_count = 0;

	k_msleep(1000);

	/*
	 * At this point, the context can be restarted, and work can again be scheduled.
	 */
	sys_asyncio_start(&foo_asyncio_ctx);

	if (sys_asyncio_work_reserve(&foo_printable_0.work, K_NO_WAIT) < 0) {
		printk("reserve failed\n");
		return 0;
	}

	strcpy(foo_printable_0.message, "hello again 0");

	if (sys_asyncio_work_schedule(&foo_printable_0.work, K_NO_WAIT) < 0) {
		printk("schedule failed\n");
		return 0;
	}

	return 0;
}
