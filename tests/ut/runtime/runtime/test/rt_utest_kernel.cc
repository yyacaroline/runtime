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
#define private public
#include "runtime.hpp"
#include "program.hpp"
#undef private
#include "kernel.hpp"
#include "kernel_utils.hpp"
#include "thread_local_container.hpp"
#include "para_convertor.hpp"
#include "args_buffer_guard.hpp"
#include "args_handle_allocator.hpp"

using namespace testing;
using namespace cce::runtime;

class KernelTest : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
        GlobalMockObject::verify();
    }
};

TEST_F(KernelTest, kernel_create)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    int32_t fun1;
    Kernel * k1 = new Kernel("f1", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    k1->SetStub_(&fun1);

    EXPECT_EQ(k1->Name_(), "f1");
    EXPECT_EQ(k1->Stub_(), &fun1);
    EXPECT_EQ(k1->Offset_(), 10);
    EXPECT_EQ((Program *)k1->Program_(), program);

    delete k1;
}

TEST_F(KernelTest, kernel_create_second)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    int32_t fun1;
    Kernel * k1 = new Kernel("f1", 1, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    k1->SetStub_(&fun1);

    EXPECT_EQ(k1->Name_(), "f1");
    EXPECT_EQ(k1->Stub_(), &fun1);
    EXPECT_EQ(k1->Offset_(), 10);
    EXPECT_EQ(k1->TilingKey(), 1);
    EXPECT_EQ((Program *)k1->Program_(), program);

    delete k1;
}

TEST_F(KernelTest, kernel_create_third)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    int32_t fun1;
    Kernel * k1 = new Kernel("f1", 1, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    k1->SetStub_(&fun1);
    k1->SetMixType(3);
    k1->SetOffset2(11);
    EXPECT_EQ(k1->Name_(), "f1");
    EXPECT_EQ(k1->Stub_(), &fun1);
    EXPECT_EQ(k1->Offset_(), 10);
    EXPECT_EQ(k1->TilingKey(), 1);
    EXPECT_EQ(k1->Offset2_(), 11);
    EXPECT_EQ(k1->GetMixType(), 3);
    EXPECT_EQ((Program *)k1->Program_(), program);

    delete k1;
}

