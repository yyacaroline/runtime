/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <queue>
#include "gtest/gtest.h"
#define private public
#include "aicpusd_threads_process.h"
#include "aicpusd_worker.h"
#include "ascend_hal.h"
#undef private

using namespace AicpuSchedule;

class AicpusdLhisiCoverageTest : public testing::Test {};

TEST_F(AicpusdLhisiCoverageTest, ComputeProcessApisReturnStubValues)
{
    std::vector<uint32_t> deviceVec = {0U};
    const std::string pidSign = "pid";
    const AICPUSharderTaskInfo taskInfo = {};
    const aicpu::Closure task = []() {};
    std::queue<aicpu::Closure> taskQueue;
    taskQueue.push(task);

    ComputeProcess &process = ComputeProcess::GetInstance();
    process.UpdateProfilingMode(PROFILING_OPEN);
    process.UpdateProfilingModelMode(true);
    EXPECT_EQ(process.Start(deviceVec, 0, pidSign, PROFILING_OPEN, 0, aicpu::THREAD_MODE), 0);
    process.Stop();
    EXPECT_TRUE(process.DoSplitKernelTask(taskInfo));
    EXPECT_TRUE(process.DoRandomKernelTask());
    EXPECT_EQ(process.SubmitRandomKernelTask(task), 0U);
    EXPECT_EQ(process.SubmitSplitKernelTask(taskInfo, taskQueue), 0U);
    EXPECT_EQ(process.SubmitBatchSplitKernelEventOneByOne(taskInfo), 0U);
    EXPECT_EQ(process.SubmitBatchSplitKernelEventDc(taskInfo), 0U);
    EXPECT_EQ(process.SubmitOneSplitKernelEvent(taskInfo), 0U);
    EXPECT_TRUE(process.GetAndDoSplitKernelTask());
}

TEST_F(AicpusdLhisiCoverageTest, QueueAndMbufApisReturnSuccess)
{
    void *tmp = nullptr;
    void **buf = &tmp;
    Mbuf *mbuf = nullptr;
    Mbuf *chainMbuf = nullptr;
    uint64_t dataLen = 0U;
    unsigned int size = 0U;
    unsigned int num = 0U;

    EXPECT_EQ(halQueueSubscribe(0U, 0U, 0U, 0), DRV_ERROR_NONE);
    EXPECT_EQ(halQueueUnsubscribe(0U, 0U), DRV_ERROR_NONE);
    EXPECT_EQ(halQueueSubF2NFEvent(0U, 0U, 0U), DRV_ERROR_NONE);
    EXPECT_EQ(halQueueSet(0U, QueueSetCmdType(), nullptr), DRV_ERROR_NONE);
    EXPECT_EQ(halQueueUnsubF2NFEvent(0U, 0U), DRV_ERROR_NONE);
    EXPECT_EQ(halQueueDeQueue(0U, 0U, buf), DRV_ERROR_NONE);
    EXPECT_EQ(halQueueEnQueue(0U, 0U, mbuf), DRV_ERROR_NONE);
    EXPECT_EQ(halMbufGetBuffAddr(mbuf, buf), DRV_ERROR_NONE);
    EXPECT_EQ(halMbufSetDataLen(mbuf, dataLen), DRV_ERROR_NONE);
    EXPECT_EQ(halMbufGetDataLen(mbuf, &dataLen), DRV_ERROR_NONE);
    EXPECT_EQ(halMbufAllocEx(0U, 0U, 0UL, 0, &mbuf), DRV_ERROR_NONE);
    EXPECT_EQ(halMbufFree(mbuf), DRV_ERROR_NONE);
    EXPECT_EQ(halMbufGetPrivInfo(mbuf, buf, &size), DRV_ERROR_NONE);
    EXPECT_EQ(halQueueInit(0U), DRV_ERROR_NONE);
    EXPECT_EQ(drvHdcGetCapacity(nullptr), DRV_ERROR_NONE);
    EXPECT_EQ(halMbufChainGetMbuf(mbuf, 0U, &chainMbuf), DRV_ERROR_NONE);
    EXPECT_EQ(halMbufChainGetMbufNum(mbuf, &num), DRV_ERROR_NONE);
    EXPECT_EQ(halMbufChainAppend(mbuf, chainMbuf), DRV_ERROR_NONE);
}

TEST_F(AicpusdLhisiCoverageTest, OtherStubApisReturnSuccess)
{
    drvBindHostpidInfo info = {};
    unsigned int vfMaxNum = 0U;
    unsigned int vfList = 0U;
    unsigned int vfNum = 0U;

    EXPECT_EQ(halTsDevRecord(0U, 0U, 0U, 0U), 0);
    EXPECT_EQ(drvBindHostPid(info), DRV_ERROR_NONE);
    ThreadPool localPool;
    ThreadPool &threadPool = ThreadPool::Instance();
    threadPool.WaitForStop();
    EXPECT_EQ(threadPool.semInitedNum_, 0);
    EXPECT_EQ(halGetDeviceVfMax(0U, &vfMaxNum), DRV_ERROR_NONE);
    EXPECT_EQ(halGetDeviceVfList(0U, &vfList, 1U, &vfNum), DRV_ERROR_NONE);
}
