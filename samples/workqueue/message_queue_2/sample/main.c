/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <sample/k_msg_workq.h>

#define FOO_MSGQ_MAX_MSGS 6
#define FOO_MSG_SIZE sizeof(struct foo_msg)
#define FOO_MSGQ_BUF_SIZE (FOO_MSG_SIZE * FOO_MSGQ_MAX_MSGS)

#define BAR_MSGQ_MAX_MSGS 6
#define BAR_MSG_SIZE sizeof(struct bar_msg)
#define BAR_MSGQ_BUF_SIZE (BAR_MSG_SIZE * BAR_MSGQ_MAX_MSGS)

struct foo_msg {
	uint8_t value;
};

struct bar_msg {
	int value;
};

static struct k_msg_workq foo_msg_workq;
static char foo_msg_workq_buf[FOO_MSGQ_BUF_SIZE];
static struct k_msg_workq bar_msg_workq;
static char bar_msg_workq_buf[BAR_MSGQ_BUF_SIZE];

static inline int foo_put_msg(const struct foo_msg *msg, k_timeout_t timeout)
{
	return k_msg_workq_put(&foo_msg_workq, msg, timeout);
}

static inline int foo_get_msg(struct foo_msg *msg)
{
	return k_msg_workq_get(&foo_msg_workq, msg);
}

static inline int bar_put_msg(const struct bar_msg *msg, k_timeout_t timeout)
{
	return k_msg_workq_put(&bar_msg_workq, msg, timeout);
}

static inline int bar_get_msg(struct bar_msg *msg)
{
	return k_msg_workq_get(&bar_msg_workq, msg);
}

static void foo_msg_workq_handler(struct k_msg_workq *msg_workq)
{
	int ret;
	struct foo_msg msg;

	ret = foo_get_msg(&msg);
	if (ret) {
		return;
	}

	printk("foo: %u\n", msg.value);
}

static void bar_msg_workq_handler(struct k_msg_workq *msg_workq)
{
	int ret;
	struct bar_msg msg;

	ret = bar_get_msg(&msg);
	if (ret) {
		return;
	}

	printk("bar: %i\n", msg.value);
}

int main(void)
{
	struct foo_msg foo_msg;
	struct bar_msg bar_msg;

	k_msg_workq_init(&foo_msg_workq, foo_msg_workq_buf, FOO_MSG_SIZE, FOO_MSGQ_MAX_MSGS,
			 foo_msg_workq_handler);
	k_msg_workq_init(&bar_msg_workq, bar_msg_workq_buf, BAR_MSG_SIZE, BAR_MSGQ_MAX_MSGS,
			 bar_msg_workq_handler);

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
