/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INC_EXTERNAL_ACL_ACL_RT_API_H_
#define INC_EXTERNAL_ACL_ACL_RT_API_H_

#include "acl_rt.h"

#ifdef __cplusplus

static inline aclError aclrtSynchronizeDevice(int32_t timeout)
{
    return aclrtSynchronizeDeviceWithTimeout(timeout);
}

static inline aclError aclrtSynchronizeStream(aclrtStream stream, int32_t timeout)
{
    return aclrtSynchronizeStreamWithTimeout(stream, timeout);
}

static inline aclError aclrtSynchronizeEvent(aclrtEvent event, int32_t timeout)
{
    return aclrtSynchronizeEventWithTimeout(event, timeout);
}

static inline aclError aclrtStreamWaitEvent(aclrtStream stream, aclrtEvent event, int32_t timeout)
{
    return aclrtStreamWaitEventWithTimeout(stream, event, timeout);
}

static inline aclError aclrtCreateStream(aclrtStream *stream, uint32_t priority, uint32_t flag)
{
    return aclrtCreateStreamWithConfig(stream, priority, flag);
}

static inline aclError aclrtSetOpExecuteTimeOut(uint64_t timeout, uint64_t *actualTimeout)
{
    return aclrtSetOpExecuteTimeOutV2(timeout, actualTimeout);
}

static inline aclError aclrtCreateEvent(aclrtEvent *event, uint32_t flag)
{
    return aclrtCreateEventExWithFlag(event, flag);
}

template <typename T>
static inline aclError aclrtMalloc(T **devPtr, size_t size, aclrtMallocConfig *cfg = nullptr)
{
    return aclrtMallocWithCfg(reinterpret_cast<void **>(devPtr), size, ACL_MEM_MALLOC_HUGE_FIRST, cfg);
}

template <typename T>
static inline aclError aclrtMalloc(T **devPtr, size_t size, aclrtMemMallocPolicy policy, aclrtMallocConfig *cfg = nullptr)
{
    return aclrtMallocWithCfg(reinterpret_cast<void **>(devPtr), size, policy, cfg);
}

template <typename T>
static inline aclError aclrtMallocHost(T **hostPtr, size_t size, aclrtMallocConfig *cfg = nullptr)
{
    return aclrtMallocHostWithCfg(reinterpret_cast<void **>(hostPtr), static_cast<uint64_t>(size), cfg);
}

template <typename T, typename U>
static inline aclError aclrtMemcpy(T *dst, size_t destMax, const U *src, size_t count, aclrtMemcpyKind kind)
{
    return aclrtMemcpy(static_cast<void *>(dst), destMax, static_cast<const void *>(src), count, kind);
}

template <typename T, typename U>
static inline aclError aclrtMemcpyAsync(T *dst, size_t destMax, const U *src, size_t count,
                                        aclrtMemcpyKind kind, aclrtStream stream)
{
    return aclrtMemcpyAsync(static_cast<void *>(dst), destMax, static_cast<const void *>(src), count, kind, stream);
}

template <typename T, typename U>
static inline aclError aclrtMemcpy2d(T *dst, size_t dpitch, const U *src, size_t spitch,
                                     size_t width, size_t height, aclrtMemcpyKind kind)
{
    return aclrtMemcpy2d(static_cast<void *>(dst), dpitch, static_cast<const void *>(src),
                           spitch, width, height, kind);
}

template <typename T, typename U>
static inline aclError aclrtMemcpy2dAsync(T *dst, size_t dpitch, const U *src, size_t spitch,
                                          size_t width, size_t height, aclrtMemcpyKind kind, aclrtStream stream)
{
    return aclrtMemcpy2dAsync(static_cast<void *>(dst), dpitch, static_cast<const void *>(src),
                                spitch, width, height, kind, stream);
}

template <typename T, typename U>
static inline aclError aclrtMemcpyBatch(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes,
                                        size_t numBatches, aclrtMemcpyBatchAttr attr, size_t *failIndex = nullptr)
{
    (void)failIndex;
    aclrtMemcpyBatchAttr attrs[1] = {attr};
    size_t attrsIndexes[1] = {0};
    return aclrtMemcpyBatchV2(reinterpret_cast<void **>(dsts), destMaxs, reinterpret_cast<void **>(srcs),
                                sizes, numBatches, attrs, attrsIndexes, 1);
}

template <typename T, typename U>
static inline aclError aclrtMemcpyBatch(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes,
                                        size_t numBatches, aclrtMemcpyBatchAttr *attrs,
                                        size_t *attrsIndexes, size_t numAttrs, size_t *failIndex = nullptr)
{
    (void)failIndex;
    return aclrtMemcpyBatchV2(reinterpret_cast<void **>(dsts), destMaxs, reinterpret_cast<void **>(srcs),
                                sizes, numBatches, attrs, attrsIndexes, numAttrs);
}

