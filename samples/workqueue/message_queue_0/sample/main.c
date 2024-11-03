/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#define FOO_THRAD_STACK_SIZE 512
#define FOO_THRAD_PRIO       2
#define FOO_MSGQ_MAX_MSGS    6
#define FOO_MSG_SIZE         sizeof(struct foo_msg)
#define FOO_MSGQ_BUF_SIZE    (FOO_MSG_SIZE * FOO_MSGQ_MAX_MSGS)

#define BAR_THRAD_STACK_SIZE 512
#define BAR_THRAD_PRIO       2
#define BAR_MSGQ_MAX_MSGS    6
#define BAR_MSG_SIZE         sizeof(struct bar_msg)
#define BAR_MSGQ_BUF_SIZE    (BAR_MSG_SIZE * BAR_MSGQ_MAX_MSGS)

struct foo_msg {
	uint8_t value;
};

struct bar_msg {
	int value;
};

K_MSGQ_DEFINE(foo_msgq, FOO_MSG_SIZE, FOO_MSGQ_MAX_MSGS, 8);
K_MSGQ_DEFINE(bar_msgq, BAR_MSG_SIZE, BAR_MSGQ_MAX_MSGS, 8);

static inline int foo_put_msg(const struct foo_msg *msg, k_timeout_t timeout)
{
	return k_msgq_put(&foo_msgq, msg, timeout);
}

static inline int foo_get_msg(struct foo_msg *msg, k_timeout_t timeout)
{
	return k_msgq_get(&foo_msgq, msg, timeout);
}

static inline int bar_put_msg(const struct bar_msg *msg, k_timeout_t timeout)
{
	return k_msgq_put(&bar_msgq, msg, timeout);
}

static inline int bar_get_msg(struct bar_msg *msg, k_timeout_t timeout)
{
	return k_msgq_get(&bar_msgq, msg, timeout);
}

static void foo_routine(void *arg0, void *arg1, void *arg2)
{
	struct foo_msg msg;

	while (1) {
		if (foo_get_msg(&msg, K_FOREVER)) {
			continue;
		}

		printk("foo: %u\n", msg.value);

		/* we can be nice and yield after each message, we don't have to */
		k_yield();
	}
}

static void bar_routine(void *arg0, void *arg1, void *arg2)
{
	struct bar_msg msg;

	while (1) {
		if (bar_get_msg(&msg, K_FOREVER)) {
			continue;
		}

		printk("bar: %i\n", msg.value);

		/* we can be nice and yield after each message, we don't have to */
		k_yield();
	}
}

K_KERNEL_THREAD_DEFINE(foo_thread, FOO_THRAD_STACK_SIZE, foo_routine, NULL, NULL, NULL,
		       FOO_THRAD_PRIO, 0, 0);

K_KERNEL_THREAD_DEFINE(bar_thread, BAR_THRAD_STACK_SIZE, bar_routine, NULL, NULL, NULL,
		       BAR_THRAD_PRIO, 0, 0);

int main(void)
{
	struct foo_msg foo_msg;
	struct bar_msg bar_msg;

	foo_msg.value = 0;
	foo_put_msg(&foo_msg, K_NO_WAIT);
	foo_msg.value = 1;
	foo_put_msg(&foo_msg, K_NO_WAIT);
	foo_msg.value = 2;
	foo_put_msg(&foo_msg, K_NO_WAIT);
	foo_msg.value = 3;
	foo_put_msg(&foo_msg, K_NO_WAIT);

	bar_msg.value = -1;
	bar_put_msg(&bar_msg, K_NO_WAIT);
	bar_msg.value = -2;
	bar_put_msg(&bar_msg, K_NO_WAIT);
	bar_msg.value = -3;
	bar_put_msg(&bar_msg, K_NO_WAIT);
	bar_msg.value = -4;
	bar_put_msg(&bar_msg, K_NO_WAIT);

	return 0;
}
