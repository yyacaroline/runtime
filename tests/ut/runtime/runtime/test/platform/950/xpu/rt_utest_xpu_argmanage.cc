/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "gtest/gtest.h"
#include "mockcpp/mockcpp.hpp"
#include "runtime/rt.h"
#include "utils.h"
#include "xpu_context.hpp"
#include "stars_arg_manager.hpp"
#define private public
#define protected public
#include "kernel.hpp"
#include "program.hpp"
#include "arg_loader_xpu.hpp"
#include "stream_xpu.hpp"
#undef protected
#undef private
#include "xpu_stub.h"
#include "stream.hpp"
#include "runtime.hpp"
#include "api.hpp"
#include "tprt.hpp"
#include "inner_thread_local.hpp"

using namespace testing;
using namespace cce::runtime;

class ArgManageXpuTest : public testing::Test {
protected:
    XpuContext *context_ = nullptr;

    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

    virtual void SetUp()
    {
        MOCKER(drvGetPlatformInfo).stubs().will(invoke(drvGetPlatformInfo_online));
        MOCKER_CPP(&XpuDevice::ParseXpuConfigInfo).stubs().will(invoke(ParseXpuConfigInfo_mock));
        rtError_t error = rtSetXpuDevice(RT_DEV_TYPE_DPU, 0);
        ASSERT_EQ(error, ACL_RT_SUCCESS);
        Runtime *rt = (Runtime *)Runtime::Instance();
        context_ = static_cast<XpuContext *>(rt->GetXpuCtxt());
    }

    virtual void TearDown()
    {
        rtResetXpuDevice(RT_DEV_TYPE_DPU, 0);
        GlobalMockObject::verify();
    }
};

TEST_F(ArgManageXpuTest, xpu_arg_manager_test_01)
{
    const uint32_t prio = RT_STREAM_PRIORITY_DEFAULT;
    const uint32_t flag = 0;
    Stream **result = new Stream*(nullptr);
    rtError_t error = context_->StreamCreate(prio, flag, result);
    EXPECT_EQ(error, RT_ERROR_NONE);

    XpuStream *stream = static_cast<XpuStream *>(context_->StreamList_().front());
    MOCKER(memcpy_s).stubs().will(returnValue(NULL));
    StarsArgLoaderResult result1 = {nullptr, nullptr, nullptr, UINT32_MAX};
    error = stream->ArgManagePtr()->AllocCopyPtr(100, true, LoadPolicy::LP_GENERIC, &result1);
    EXPECT_EQ(error, RT_ERROR_NONE);

    error = stream->ArgManagePtr()->AllocCopyPtr(100, false, LoadPolicy::LP_GENERIC, &result1);
    EXPECT_EQ(error, RT_ERROR_NONE);

    void *args = malloc(100);

    error = stream->ArgManagePtr()->H2DArgCopy(&result1, args, 100);
    EXPECT_EQ(error, RT_ERROR_NONE);

    result1.handle = nullptr;
    error = stream->ArgManagePtr()->H2DArgCopy(&result1, args, 100);
    EXPECT_EQ(error, RT_ERROR_NONE);
    delete result;
    free(args);
 }

 TEST_F(ArgManageXpuTest, xpu_arg_manager_test_02)
{
    const uint32_t prio = RT_STREAM_PRIORITY_DEFAULT;
    const uint32_t flag = 0;
    Stream **result = new Stream*(nullptr);
    MOCKER(malloc).stubs().will(returnValue((void *)NULL));
    rtError_t error = context_->StreamCreate(prio, flag, result);
    EXPECT_NE(error, RT_ERROR_NONE);
    delete result;
 }

