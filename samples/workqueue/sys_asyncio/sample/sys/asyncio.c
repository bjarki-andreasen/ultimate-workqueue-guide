/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sample/sys/asyncio.h>

#define SAMPLE_COMPONENT_HEARTBEAT_INTERVAL K_SECONDS(5)

void sys_asyncio_init(struct sys_asyncio_ctx *ctx)
{
	sys_slist_init(&ctx->work_list);
	ctx->running = false;
}

static void sys_asyncio_work_handler(struct k_work *work)
{
	struct k_work_delayable *dwork = k_work_delayable_from_work(work);
	struct sys_asyncio_work *asyncio_work =
		CONTAINER_OF(dwork, struct sys_asyncio_work, dwork);

	asyncio_work->handler(asyncio_work);
	k_sem_give(&asyncio_work->idle_sem);
}

void sys_asyncio_work_init(struct sys_asyncio_ctx *ctx,
			   struct sys_asyncio_work *work,
			   sys_asyncio_work_handler_t handler)
{
	work->ctx = ctx;
	sys_slist_append(&ctx->work_list, &work->work_node);
	k_work_init_delayable(&work->dwork, sys_asyncio_work_handler);
	work->handler = handler;
	k_sem_init(&work->idle_sem, 1, 1);
}

int sys_asyncio_work_reserve(struct sys_asyncio_work *work, k_timeout_t timeout)
{
	return k_sem_take(&work->idle_sem, timeout);
}

int sys_asyncio_work_schedule(struct sys_asyncio_work *work, k_timeout_t timeout)
{
	struct sys_asyncio_ctx *ctx = work->ctx;
	int ret;

	K_SPINLOCK(&ctx->spinlock) {
		if (!ctx->running) {
			k_sem_give(&work->idle_sem);
			ret = -EPERM;
			K_SPINLOCK_BREAK;
		}

		k_work_schedule(&work->dwork, timeout);
		ret = 0;
	}

	return ret;
}

void sys_asyncio_start(struct sys_asyncio_ctx *ctx)
{
	K_SPINLOCK(&ctx->spinlock) {
		ctx->running = true;
	}
}

void sys_asyncio_stop(struct sys_asyncio_ctx *ctx)
{
	sys_snode_t *work_node;
	struct sys_asyncio_work *work;
	struct k_work_sync sync;

	K_SPINLOCK(&ctx->spinlock) {
		ctx->running = false;

		SYS_SLIST_FOR_EACH_NODE(&ctx->work_list, work_node) {
			work = (struct sys_asyncio_work *)work_node;
			k_work_cancel_delayable_sync(&work->dwork, &sync);
			k_sem_give(&work->idle_sem);
		}
	}
}
