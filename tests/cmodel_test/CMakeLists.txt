# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.14)

set(RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." CACHE PATH "Project top directory")

include(${RUNTIME_DIR}/cmake/fetch_cann_cmake.cmake)
project(cmodel_test)

init_cann_project()
add_cann_target_options()

include(${RUNTIME_DIR}/cmake/func.cmake)
set_runtime_params("${RUNTIME_DIR}")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type: Release/Debug" FORCE)
endif()

if(NOT DEFINED PRODUCT_SIDE)
    set(PRODUCT_SIDE host)
endif()

include(${RUNTIME_DIR}/cmake/third_party/acl_compat.cmake)
add_cann_third_party(json)
add_cann_third_party(csec)
add_cann_third_party(protobuf)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/local_cmodel_build
                 ${CMAKE_BINARY_DIR}/tests/cmodel_test/local_cmodel_build)
add_subdirectory(${RUNTIME_DIR}/src ${CMAKE_BINARY_DIR}/src)

add_dependencies(mmpa c_sec)
add_dependencies(static_mmpa c_sec)
add_dependencies(runtime_platform_others c_sec)
add_dependencies(runtime_platform_tiny c_sec)
add_dependencies(runtime_platform_910B c_sec)
add_dependencies(runtime_platform_kirin c_sec)
