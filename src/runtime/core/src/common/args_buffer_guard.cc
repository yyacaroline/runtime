/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "args_buffer_guard.hpp"
#include "error_message_manage.hpp"

namespace cce {
namespace runtime {

ArgsBufferGuard::~ArgsBufferGuard()
{
    if (buffer_ != nullptr) {
        free(buffer_);
        RT_LOG(RT_LOG_DEBUG, "free args buffer, size=%llu.", size_);
        buffer_ = nullptr;
        size_ = 0ULL;
    }
}

void* ArgsBufferGuard::EnsureCapacity(uint64_t requiredSize)
{
    if (buffer_ != nullptr && size_ >= requiredSize) {
        return buffer_;
    }

    if (buffer_ != nullptr) {
        free(buffer_);
        buffer_ = nullptr;
        size_ = 0ULL;
    }

    uint64_t allocSize = (requiredSize > ARGS_BUFFER_DEFAULT_SIZE) ? requiredSize : ARGS_BUFFER_DEFAULT_SIZE;
    buffer_ = malloc(allocSize);
    COND_RETURN_AND_MSG_OUTER(buffer_ == nullptr, nullptr, ErrorCode::EE1013, allocSize);

    size_ = allocSize;
    RT_LOG(RT_LOG_DEBUG, "alloc args buffer success, size=%llu.", allocSize);
    return buffer_;
}

}  // namespace runtime
}  // namespace cce
