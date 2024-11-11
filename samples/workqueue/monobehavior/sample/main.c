/*
 * Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#define SAMPLE_TICKS_PER_SEC (50)
#define SAMPLE_TICKTIME_MS   (1000 / SAMPLE_TICKS_PER_SEC)

typedef void (*sample_start_fn)(void *data);
typedef void (*sample_update_fn)(void *data);

struct sample_entity {
	sample_start_fn start;
	sample_start_fn update;
	void *data;
};

struct blinky_data {
	size_t ticks;
	size_t id;
};

static void blinky_start(void *data)
{
	struct blinky_data *bdata = data;

	bdata->ticks = 0;
}

static void blinky_update(void *data)
{
	struct blinky_data *bdata = data;

	if (bdata->ticks == 0) {
		printk("LED%u: %s\n", bdata->id, "ON");
	} else if (bdata->ticks == (SAMPLE_TICKS_PER_SEC / 2)) {
		printk("LED%u: %s\n", bdata->id, "OFF");
	}

	bdata->ticks++;

	if (bdata->ticks == SAMPLE_TICKS_PER_SEC) {
		bdata->ticks = 0;
	}
}

struct counter_data {
	size_t ticks;
	size_t counter;
};

static void counter_start(void *data)
{
	struct counter_data *cdata = data;

	cdata->ticks = 0;
	cdata->counter = 0;
}

static void counter_update(void *data)
{
	struct counter_data *cdata = data;

	if (cdata->ticks == SAMPLE_TICKS_PER_SEC) {
		cdata->counter++;
		printk("Seconds: %u\n", cdata->counter);
		cdata->ticks = 0;
	}

	cdata->ticks++;
}

static struct blinky_data sample_bdata[2] = {
	{
		.id = 0,
	},
	{
		.id = 1,
	},
};

static struct counter_data sample_cdata;

static const struct sample_entity sample_entities[] = {
	{
		.start = blinky_start,
		.update = blinky_update,
		.data = &sample_bdata[0],
	},
	{
		.start = blinky_start,
		.update = blinky_update,
		.data = &sample_bdata[1],
	},
	{
		.start = counter_start,
		.update = counter_update,
		.data = &sample_cdata,
	},
};

int main(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(sample_entities); i++) {
		sample_entities[i].start(sample_entities[i].data);
	}

	while (1) {
		for (size_t i = 0; i < ARRAY_SIZE(sample_entities); i++) {
			sample_entities[i].update(sample_entities[i].data);
		}

		k_msleep(SAMPLE_TICKTIME_MS);
	}

	return 0;
}
