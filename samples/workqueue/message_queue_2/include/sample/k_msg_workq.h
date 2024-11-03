/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SAMPLE_K_MSG_WORKQ_H_
#define SAMPLE_K_MSG_WORKQ_H_

#include <zephyr/kernel.h>

struct k_msg_workq *msg_workq;

typedef void (*k_msg_workq_handler)(struct k_msg_workq *msg_workq);

struct k_msg_workq {
	struct k_msgq msgq;
	struct k_work work;
	k_msg_workq_handler handler;
};

static inline void k_msg_workq_internal_handler(struct k_work *work)
{
	struct k_msg_workq *msg_workq;

	msg_workq = CONTAINER_OF(work, struct k_msg_workq, work);
	msg_workq->handler(msg_workq);
}

static inline void k_msg_workq_init(struct k_msg_workq *msg_workq,
				    char *buffer,
				    size_t msg_size,
				    uint32_t max_msgs,
				    k_msg_workq_handler handler)
{
	k_msgq_init(&msg_workq->msgq, buffer, msg_size, max_msgs);
	k_work_init(&msg_workq->work, k_msg_workq_internal_handler);
	msg_workq->handler = handler;
}

static inline int k_msg_workq_put(struct k_msg_workq *msg_workq,
				  const void *data,
				  k_timeout_t timeout)
{
	int ret;

	ret = k_msgq_put(&msg_workq->msgq, data, timeout);
	if (ret) {
		return ret;
	}

	k_work_submit(&msg_workq->work);
	return ret;
}

static inline int k_msg_workq_get(struct k_msg_workq *msg_workq,
				  void *data)
{
	int ret;

	ret = k_msgq_get(&msg_workq->msgq, data, K_NO_WAIT);
	if (ret) {
		return ret;
	}

	if (k_msgq_num_used_get(&msg_workq->msgq)) {
		k_work_submit(&msg_workq->work);
	}

	return ret;
}

#endif /* SAMPLE_K_MSG_WORKQ_H_ */