TEST_F(KernelTest, kernel_lookup)
{
    rtError_t error;
    KernelTable table;
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    int32_t fun1, fun2;

    MOCKER_CPP(&Runtime::GetProgram)
        .stubs()
        .will(returnValue(true));

    Kernel * k1 = new Kernel("f1", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    Kernel * k2 = new Kernel("f2", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    k1->SetStub_(&fun1);
    k2->SetStub_(&fun2);

    error = table.Add(k1);
    EXPECT_EQ(error, RT_ERROR_NONE);

    error = table.Add(k2);
    EXPECT_EQ(error, RT_ERROR_NONE);
    auto &str = program->GetKernelNamesBuffer();
    const Kernel *kernel;

    kernel = table.Lookup(&fun1);
    EXPECT_EQ(kernel, k1);

    kernel = table.Lookup(&fun2);
    EXPECT_EQ(kernel, k2);
}

TEST_F(KernelTest, kernel_remove)
{
    rtError_t error;
    KernelTable table;
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    int32_t fun1, fun2;

    Kernel * k1 = new Kernel("f1", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    Kernel * k2 = new Kernel("f2", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    k1->SetStub_(&fun1);
    k2->SetStub_(&fun2);

    error = table.Add(k1);
    EXPECT_EQ(error, RT_ERROR_NONE);

    error = table.Add(k2);
    EXPECT_EQ(error, RT_ERROR_NONE);

    error = table.RemoveAll(program);
    EXPECT_EQ(error, RT_ERROR_NONE);

    const Kernel *kernel;

    kernel = table.Lookup(&fun1);
    EXPECT_EQ(kernel, (const Kernel *)NULL);

    kernel = table.Lookup(&fun2);
    EXPECT_EQ(kernel, (const Kernel *)NULL);
}

TEST_F(KernelTest, kernel_alloc_kernel_arr)
{
    rtError_t error;
    KernelTable table;
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    int32_t fun[2049], f[2049];
    Kernel * k[2049];

    for(int i = 0; i < 2049; i++)
    {
        k[i] = new Kernel("f[i]", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
        k[i]->SetStub_(&fun[i]);

        error = table.Add(k[i]);
        EXPECT_EQ(error, RT_ERROR_NONE);
    }

    error = table.AllocKernelArr();
    EXPECT_EQ(error, RT_ERROR_NONE);
}

TEST_F(KernelTest, kernel_all_kernel_add)
{
    rtError_t error;
    KernelTable table;

    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;

    PlainProgram stubProg2(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program2 = &stubProg2;
    uint64_t key1 = 1U;
    uint64_t key2 = 2U;
    Kernel *k1 = new Kernel("", key1, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    Kernel *k2 = new Kernel("", key2, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    Kernel *k3 = new Kernel("", key1, program2, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    bool addKernelFlag = true;

    program->kernelCount_ = 2;
    program2->kernelCount_ = 1;
    if (program->KernelTable_ == nullptr) {
        program->KernelTable_ = new rtKernelArray_t[program->kernelCount_];
    }

    if (program2->KernelTable_ == nullptr) {
        program2->KernelTable_ = new rtKernelArray_t[program2->kernelCount_];
    }

    error = program->AllKernelAdd(k1, addKernelFlag);
    EXPECT_EQ(error, RT_ERROR_NONE);

    error = program->AllKernelAdd(k1, addKernelFlag);
    EXPECT_EQ(error, RT_ERROR_NONE);

    error = program2->AllKernelAdd(k3, addKernelFlag);
    EXPECT_EQ(error, RT_ERROR_NONE);

    MOCKER_CPP(&Runtime::GetProgram)
        .stubs()
        .will(returnValue(true));

    const Kernel *kernel;
    kernel = program->AllKernelLookup(1);
    EXPECT_NE(kernel, (const Kernel *)NULL);

    error = program->AllKernelAdd(k2, addKernelFlag);
    EXPECT_EQ(error, RT_ERROR_NONE);

    error = table.RemoveAll(program);
    EXPECT_EQ(error, RT_ERROR_NONE);

    error = table.RemoveAll(program2);
    EXPECT_EQ(error, RT_ERROR_NONE);
}

TEST_F(KernelTest, kernel_info_extern_lookup)
{
    rtError_t error;
    KernelTable table;
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    int32_t fun1, fun2;

    MOCKER_CPP(&Runtime::GetProgram)
        .stubs()
        .will(returnValue(true));
    Kernel * k1 = new Kernel("f1", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    error = table.Add(k1);
    const char_t * const stubFunc = "test";
    const Kernel *tempKernel = table.KernelInfoExtLookup(stubFunc);
    EXPECT_EQ(tempKernel, nullptr);
}

TEST_F(KernelTest, Kernel_ParamInfo_GetParamInfo_Success)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    std::shared_ptr<ElfParamInfo[]> paramInfos(new ElfParamInfo[2]);
    paramInfos[0].info.offset = 0;
    paramInfos[0].info.size = 16;
    paramInfos[1].info.offset = 16;
    paramInfos[1].info.size = 32;

    kernel->SetHasParamSummary(true);
    kernel->SetParamCount(2);
    kernel->SetParamInfos(paramInfos);

    uint32_t offset = 0;
    uint32_t size = 0;
    rtError_t error = kernel->GetParamInfo(0, &offset, &size);
    EXPECT_EQ(error, RT_ERROR_NONE);
    EXPECT_EQ(offset, 0);
    EXPECT_EQ(size, 16);

    error = kernel->GetParamInfo(1, &offset, &size);
    EXPECT_EQ(error, RT_ERROR_NONE);
    EXPECT_EQ(offset, 16);
    EXPECT_EQ(size, 32);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_GetParamInfo_InvalidIndex)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    std::shared_ptr<ElfParamInfo[]> paramInfos(new ElfParamInfo[1]);
    paramInfos[0].info.offset = 0;
    paramInfos[0].info.size = 16;

    kernel->SetHasParamSummary(true);
    kernel->SetParamCount(1);
    kernel->SetParamInfos(paramInfos);

    uint32_t offset = 0;
    uint32_t size = 0;
    rtError_t error = kernel->GetParamInfo(2, &offset, &size);
    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_GetParamInfo_NoParamInfo)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    kernel->SetHasParamSummary(false);
    kernel->SetParamCount(0);

    uint32_t offset = 0;
    uint32_t size = 0;
    rtError_t error = kernel->GetParamInfo(0, &offset, &size);
    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_GetParamInfo_NullptrParamInfos)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    kernel->SetHasParamSummary(true);
    kernel->SetParamCount(1);
    kernel->SetParamInfos(std::shared_ptr<ElfParamInfo[]>());

    uint32_t offset = 0;
    uint32_t size = 0;
    rtError_t error = kernel->GetParamInfo(0, &offset, &size);
    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_GetParamInfo_NullptrOutput)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    std::shared_ptr<ElfParamInfo[]> paramInfos(new ElfParamInfo[1]);
    paramInfos[0].info.offset = 0;
    paramInfos[0].info.size = 16;

    kernel->SetHasParamSummary(true);
    kernel->SetParamCount(1);
    kernel->SetParamInfos(paramInfos);

    rtError_t error = kernel->GetParamInfo(0, nullptr, nullptr);
    EXPECT_EQ(error, RT_ERROR_NONE);

    uint32_t offset = 0;
    error = kernel->GetParamInfo(0, &offset, nullptr);
    EXPECT_EQ(error, RT_ERROR_NONE);
    EXPECT_EQ(offset, 0);

    uint32_t size = 0;
    error = kernel->GetParamInfo(0, nullptr, &size);
    EXPECT_EQ(error, RT_ERROR_NONE);
    EXPECT_EQ(size, 16);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_CopyParamsToBuffer_Success)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    std::shared_ptr<ElfParamInfo[]> paramInfos(new ElfParamInfo[2]);
    paramInfos[0].info.offset = 0;
    paramInfos[0].info.size = 16;
    paramInfos[1].info.offset = 16;
    paramInfos[1].info.size = 32;

    kernel->SetHasParamSummary(true);
    kernel->SetParamCount(2);
    kernel->SetParamInfos(paramInfos);

    char src1[16] = {0};
    char src2[32] = {0};
    void *argsArray[2] = {src1, src2};
    char dest[48] = {0};

    rtError_t error = CopyKernelParamsToBuffer(kernel, argsArray, dest);
    EXPECT_EQ(error, RT_ERROR_NONE);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_CopyParamsToBuffer_GetParamInfoFailure)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    kernel->SetHasParamSummary(false);
    kernel->SetParamCount(1);

    char src[16] = {0};
    void *argsArray[1] = {src};
    char dest[16] = {0};

    rtError_t error = CopyKernelParamsToBuffer(kernel, argsArray, dest);
    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_CopyParamsToBuffer_MemcpyFailure)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    std::shared_ptr<ElfParamInfo[]> paramInfos(new ElfParamInfo[1]);
    paramInfos[0].info.offset = 0;
    paramInfos[0].info.size = 16;

    kernel->SetHasParamSummary(true);
    kernel->SetParamCount(1);
    kernel->SetParamInfos(paramInfos);

    char src[16] = {0};
    void *argsArray[1] = {src};
    char dest[16] = {0};

    MOCKER(memcpy_s).stubs().will(returnValue(1));
    rtError_t error = CopyKernelParamsToBuffer(kernel, argsArray, dest);
    EXPECT_EQ(error, RT_ERROR_SEC_HANDLE);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_CopyParamsToBuffer_NullArrayElement)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    kernel->SetHasParamSummary(true);
    kernel->SetParamCount(1);

    void *argsArray[1] = {nullptr};
    char dest[16] = {0};

    rtError_t error = CopyKernelParamsToBuffer(kernel, argsArray, dest);
    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_SetAndGetParamTotalSize)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    kernel->SetParamTotalSize(128);
    uint64_t size = kernel->GetParamTotalSize();
    EXPECT_EQ(size, 128);

    kernel->SetParamTotalSize(256);
    size = kernel->GetParamTotalSize();
    EXPECT_EQ(size, 256);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_SetAndGetParamCount)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    kernel->SetParamCount(5);
    uint32_t count = kernel->GetParamCount();
    EXPECT_EQ(count, 5);

    kernel->SetParamCount(10);
    count = kernel->GetParamCount();
    EXPECT_EQ(count, 10);

    delete kernel;
}

