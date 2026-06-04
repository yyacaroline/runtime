/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <dlfcn.h>
#include <cstring>

#include "gtest/gtest.h"
#include "mockcpp/mockcpp.hpp"
#include "aicpusd_status.h"

#define private public
#include "aicpusd_msq_operator_manager.h"
#undef private

using namespace AicpuSchedule;

namespace {
void ResetStaticState()
{
    MsqOperatorManager::handle_ = nullptr;
    MsqOperatorManager::inited_ = false;
    MsqOperatorManager::v1ResetT0Status_ = nullptr;
    MsqOperatorManager::v1ResetT1Status_ = nullptr;
    MsqOperatorManager::v1ReadT0Status_ = nullptr;
    MsqOperatorManager::v1ReadT1Status_ = nullptr;
    MsqOperatorManager::v1ReadT0Data_ = nullptr;
    MsqOperatorManager::v1ReadT1Data_ = nullptr;
    MsqOperatorManager::v1SendT0Response_ = nullptr;
    MsqOperatorManager::v1SendT1Response_ = nullptr;
    MsqOperatorManager::v2ResetT0Status_ = nullptr;
    MsqOperatorManager::v2ResetT1Status_ = nullptr;
    MsqOperatorManager::v2ReadT1Status_ = nullptr;
    MsqOperatorManager::v2ReadT1Data_ = nullptr;
    MsqOperatorManager::v2SendT1Response_ = nullptr;
    MsqOperatorManager::waitFunc_ = nullptr;
}

void FakeReset()
{
}

MsqStatus FakeReadStatus()
{
    MsqStatus status = {};
    status.valid = 1U;
    status.size = 2U;
    status.comp = 3U;
    return status;
}

void FakeReadData(uint32_t msgSize, MsqDatas *datas)
{
    if (datas == nullptr) {
        return;
    }
    datas->data0 = msgSize;
    datas->data1 = msgSize + 1U;
}

void FakeSendRsp()
{
}

void FakeWait()
{
}

void *FakeDlopen(const char *, int)
{
    return reinterpret_cast<void *>(0x1234);
}

void *FakeDlsymSuccess(void *, const char *name)
{
    if ((std::strcmp(name, "MsqV1ResetT0Status") == 0) || (std::strcmp(name, "MsqV1ResetT1Status") == 0) ||
        (std::strcmp(name, "MsqV2ResetT0Status") == 0) || (std::strcmp(name, "MsqV2ResetT1Status") == 0)) {
        return reinterpret_cast<void *>(&FakeReset);
    }
    if ((std::strcmp(name, "MsqV1ReadT0Status") == 0) || (std::strcmp(name, "MsqV1ReadT1Status") == 0) ||
        (std::strcmp(name, "MsqV2ReadT1Status") == 0)) {
        return reinterpret_cast<void *>(&FakeReadStatus);
    }
    if ((std::strcmp(name, "MsqV1ReadT0Data") == 0) || (std::strcmp(name, "MsqV1ReadT1Data") == 0) ||
        (std::strcmp(name, "MsqV2ReadT1Data") == 0)) {
        return reinterpret_cast<void *>(&FakeReadData);
    }
    if ((std::strcmp(name, "MsqV1SendT0Response") == 0) || (std::strcmp(name, "MsqV1SendT1Response") == 0) ||
        (std::strcmp(name, "MsqV2SendT1Response") == 0)) {
        return reinterpret_cast<void *>(&FakeSendRsp);
    }
    if (std::strcmp(name, "Wait") == 0) {
        return reinterpret_cast<void *>(&FakeWait);
    }
    return static_cast<void *>(nullptr);
}

void *FakeDlsymFail(void *, const char *)
{
    return static_cast<void *>(nullptr);
}

bool g_resetCalled = false;
bool g_resetT1Called = false;
bool g_sendCalled = false;
bool g_sendT1Called = false;
bool g_waitCalled = false;
uint32_t g_readStatusCalled = 0U;
uint32_t g_readStatusT1Called = 0U;
uint32_t g_lastMsgSize = 0U;
uint32_t g_lastMsgSizeT1 = 0U;

void MarkReset()
{
    g_resetCalled = true;
}

void MarkResetT1()
{
    g_resetT1Called = true;
}

MsqStatus MarkReadStatus()
{
    ++g_readStatusCalled;
    MsqStatus status = {};
    status.valid = 1U;
    status.size = 3U;
    status.comp = 1U;
    return status;
}

MsqStatus MarkReadStatusT1()
{
    ++g_readStatusT1Called;
    MsqStatus status = {};
    status.valid = 1U;
    status.size = 2U;
    status.comp = 0U;
    return status;
}

void MarkReadData(uint32_t msgSize, MsqDatas *datas)
{
    g_lastMsgSize = msgSize;
    if (datas != nullptr) {
        datas->data0 = 0x55U;
    }
}

void MarkReadDataT1(uint32_t msgSize, MsqDatas *datas)
{
    g_lastMsgSizeT1 = msgSize;
    if (datas != nullptr) {
        datas->data1 = 0x66U;
    }
}

void MarkSend()
{
    g_sendCalled = true;
}

void MarkSendT1()
{
    g_sendT1Called = true;
}

void MarkWait()
{
    g_waitCalled = true;
}

void PrepareForwardLoadedSymbolState()
{
    MsqOperatorManager::v1ResetT0Status_ = &MarkReset;
    MsqOperatorManager::v1ResetT1Status_ = &MarkResetT1;
    MsqOperatorManager::v1ReadT0Status_ = &MarkReadStatus;
    MsqOperatorManager::v1ReadT1Status_ = &MarkReadStatusT1;
    MsqOperatorManager::v1ReadT0Data_ = &MarkReadData;
    MsqOperatorManager::v1ReadT1Data_ = &MarkReadDataT1;
    MsqOperatorManager::v1SendT0Response_ = &MarkSend;
    MsqOperatorManager::v1SendT1Response_ = &MarkSendT1;
    MsqOperatorManager::v2ResetT0Status_ = &MarkReset;
    MsqOperatorManager::v2ResetT1Status_ = &MarkResetT1;
    MsqOperatorManager::v2ReadT1Status_ = &MarkReadStatusT1;
    MsqOperatorManager::v2ReadT1Data_ = &MarkReadDataT1;
    MsqOperatorManager::v2SendT1Response_ = &MarkSendT1;
    MsqOperatorManager::waitFunc_ = &MarkWait;
}
}  // namespace

