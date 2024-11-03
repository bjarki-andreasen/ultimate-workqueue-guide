/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

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

K_MSGQ_DEFINE(foo_msgq, FOO_MSG_SIZE, FOO_MSGQ_MAX_MSGS, 8);
static struct k_work foo_work;
K_MSGQ_DEFINE(bar_msgq, BAR_MSG_SIZE, BAR_MSGQ_MAX_MSGS, 8);
static struct k_work bar_work;

/*
 * This helper both ensures the correct message type passed to the correct
 * message queue, and submits the work which will get and handle the message.
 */
static inline int foo_put_msg(const struct foo_msg *msg, k_timeout_t timeout)
{
	int ret;

	ret = k_msgq_put(&foo_msgq, msg, timeout);
	if (ret) {
		return ret;
	}

	k_work_submit(&foo_work);
	return ret;
}

/*
 * We get the message, then check if there are more messages pending. If there
 * are messages pending, we resubmit the work to handle the next message later.
 *
 * We could technically flush the entire message queue a while loop, but this
 * risks starving the work queue if we are submitting messages very near to or
 * faster than the work queue can handle each message.
 *
 * To be nice, we handle only one message at a time, to let other work items be
 * handled in the order they where submitted.
 */
static inline int foo_get_msg(struct foo_msg *msg)
{
	int ret;

	ret = k_msgq_get(&foo_msgq, msg, K_NO_WAIT);
	if (ret) {
		return ret;
	}

	if (k_msgq_num_used_get(&foo_msgq)) {
		k_work_submit(&foo_work);
	}

	return ret;
}

static inline int bar_put_msg(const struct bar_msg *msg, k_timeout_t timeout)
{
	int ret;

	ret = k_msgq_put(&bar_msgq, msg, timeout);
	if (ret) {
		return ret;
	}

	k_work_submit(&bar_work);
	return ret;
}

static inline int bar_get_msg(struct bar_msg *msg)
{
	int ret;

	ret = k_msgq_get(&bar_msgq, msg, K_NO_WAIT);
	if (ret) {
		return ret;
	}

	if (k_msgq_num_used_get(&bar_msgq)) {
		k_work_submit(&bar_work);
	}

	return ret;
}

static void foo_work_handler(struct k_work *work)
{
	int ret;
	struct foo_msg msg;

	ret = foo_get_msg(&msg);
	if (ret) {
		return;
	}

	printk("foo: %u\n", msg.value);
}

static void bar_work_handler(struct k_work *work)
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

	k_work_init(&foo_work, foo_work_handler);
	k_work_init(&bar_work, bar_work_handler);

	/*
	 * The main thread has higher prio than system workqueue in this sample.
	 *
	 * We put all our messages in one go, and when we return, the work queue
	 * starts processing the work.
	 *
	 * We will get the following output:
	 *
	 * foo: 0
	 * bar: -1
	 * foo: 1
	 * bar: -2
	 * foo: 2
	 * bar: -3
	 * foo: 3
	 * bar: -4
	 *
	 * Since every user of the thread is being nice, only doing one piece of
	 * work at a time, we handle all messages a "fair" way.
	 *
	 * There are complex work queues like p4wq which implements priority of work
	 * items which could also be used to tailor the scheduling.
	 */

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