TEST_F(KernelTest, Kernel_ParamInfo_SetAndGetHasParamSummary)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);

    kernel->SetHasParamSummary(true);
    bool hasInfo = kernel->HasParamSummary();
    EXPECT_TRUE(hasInfo);

    kernel->SetHasParamSummary(false);
    hasInfo = kernel->HasParamSummary();
    EXPECT_FALSE(hasInfo);

    delete kernel;
}

// ConvertArgsArrayToArgsEx tests
TEST(ParaConvertorTest, ConvertArgsArrayToArgsEx_ZeroSize)
{
    PlainProgram program(RT_KERNEL_ATTR_TYPE_AICORE);
    Kernel kernel("test", 0ULL, &program, RT_KERNEL_ATTR_TYPE_AICORE, 10);
    kernel.SetParamTotalSize(0);

    void *argsArray[2] = {(void*)0x10, (void*)0x20};
    rtArgsEx_t argsEx = {};

    rtError_t error = ConvertArgsArrayToArgsEx(argsEx, &kernel, argsArray);

    EXPECT_EQ(error, RT_ERROR_NONE);
    EXPECT_EQ(argsEx.args, nullptr);
    EXPECT_EQ(argsEx.argsSize, 0U);
    EXPECT_EQ(argsEx.isNoNeedH2DCopy, 1U);
}

