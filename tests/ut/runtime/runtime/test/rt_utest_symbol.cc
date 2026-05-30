/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "runtime/rt.h"
#include "runtime/inner_kernel.h"
#include "runtime.hpp"
#include "api.hpp"
#include "kernel/program.hpp"
#include "kernel/elf.hpp"
#include "securec.h"
#include "gtest/gtest.h"
#include "mockcpp/mockcpp.hpp"

using namespace testing;
using namespace cce::runtime;

class SymbolTest : public testing::Test {
protected:
    static void SetUpTestCase()
    {
        std::cout << "symbol test start" << std::endl;
    }

    static void TearDownTestCase()
    {
        std::cout << "symbol test end" << std::endl;
    }

    virtual void SetUp()
    {
        rtSetDevice(0);
        prog_ = CreateMockElfProgram();
        Runtime::Instance()->GetSymbolTable().Register(prog_, &hostVar_, "test_symbol", sizeof(hostVar_), 0U);
    }

    virtual void TearDown()
    {
        delete prog_;
        prog_ = nullptr;
        rtDeviceReset(0);
        GlobalMockObject::verify();
    }

    ElfProgram *CreateMockElfProgram()
    {
        ElfProgram *prog = new ElfProgram();
        prog->elfData_ = new rtElfData();
        prog->elfData_->globalSymbolMap["test_symbol"] = {0x100, sizeof(int)};
        prog->SetBinBaseAddr((void *)0x8000, 0);
        prog->SetBinAlignBaseAddr((void *)0x8000, 0);
        return prog;
    }

    ElfProgram *prog_ = nullptr;
    int hostVar_ = 0;
};

TEST_F(SymbolTest, rtSymbolLookup_SymbolNotFound)
{
    int hostVarNotFound = 0;
    void *devPtr = nullptr;
    size_t size = 0;

    rtError_t error = rtSymbolLookup(&hostVarNotFound, &devPtr, &size);
    EXPECT_EQ(error, ACL_ERROR_RT_INVALID_SYMBOL);
}

TEST_F(SymbolTest, rtSymbolLookup_Success)
{
    void *devPtr = nullptr;
    size_t size = 0;

    rtError_t error = rtSymbolLookup(&hostVar_, &devPtr, &size);
    EXPECT_EQ(error, ACL_RT_SUCCESS);
    EXPECT_EQ(devPtr, (void *)0x8100);
    EXPECT_EQ(size, sizeof(hostVar_));
}

TEST_F(SymbolTest, SymbolTable_Register_Duplicate)
{
    rtError_t error = Runtime::Instance()->GetSymbolTable().Register(prog_, &hostVar_, "test_symbol", sizeof(hostVar_), 0U);
    EXPECT_EQ(error, RT_ERROR_NONE);
}

TEST_F(SymbolTest, ApiRegisterVariable_DeviceVarNameTooLong)
{
    Api *api = Api::Instance();
    int newHostVar = 0;
    char longName[4097];  // NAME_MAX_LENGTH = 4096
    (void)memset_s(longName, sizeof(longName), 'a', 4096);
    longName[4096] = '\0';

    rtError_t error = api->RegisterVariable(prog_, &newHostVar, longName, sizeof(newHostVar), 0U);
    EXPECT_EQ(error, RT_ERROR_INVALID_VALUE);
}

TEST_F(SymbolTest, ApiRegisterVariable_ImplSuccess)
{
    Api *api = Api::Instance();
    int newHostVar = 0;
    prog_->elfData_->globalSymbolMap["new_symbol"] = {0x200, sizeof(int)};

    rtError_t error = api->RegisterVariable(prog_, &newHostVar, "new_symbol", sizeof(newHostVar), 0U);
    EXPECT_EQ(error, RT_ERROR_NONE);

    void *devPtr = nullptr;
    size_t size = 0;
    error = rtSymbolLookup(&newHostVar, &devPtr, &size);
    EXPECT_EQ(error, ACL_RT_SUCCESS);
}

