#
# Copyright (c) 2016 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)
set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf_connect_sdk_fundamentals)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
zephyr_library_sources(Kconfig)