TEST(ParaConvertorTest, ConvertArgsArrayToArgsEx_CopyParamsFail)
{
    PlainProgram program(RT_KERNEL_ATTR_TYPE_AICORE);
    Kernel kernel("test", 0ULL, &program, RT_KERNEL_ATTR_TYPE_AICORE, 10);
    kernel.SetParamTotalSize(48);
    kernel.SetHasParamSummary(true);
    kernel.SetParamCount(2);

    std::shared_ptr<ElfParamInfo[]> paramInfos(new ElfParamInfo[2]);
    paramInfos[0].info.offset = 0;
    paramInfos[0].info.size = 16;
    paramInfos[1].info.offset = 16;
    paramInfos[1].info.size = 32;
    kernel.SetParamInfos(paramInfos);

    char src[16] = {0};
    void *argsArray[2] = {src, nullptr};  // argsArray[1]为nullptr会导致CopyParams失败
    
    rtArgsEx_t argsEx = {};

    rtError_t error = ConvertArgsArrayToArgsEx(argsEx, &kernel, argsArray);

    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);
}

TEST(ParaConvertorTest, ConvertArgsArrayToArgsEx_Success)
{
    PlainProgram program(RT_KERNEL_ATTR_TYPE_AICORE);
    Kernel kernel("test", 0ULL, &program, RT_KERNEL_ATTR_TYPE_AICORE, 10);
    kernel.SetParamTotalSize(48);
    kernel.SetHasParamSummary(true);
    kernel.SetParamCount(2);

    std::shared_ptr<ElfParamInfo[]> paramInfos(new ElfParamInfo[2]);
    paramInfos[0].info.offset = 0;
    paramInfos[0].info.size = 16;
    paramInfos[1].info.offset = 16;
    paramInfos[1].info.size = 32;
    kernel.SetParamInfos(paramInfos);

    char src1[16] = {1};
    char src2[32] = {2};
    void *argsArray[2] = {src1, src2};
    rtArgsEx_t argsEx = {};

    rtError_t error = ConvertArgsArrayToArgsEx(argsEx, &kernel, argsArray);

    EXPECT_EQ(error, RT_ERROR_NONE);
    EXPECT_NE(argsEx.args, nullptr);
    EXPECT_EQ(argsEx.argsSize, 48U);
    EXPECT_EQ(argsEx.isNoNeedH2DCopy, 0U);
}

// ArgsBufferGuard tests
TEST(ArgsBufferGuardTest, FirstAlloc)
{
    ArgsBufferGuard guard;
    EXPECT_EQ(guard.buffer_, nullptr);

    void *buffer = guard.EnsureCapacity(1024);

    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(guard.size_, 4096ULL);
}

TEST(ArgsBufferGuardTest, BufferEnough)
{
    ArgsBufferGuard guard;
    guard.EnsureCapacity(2048);

    void *buffer1 = guard.buffer_;
    void *buffer2 = guard.EnsureCapacity(1024);

    EXPECT_EQ(buffer2, buffer1);
    EXPECT_EQ(guard.size_, 4096ULL);
}

