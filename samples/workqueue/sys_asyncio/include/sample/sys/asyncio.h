/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * System asyncio is an asynchronous framework based on the system workqueue.
 *
 * The framework consists of work items bundled together into independent
 * contexts.
 *
 * The framework provides:
 *   - Threadsafe sharing of data between the work items and the context
 *     they belong to.
 *   - Threadsafe submission of work to each individual context.
 *   - Threadsafe starting and stopping of every individual context.
 */

#ifndef SAMPLE_SYS_ASYNCIO_H_
#define SAMPLE_SYS_ASYNCIO_H_

#include <zephyr/kernel.h>

struct sys_asyncio_ctx {
	sys_slist_t work_list;
	struct k_spinlock spinlock;
	bool running;
};

struct sys_asyncio_work;

typedef void (*sys_asyncio_work_handler_t)(struct sys_asyncio_work *work);

struct sys_asyncio_work {
	sys_snode_t work_node;
	struct sys_asyncio_ctx *ctx;
	sys_asyncio_work_handler_t handler;
	struct k_work_delayable dwork;
	struct k_sem idle_sem;
};

/** Initialize asyncio context work */
void sys_asyncio_init(struct sys_asyncio_ctx *ctx);

/** Initialize work assigning it to asyncio context */
void sys_asyncio_work_init(struct sys_asyncio_ctx *ctx,
			   struct sys_asyncio_work *work,
			   sys_asyncio_work_handler_t handler);

/** Reserve work */
int sys_asyncio_work_reserve(struct sys_asyncio_work *work, k_timeout_t timeout);

/** Schedule work to be executed */
int sys_asyncio_work_schedule(struct sys_asyncio_work *work, k_timeout_t timeout);

/** Allow work to be scheduled */
void sys_asyncio_run(struct sys_asyncio_ctx *ctx);

/** Cancel all work and prevent new work from being scheduled */
void sys_asyncio_stop(struct sys_asyncio_ctx *ctx);

#endif /* SAMPLE_SYS_ASYNCIO_H_ */
