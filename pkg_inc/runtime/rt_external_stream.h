/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCE_RUNTIME_RT_EXTERNAL_STREAM_H
#define CCE_RUNTIME_RT_EXTERNAL_STREAM_H

#include <stdlib.h>

#include "rt_external_base.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @ingroup stream_type
 * @brief stream op bit flags
 */
#define RT_STREAM_DEFAULT (0x00U)
#define RT_STREAM_PERSISTENT (0x01U)
#define RT_STREAM_FORCE_COPY (0x02U)
#define RT_STREAM_HUGE (0x04U)
#define RT_STREAM_AICPU (0x08U)
#define RT_STREAM_CPU_SCHEDULE (0x08U)
#define RT_STREAM_FORBIDDEN_DEFAULT (0x10U)
#define RT_STREAM_HEAD (0x20U)
#define RT_STREAM_PRIMARY_DEFAULT (0x40U)
#define RT_STREAM_PRIMARY_FIRST_DEFAULT (0x80U)
#define RT_STREAM_OVERFLOW (0x100U)
#define RT_STREAM_VECTOR_CORE_USE (0x1000U)
#define RT_STREAM_ACSQ_LOCK (0x2000U)
#define RT_STREAM_DQS_CTRL (0x4000U)
#define RT_STREAM_DQS_INTER_CHIP (0x8000U)

/**
 * @ingroup stream_type
 * @brief priority level default value when create a stream
 */
#define RT_STREAM_PRIORITY_DEFAULT (0U)

/**
 * @ingroup dvrt_stream
 * @brief stream type
 */
#define RT_NORMAL_STREAM    (0x00U)
#define RT_HUGE_STREAM      (0x01U)

/**
 * @ingroup dvrt_stream
 * @brief create stream instance
 * @param [in|out] stm   created stream
 * @param [in] priority   stream priority
 * @param [in] flags  stream op flags
 * @return RT_ERROR_NONE for ok
 * @return RT_ERROR_INVALID_VALUE for error input
 */
RTS_API rtError_t rtStreamCreateWithFlags(rtStream_t *stm, int32_t priority, uint32_t flags);

/**
 * @ingroup dvrt_stream
 * @brief create stream instance
 * @param [in] stm   stream hadle
 * @param [out] sqId   stream op sqId
 * @return RT_ERROR_NONE for ok
 * @return RT_ERROR_INVALID_VALUE for error input
 */
RTS_API rtError_t rtStreamGetSqid(const rtStream_t stm, uint32_t *sqId);

/**
 * @ingroup dvrt_stream
 * @brief get stream cq info
 * @param [in] stm   stream hadle
 * @param [out] sqId   stream op cqId
 * @param [out] cqId   stream op logic cqId
 * @return RT_ERROR_NONE for ok
 * @return RT_ERROR_INVALID_VALUE for error input
 */
RTS_API rtError_t rtStreamGetCqid(const rtStream_t stm, uint32_t *cqId, uint32_t *logicCqId);

/*
 * @ingroup dvrt_stream
 * @brief enable debug for dump overflow exception with stream
 * @param [in] addr: ddr address of kernel exception dumpped
 * @param [in] stm: stream handle
 * @param [in] flag: debug flag
 * @return RT_ERROR_NONE for ok
 * @return RT_ERROR_INVALID_VALUE for error input
 */
RTS_API rtError_t rtDebugRegisterForStream(rtStream_t stm, uint32_t flag, const void *addr,
                                           uint32_t *streamId, uint32_t *taskId);

/**
 * @ingroup dvrt_stream
 * @brief inquire max stream count and max task count per stream
 * @param [in] streamType   Stream Type
 * @param [in] MaxStrCount   Max stream count
 * @param [in] MaxTaskCount   max task count per stream
 * @return RT_ERROR_NONE for complete
 * @return RT_ERROR_INVALID_VALUE for error input
 */
RTS_API rtError_t rtGetMaxStreamAndTask(uint32_t streamType, uint32_t *maxStrCount, uint32_t *maxTaskCount);

/**
 * @ingroup dvrt_stream
 * @brief create stream instance
 * @param [in] stm   stream hadle
 * @param [out] workaddr   workaddr on stream
 * @param [out] worksize   worksize on stream
 * @return RT_ERROR_NONE for ok
 * @return RT_ERROR_INVALID_VALUE for error input
 */
RTS_API rtError_t rtStreamGetWorkspace(const rtStream_t stm, void **workaddr, size_t *worksize);

#if defined(__cplusplus)
}
#endif

#endif  // CCE_RUNTIME_RT_EXTERNAL_STREAM_H

