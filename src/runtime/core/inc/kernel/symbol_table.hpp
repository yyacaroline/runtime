/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef __CCE_RUNTIME_SYMBOL_TABLE_HPP__
#define __CCE_RUNTIME_SYMBOL_TABLE_HPP__

#include <map>
#include "base.hpp"
#include "osal.hpp"

namespace cce {
namespace runtime {

struct SymbolEntry {
    void *binHandle;
    const void *hostVar;
    const char *deviceVarName;
    uint32_t deviceSymbolOffset;
    uint32_t deviceSymbolSize;
    uint32_t flags;
};

class SymbolTable : public NoCopy {
public:
    SymbolTable() = default;

    rtError_t Register(void *binHandle, const void *hostVar, const char *deviceVarName,
                       size_t size, uint32_t flags);
    rtError_t Lookup(const void *hostVar, SymbolEntry &entry);
    rtError_t GetDeviceAddress(const void *hostVar, uint32_t deviceId, void **devPtr);
    rtError_t GetSize(const void *hostVar, size_t *size);

private:
    std::map<const void *, SymbolEntry> symbolMap_;
    SpinLock symbolMapLock_;
};

} // namespace runtime
} // namespace cce

#endif // __CCE_RUNTIME_SYMBOL_TABLE_HPP__