/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_UT_AICPU_SCHED_AICPU_SCHEDULE_UT_TESTCASE_AICPUSD_MSQ_OPERATOR_STUB_H_
#define TESTS_UT_AICPU_SCHED_AICPU_SCHEDULE_UT_TESTCASE_AICPUSD_MSQ_OPERATOR_STUB_H_

#include <cstdint>
#include <cstring>
#include "aicpusd_message_queue.h"

namespace AicpuScheduleUtStub {
constexpr uintptr_t kMsqOperatorHandle = 0x20260601;

inline void MsqOperatorResetStub() {}

inline AicpuSchedule::MsqStatus MsqOperatorReadStatusStub()
{
    AicpuSchedule::MsqStatus status = {};
    status.valid = 0U;
    status.size = 0U;
    status.comp = 0U;
    return status;
}

inline void MsqOperatorReadDataStub(uint32_t, AicpuSchedule::MsqDatas *datas)
{
    if (datas == nullptr) {
        return;
    }
    *datas = {};
}

inline void MsqOperatorSendRspStub() {}

inline void MsqOperatorWaitStub() {}

inline void *DlopenMsqOperatorStub(const char *filename, int)
{
    if ((filename != nullptr) && (std::strcmp(filename, "libaicpu_msq_operator.so") == 0)) {
        return reinterpret_cast<void *>(kMsqOperatorHandle);
    }
    return nullptr;
}

inline void *DlsymMsqOperatorStub(void *handle, const char *symbol)
{
    if ((handle == nullptr) || (symbol == nullptr) ||
        (reinterpret_cast<uintptr_t>(handle) != kMsqOperatorHandle)) {
        return nullptr;
    }

    if ((std::strcmp(symbol, "MsqV1ResetT0Status") == 0) || (std::strcmp(symbol, "MsqV1ResetT1Status") == 0) ||
        (std::strcmp(symbol, "MsqV2ResetT0Status") == 0) || (std::strcmp(symbol, "MsqV2ResetT1Status") == 0)) {
        return reinterpret_cast<void *>(&MsqOperatorResetStub);
    }
    if ((std::strcmp(symbol, "MsqV1ReadT0Status") == 0) || (std::strcmp(symbol, "MsqV1ReadT1Status") == 0) ||
        (std::strcmp(symbol, "MsqV2ReadT1Status") == 0)) {
        return reinterpret_cast<void *>(&MsqOperatorReadStatusStub);
    }
    if ((std::strcmp(symbol, "MsqV1ReadT0Data") == 0) || (std::strcmp(symbol, "MsqV1ReadT1Data") == 0) ||
        (std::strcmp(symbol, "MsqV2ReadT1Data") == 0)) {
        return reinterpret_cast<void *>(&MsqOperatorReadDataStub);
    }
    if ((std::strcmp(symbol, "MsqV1SendT0Response") == 0) || (std::strcmp(symbol, "MsqV1SendT1Response") == 0) ||
        (std::strcmp(symbol, "MsqV2SendT1Response") == 0)) {
        return reinterpret_cast<void *>(&MsqOperatorSendRspStub);
    }
    if (std::strcmp(symbol, "Wait") == 0) {
        return reinterpret_cast<void *>(&MsqOperatorWaitStub);
    }

    return nullptr;
}
}  // namespace AicpuScheduleUtStub

#endif