class AicpusdMsqOperatorManagerTest : public testing::Test {
protected:
    static void SetUpTestCase()
    {
        ResetStaticState();
    }

    static void TearDownTestCase()
    {
        ResetStaticState();
    }

    void SetUp() override
    {
        ResetStaticState();
        g_resetCalled = false;
        g_resetT1Called = false;
        g_sendCalled = false;
        g_sendT1Called = false;
        g_waitCalled = false;
        g_readStatusCalled = 0U;
        g_readStatusT1Called = 0U;
        g_lastMsgSize = 0U;
        g_lastMsgSizeT1 = 0U;
    }

    void TearDown() override
    {
        ResetStaticState();
        GlobalMockObject::verify();
    }
};

TEST_F(AicpusdMsqOperatorManagerTest, InitFailWhenDlopenFails)
{
    MOCKER(dlopen).stubs().will(returnValue(static_cast<void *>(nullptr)));
    EXPECT_EQ(MsqOperatorManager::Init(), AICPU_SCHEDULE_ERROR_INNER_ERROR);
    EXPECT_EQ(MsqOperatorManager::handle_, nullptr);
    EXPECT_FALSE(MsqOperatorManager::inited_);
}

TEST_F(AicpusdMsqOperatorManagerTest, InitFailWhenLoadSymbolFails)
{
    MOCKER(dlopen).stubs().will(invoke(FakeDlopen));
    MOCKER(dlsym).stubs().will(invoke(FakeDlsymFail));
    MOCKER(dlclose).stubs().will(returnValue(0));
    EXPECT_EQ(MsqOperatorManager::Init(), AICPU_SCHEDULE_ERROR_INNER_ERROR);
    EXPECT_EQ(MsqOperatorManager::handle_, nullptr);
    EXPECT_FALSE(MsqOperatorManager::inited_);
}