TEST_F(ArgManageXpuTest, xpu_alloc_copy_ptr_no_copy)
{
    MOCKER(drvGetPlatformInfo).stubs().will(invoke(drvGetPlatformInfo_online));
    MOCKER_CPP(&XpuDevice::ParseXpuConfigInfo).stubs().will(invoke(ParseXpuConfigInfo_mock));

    rtError_t error = rtSetXpuDevice(RT_DEV_TYPE_DPU, 0);
    EXPECT_EQ(error, ACL_RT_SUCCESS);
    Runtime* rt = (Runtime *)Runtime::Instance();
    XpuContext* context = static_cast<XpuContext *>(rt->GetXpuCtxt());

    const uint32_t prio = RT_STREAM_PRIORITY_DEFAULT;
    const uint32_t flag = 0;
    Stream** result = new(std::nothrow) Stream *(nullptr);
    error = context->StreamCreate(prio, flag, result);
    EXPECT_EQ(error, RT_ERROR_NONE);

    XpuStream* stream = static_cast<XpuStream *>(context->StreamList_().front());
    StarsArgLoaderResult result1 = {nullptr, nullptr, nullptr, UINT32_MAX};
    error = stream->ArgManagePtr()->AllocCopyPtr(100, false, LoadPolicy::LP_GENERIC, &result1);
    EXPECT_EQ(error, RT_ERROR_NONE);
    rtResetXpuDevice(RT_DEV_TYPE_DPU, 0);
    delete result;
}

TEST_F(ArgManageXpuTest, xpu_arg_manager_test_03)
{
    const uint32_t prio = RT_STREAM_PRIORITY_DEFAULT;
    const uint32_t flag = 0;
    Stream **result = new Stream*(nullptr);
    rtError_t error = context_->StreamCreate(prio, flag, result);
    EXPECT_EQ(error, RT_ERROR_NONE);
    XpuArgManage *xpuArgsManager = new XpuArgManage(context_->StreamList_().front());
    StarsArgLoaderResult result1 = {nullptr, nullptr, nullptr, UINT32_MAX};
    MOCKER_CPP(&StarsArgManager::AllocStmArgPos).stubs().will(returnValue(false));
    xpuArgsManager->AllocStmPool(100, &result1);
    delete xpuArgsManager;
    delete result;
 }

TEST_F(ArgManageXpuTest, xpu_arg_manager_H2DArgCopy_MemcpyFail)
{
    const uint32_t prio = RT_STREAM_PRIORITY_DEFAULT;
    const uint32_t flag = 0;
    Stream **result = new Stream *(nullptr);
    rtError_t error = context_->StreamCreate(prio, flag, result);
    EXPECT_EQ(error, RT_ERROR_NONE);

    XpuStream *stream = static_cast<XpuStream *>(context_->StreamList_().front());

    // Set up result with handle=nullptr to enter else branch (memcpy_s path)
    void *args = malloc(100);
    StarsArgLoaderResult result1 = {};
    result1.kerArgs = malloc(100);
    result1.handle = nullptr;

    // Mock memcpy_s to fail -> triggers lines 107, 114
    MOCKER(memcpy_s).stubs().will(returnValue(1));

    error = stream->ArgManagePtr()->H2DArgCopy(&result1, args, 100);
    EXPECT_EQ(error, RT_ERROR_DRV_MEMORY);

    free(args);
    free(result1.kerArgs);
    delete result;
}

TEST_F(ArgManageXpuTest, XpuArgManage_LoadArgsFromArray_NotSupport)
{
    const uint32_t prio = RT_STREAM_PRIORITY_DEFAULT;
    const uint32_t flag = 0;
    Stream **result = new Stream *(nullptr);
    rtError_t error = context_->StreamCreate(prio, flag, result);
    EXPECT_EQ(error, RT_ERROR_NONE);
    
    XpuStream *stream = static_cast<XpuStream *>(context_->StreamList_().front());
    
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel kernelMock("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    
    void *argsArray[2] = {(void *)0x100, (void *)0x200};
    StarsArgLoaderResult result1 = {nullptr, nullptr, nullptr, UINT32_MAX};
    
    error = stream->ArgManagePtr()->LoadArgsFromArray(false, &kernelMock, argsArray, &result1);
    EXPECT_EQ(error, RT_ERROR_FEATURE_NOT_SUPPORT);
    
    delete result;
}
