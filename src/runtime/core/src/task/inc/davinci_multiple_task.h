/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef RUNTIME_DAVINCI_MULTIPLE_TASK_H
#define RUNTIME_DAVINCI_MULTIPLE_TASK_H

#include "stars.hpp"

namespace cce {
namespace runtime {
rtError_t DavinciMultipleTaskInit(TaskInfo* taskInfo, const void *const multipleTaskInfo, const uint32_t flag);
void ResetCmdList(TaskInfo* taskInfo);
void IncMultipleTaskCqeNum(TaskInfo *taskInfo);
void DecMultipleTaskCqeNum(TaskInfo *taskInfo);
void SetMultipleTaskCqeErrorInfo(TaskInfo *taskInfo, uint8_t sqeType, uint8_t errorType, uint32_t errorCode);
void GetMultipleTaskCqeErrorInfo(TaskInfo * const taskInfo, volatile uint8_t &sqeType,
                                 volatile uint8_t &errorType, volatile uint32_t &errorCode);
uint32_t GetSendSqeNumForDavinciMultipleTask(const TaskInfo * const taskInfo);
uint8_t GetMultipleTaskCqeNum(TaskInfo * const taskInfo);
}  // namespace runtime
}  // namespace cce
#endif  // RUNTIME_DAVINCI_MULTIPLE_TASK_H