template <typename T, typename U>
static inline aclError aclrtMemcpyBatchAsync(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes,
                                             size_t numBatches, aclrtMemcpyBatchAttr attr, aclrtStream stream)
{
    aclrtMemcpyBatchAttr attrs[1] = {attr};
    size_t attrsIndexes[1] = {0};
    return aclrtMemcpyBatchAsyncV2(reinterpret_cast<void **>(dsts), destMaxs, reinterpret_cast<void **>(srcs),
                                     sizes, numBatches, attrs, attrsIndexes, 1, stream);
}

template <typename T, typename U>
static inline aclError aclrtMemcpyBatchAsync(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes,
                                             size_t numBatches, aclrtMemcpyBatchAttr *attrs,
                                             size_t *attrsIndexes, size_t numAttrs, aclrtStream stream)
{
    return aclrtMemcpyBatchAsyncV2(reinterpret_cast<void **>(dsts), destMaxs, reinterpret_cast<void **>(srcs),
                                     sizes, numBatches, attrs, attrsIndexes, numAttrs, stream);
}

template <typename T, typename U>
static inline aclError aclrtMemcpyBatchAsync(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes,
                                             size_t numBatches, aclrtMemcpyBatchAttr attr,
                                             size_t *failIndex, aclrtStream stream)
{
    aclrtMemcpyBatchAttr attrs[1] = {attr};
    size_t attrsIndexes[1] = {0};
    return ::aclrtMemcpyBatchAsync(reinterpret_cast<void **>(dsts), destMaxs, reinterpret_cast<void **>(srcs),
                                   sizes, numBatches, attrs, attrsIndexes, 1, failIndex, stream);
}

template <typename T, typename U>
static inline aclError aclrtMemcpyBatchAsync(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes,
                                             size_t numBatches, aclrtMemcpyBatchAttr *attrs,
                                             size_t *attrsIndexes, size_t numAttrs, size_t *failIndex, aclrtStream stream)
{
    return ::aclrtMemcpyBatchAsync(reinterpret_cast<void **>(dsts), destMaxs, reinterpret_cast<void **>(srcs),
                                   sizes, numBatches, attrs, attrsIndexes, numAttrs, failIndex, stream);
}

template <typename T>
static inline aclError aclrtPointerGetAttributes(const T *ptr, aclrtPtrAttributes *attributes)
{
    return aclrtPointerGetAttributes(static_cast<const void *>(ptr), attributes);
}

template <typename T>
static inline aclError aclrtHostRegister(T *ptr, uint64_t size, aclrtHostRegisterType type, T **devPtr)
{
    return aclrtHostRegister(static_cast<void *>(ptr), size, type, reinterpret_cast<void **>(devPtr));
}

template <typename T>
static inline aclError aclrtHostRegister(T *ptr, uint64_t size, uint32_t flag)
{
    return aclrtHostRegisterV2(static_cast<void *>(ptr), size, flag);
}

template <typename T>
static inline aclError aclrtHostGetDevicePointer(T *pHost, T **pDevice, uint32_t flag)
{
    return aclrtHostGetDevicePointer(static_cast<void *>(pHost), reinterpret_cast<void **>(pDevice), flag);
}

template <typename T>
static inline aclError aclrtHostUnregister(T *ptr)
{
    return aclrtHostUnregister(static_cast<void *>(ptr));
}

template <typename T>
static inline aclError aclrtMemAllocManaged(T **devPtr, size_t size, uint32_t flags = ACL_RT_MEM_ATTACH_GLOBAL)
{
    return aclrtMemAllocManaged(reinterpret_cast<void **>(devPtr), static_cast<uint64_t>(size), flags);
}

template <typename T>
static inline aclError aclrtMemManagedPrefetchAsync(const T *ptr, size_t size,
                                                    aclrtMemManagedLocation location, uint32_t flags, aclrtStream stream)
{
    return aclrtMemManagedPrefetchAsync(static_cast<const void *>(ptr), size, location, flags, stream);
}

template <typename T>
static inline aclError aclrtMemManagedPrefetchBatchAsync(const T **ptrs, size_t *sizes, size_t count,
                                                         aclrtMemManagedLocation prefetchLoc,
                                                         uint64_t flags, aclrtStream stream)
{
    aclrtMemManagedLocation prefetchLocs[1] = {prefetchLoc};
    size_t prefetchLocIdxs[1] = {0};
    return aclrtMemManagedPrefetchBatchAsync(reinterpret_cast<const void **>(ptrs), sizes, count,
                                                prefetchLocs, prefetchLocIdxs, 1, flags, stream);
}

template <typename T>
static inline aclError aclrtMemManagedPrefetchBatchAsync(const T **ptrs, size_t *sizes, size_t count,
                                                         aclrtMemManagedLocation *prefetchLocs,
                                                         size_t *prefetchLocIdxs, size_t numPrefetchLocs,
                                                         uint64_t flags, aclrtStream stream)
{
    return aclrtMemManagedPrefetchBatchAsync(reinterpret_cast<const void **>(ptrs), sizes, count,
                                                prefetchLocs, prefetchLocIdxs, numPrefetchLocs, flags, stream);
}

#endif // __cplusplus

#endif // INC_EXTERNAL_ACL_ACL_RT_API_H_