TEST(ArgsBufferGuardTest, ReallocNeeded)
{
    ArgsBufferGuard guard;
    guard.EnsureCapacity(1024);  // size_=4096

    void *oldBuffer = guard.buffer_;
    uint64_t oldSize = guard.size_;
    
    void *newBuffer = guard.EnsureCapacity(4096 * 2);  // request 8192 > 4096

    EXPECT_NE(newBuffer, nullptr);
    EXPECT_EQ(guard.size_, 8192ULL);  // 验证size确实更新了
    // 如果size更新了，那么buffer一定重新分配了
}

TEST(ArgsBufferGuardTest, MallocFail)
{
    ArgsBufferGuard guard;
    MOCKER(malloc).stubs().will(returnValue((void*)nullptr));
    
    void *buffer = guard.EnsureCapacity(1024);
    
    EXPECT_EQ(buffer, nullptr);
}

TEST_F(KernelTest, Kernel_GetParamInfo_Success)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    kernel->SetParamCount(3);
    kernel->SetHasParamSummary(true);
    
    std::shared_ptr<ElfParamInfo[]> paramInfos(new ElfParamInfo[3]);
    paramInfos[0].info.offset = 0;
    paramInfos[0].info.size = 16;
    paramInfos[1].info.offset = 16;
    paramInfos[1].info.size = 32;
    paramInfos[2].info.offset = 48;
    paramInfos[2].info.size = 64;
    kernel->SetParamInfos(paramInfos);
    
    uint32_t paramOffset = 0;
    uint32_t paramSize = 0;
    
    rtError_t error = kernel->GetParamInfo(0, &paramOffset, &paramSize);
    EXPECT_EQ(error, RT_ERROR_NONE);
    EXPECT_EQ(paramOffset, 0U);
    EXPECT_EQ(paramSize, 16U);
    
    error = kernel->GetParamInfo(1, &paramOffset, &paramSize);
    EXPECT_EQ(error, RT_ERROR_NONE);
    EXPECT_EQ(paramOffset, 16U);
    EXPECT_EQ(paramSize, 32U);
    
    error = kernel->GetParamInfo(2, &paramOffset, &paramSize);
    EXPECT_EQ(error, RT_ERROR_NONE);
    EXPECT_EQ(paramOffset, 48U);
    EXPECT_EQ(paramSize, 64U);
    
    delete kernel;
}

TEST_F(KernelTest, Kernel_GetParamInfo_NoParamSummary)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    kernel->SetParamCount(3);
    kernel->SetHasParamSummary(false);
    
    uint32_t paramOffset = 0;
    uint32_t paramSize = 0;
    
    rtError_t error = kernel->GetParamInfo(0, &paramOffset, &paramSize);
    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);
    
    delete kernel;
}

TEST_F(KernelTest, Kernel_GetParamInfo_IndexOutOfRange)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    kernel->SetParamCount(2);
    kernel->SetHasParamSummary(true);
    
    std::shared_ptr<ElfParamInfo[]> paramInfos(new ElfParamInfo[2]);
    paramInfos[0].info.offset = 0;
    paramInfos[0].info.size = 16;
    paramInfos[1].info.offset = 16;
    paramInfos[1].info.size = 32;
    kernel->SetParamInfos(paramInfos);
    
    uint32_t paramOffset = 0;
    uint32_t paramSize = 0;
    
    rtError_t error = kernel->GetParamInfo(5, &paramOffset, &paramSize);
    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);
    
    delete kernel;
}

TEST_F(KernelTest, Kernel_GetParamInfo_NullParamInfos)
{
    PlainProgram stubProg(RT_KERNEL_ATTR_TYPE_AICPU);
    Program *program = &stubProg;
    
    Kernel *kernel = new Kernel("test", 0ULL, program, RT_KERNEL_ATTR_TYPE_AICPU, 10);
    kernel->SetParamCount(2);
    kernel->SetHasParamSummary(true);
    kernel->SetParamInfos(std::shared_ptr<ElfParamInfo[]>()); // nullptr
    
    uint32_t paramOffset = 0;
    uint32_t paramSize = 0;
    
    rtError_t error = kernel->GetParamInfo(0, &paramOffset, &paramSize);
    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);
    
    delete kernel;
}