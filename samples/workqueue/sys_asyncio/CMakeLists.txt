# Copyright (c) Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sample)

target_sources(app PRIVATE sample/main.c sample/sys/asyncio.c)
target_include_directories(app PUBLIC include)