TEST_F(AicpusdMsqOperatorManagerTest, InitSuccessAndAlreadyInited)
{
    MOCKER(dlopen).stubs().will(invoke(FakeDlopen));
    MOCKER(dlsym).stubs().will(invoke(FakeDlsymSuccess));

    EXPECT_EQ(MsqOperatorManager::Init(), AICPU_SCHEDULE_OK);
    EXPECT_NE(MsqOperatorManager::handle_, nullptr);
    EXPECT_TRUE(MsqOperatorManager::inited_);
    EXPECT_EQ(MsqOperatorManager::Init(), AICPU_SCHEDULE_OK);
}

TEST_F(AicpusdMsqOperatorManagerTest, FinalizeClosesHandleAndClearsState)
{
    MsqOperatorManager::handle_ = reinterpret_cast<void *>(0x1234);
    MsqOperatorManager::inited_ = true;
    MsqOperatorManager::v1ResetT0Status_ = &FakeReset;
    MsqOperatorManager::v1ReadT0Status_ = &FakeReadStatus;
    MsqOperatorManager::v1ReadT0Data_ = &FakeReadData;
    MsqOperatorManager::v1SendT0Response_ = &FakeSendRsp;
    MsqOperatorManager::waitFunc_ = &FakeWait;

    MOCKER(dlclose).stubs().will(returnValue(0));
    MsqOperatorManager::Finalize();

    EXPECT_EQ(MsqOperatorManager::handle_, nullptr);
    EXPECT_FALSE(MsqOperatorManager::inited_);
    EXPECT_EQ(MsqOperatorManager::v1ResetT0Status_, nullptr);
    EXPECT_EQ(MsqOperatorManager::v1ReadT0Status_, nullptr);
    EXPECT_EQ(MsqOperatorManager::v1ReadT0Data_, nullptr);
    EXPECT_EQ(MsqOperatorManager::v1SendT0Response_, nullptr);
    EXPECT_EQ(MsqOperatorManager::waitFunc_, nullptr);
}

TEST_F(AicpusdMsqOperatorManagerTest, CallFunctionsForwardToLoadedSymbolsV1)
{
    PrepareForwardLoadedSymbolState();

    MsqOperatorManager::CallV1ResetT0Status();
    EXPECT_TRUE(g_resetCalled);
    MsqOperatorManager::CallV1ResetT1Status();
    EXPECT_TRUE(g_resetT1Called);

    (void)MsqOperatorManager::CallV1ReadT0Status();
    EXPECT_EQ(g_readStatusCalled, 1U);
    (void)MsqOperatorManager::CallV1ReadT1Status();
    EXPECT_EQ(g_readStatusT1Called, 1U);

    MsqDatas datas = {};
    MsqOperatorManager::CallV1ReadT0Data(16U, &datas);
    EXPECT_EQ(g_lastMsgSize, 16U);
    EXPECT_EQ(datas.data0, 0x55U);
    MsqOperatorManager::CallV1ReadT1Data(32U, &datas);
    EXPECT_EQ(g_lastMsgSizeT1, 32U);
    EXPECT_EQ(datas.data1, 0x66U);

    MsqOperatorManager::CallV1SendT0Response();
    EXPECT_TRUE(g_sendCalled);
    MsqOperatorManager::CallV1SendT1Response();
    EXPECT_TRUE(g_sendT1Called);
}

TEST_F(AicpusdMsqOperatorManagerTest, CallFunctionsForwardToLoadedSymbolsV2AndWait)
{
    PrepareForwardLoadedSymbolState();
    MsqDatas datas = {};
    g_resetCalled = false;
    g_resetT1Called = false;
    g_sendT1Called = false;
    g_readStatusT1Called = 0U;
    g_lastMsgSizeT1 = 0U;
    MsqOperatorManager::CallV2ResetT0Status();
    EXPECT_TRUE(g_resetCalled);
    MsqOperatorManager::CallV2ResetT1Status();
    EXPECT_TRUE(g_resetT1Called);
    (void)MsqOperatorManager::CallV2ReadT1Status();
    EXPECT_EQ(g_readStatusT1Called, 1U);
    MsqOperatorManager::CallV2ReadT1Data(64U, &datas);
    EXPECT_EQ(g_lastMsgSizeT1, 64U);
    MsqOperatorManager::CallV2SendT1Response();
    EXPECT_TRUE(g_sendT1Called);

    MsqOperatorManager::CallWait();
    EXPECT_TRUE(g_waitCalled);
}
