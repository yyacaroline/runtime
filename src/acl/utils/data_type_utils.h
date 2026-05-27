/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ACL_UTILS_DATA_TYPE_UTILS_H
#define ACL_UTILS_DATA_TYPE_UTILS_H

#include <unordered_map>
#include <memory>
#include "acl/acl_base.h"
#include "acl/acl_rt.h"
#include "acl/acl_tdt_queue.h"
#include "runtime/base.h"
#include "acl/acl_tdt.h"
#include "tdt/data_common.h"

namespace acl {

inline const char* GetDataTypeDesc(aclDataType type) {
    static const std::unordered_map<aclDataType, const char*> dataTypeDescMap = {
        {ACL_DT_UNDEFINED,  "ACL_DT_UNDEFINED"},
        {ACL_FLOAT,         "ACL_FLOAT"},
        {ACL_FLOAT16,       "ACL_FLOAT16"},
        {ACL_INT8,          "ACL_INT8"},
        {ACL_INT32,         "ACL_INT32"},
        {ACL_UINT8,         "ACL_UINT8"},
        {ACL_INT16,         "ACL_INT16"},
        {ACL_UINT16,        "ACL_UINT16"},
        {ACL_UINT32,        "ACL_UINT32"},
        {ACL_INT64,         "ACL_INT64"},
        {ACL_UINT64,        "ACL_UINT64"},
        {ACL_DOUBLE,        "ACL_DOUBLE"},
        {ACL_BOOL,          "ACL_BOOL"},
        {ACL_STRING,        "ACL_STRING"},
        {ACL_COMPLEX64,     "ACL_COMPLEX64"},
        {ACL_COMPLEX128,    "ACL_COMPLEX128"},
        {ACL_BF16,          "ACL_BF16"},
        {ACL_INT4,          "ACL_INT4"},
        {ACL_UINT1,         "ACL_UINT1"},
        {ACL_COMPLEX32,     "ACL_COMPLEX32"},
        {ACL_HIFLOAT8,      "ACL_HIFLOAT8"},
        {ACL_FLOAT8_E5M2,   "ACL_FLOAT8_E5M2"},
        {ACL_FLOAT8_E4M3FN, "ACL_FLOAT8_E4M3FN"},
        {ACL_FLOAT8_E8M0,   "ACL_FLOAT8_E8M0"},
        {ACL_FLOAT6_E3M2,   "ACL_FLOAT6_E3M2"},
        {ACL_FLOAT6_E2M3,   "ACL_FLOAT6_E2M3"},
        {ACL_FLOAT4_E2M1,   "ACL_FLOAT4_E2M1"},
        {ACL_FLOAT4_E1M2,   "ACL_FLOAT4_E1M2"},
    };

    auto it = dataTypeDescMap.find(type);
    return (it != dataTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetMemcpyKindDesc(aclrtMemcpyKind kind) {
    static const std::unordered_map<aclrtMemcpyKind, const char*> memcpyKindDescMap = {
        {ACL_MEMCPY_HOST_TO_HOST,         "ACL_MEMCPY_HOST_TO_HOST"},
        {ACL_MEMCPY_HOST_TO_DEVICE,       "ACL_MEMCPY_HOST_TO_DEVICE"},
        {ACL_MEMCPY_DEVICE_TO_HOST,       "ACL_MEMCPY_DEVICE_TO_HOST"},
        {ACL_MEMCPY_DEVICE_TO_DEVICE,     "ACL_MEMCPY_DEVICE_TO_DEVICE"},
        {ACL_MEMCPY_DEFAULT,              "ACL_MEMCPY_DEFAULT"},
        {ACL_MEMCPY_HOST_TO_BUF_TO_DEVICE, "ACL_MEMCPY_HOST_TO_BUF_TO_DEVICE"},
    };

    auto it = memcpyKindDescMap.find(kind);
    return (it != memcpyKindDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetExceptionExpandTypeDesc(rtExceptionExpandType_t type) {
    static const std::unordered_map<rtExceptionExpandType_t, const char*> exceptionExpandTypeDescMap = {
        {RT_EXCEPTION_INVALID,   "RT_EXCEPTION_INVALID"},
        {RT_EXCEPTION_FFTS_PLUS, "RT_EXCEPTION_FFTS_PLUS"},
        {RT_EXCEPTION_AICORE,    "RT_EXCEPTION_AICORE"},
        {RT_EXCEPTION_UB,        "RT_EXCEPTION_UB"},
        {RT_EXCEPTION_CCU,       "RT_EXCEPTION_CCU"},
        {RT_EXCEPTION_FUSION,    "RT_EXCEPTION_FUSION"},
    };

    auto it = exceptionExpandTypeDescMap.find(type);
    return (it != exceptionExpandTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetGroupAttrDesc(aclrtGroupAttr attr) {
    static const std::unordered_map<aclrtGroupAttr, const char*> groupAttrDescMap = {
        {ACL_GROUP_AICORE_INT,  "ACL_GROUP_AICORE_INT"},
        {ACL_GROUP_AIV_INT,     "ACL_GROUP_AIV_INT"},
        {ACL_GROUP_AIC_INT,     "ACL_GROUP_AIC_INT"},
        {ACL_GROUP_SDMANUM_INT, "ACL_GROUP_SDMANUM_INT"},
        {ACL_GROUP_ASQNUM_INT,  "ACL_GROUP_ASQNUM_INT"},
        {ACL_GROUP_GROUPID_INT, "ACL_GROUP_GROUPID_INT"},
    };

    auto it = groupAttrDescMap.find(attr);
    return (it != groupAttrDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetTsIdDesc(aclrtTsId tsId) {
    static const std::unordered_map<aclrtTsId, const char*> tsIdDescMap = {
        {ACL_TS_ID_AICORE,   "ACL_TS_ID_AICORE"},
        {ACL_TS_ID_AIVECTOR, "ACL_TS_ID_AIVECTOR"},
        {ACL_TS_ID_RESERVED, "ACL_TS_ID_RESERVED"},
    };

    auto it = tsIdDescMap.find(tsId);
    return (it != tsIdDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetQueueRouteQueryModeDesc(acltdtQueueRouteQueryMode mode) {
    static const std::unordered_map<acltdtQueueRouteQueryMode, const char*> queueRouteQueryModeDescMap = {
        {ACL_TDT_QUEUE_ROUTE_QUERY_SRC,       "ACL_TDT_QUEUE_ROUTE_QUERY_SRC"},
        {ACL_TDT_QUEUE_ROUTE_QUERY_DST,       "ACL_TDT_QUEUE_ROUTE_QUERY_DST"},
        {ACL_TDT_QUEUE_ROUTE_QUERY_SRC_AND_DST, "ACL_TDT_QUEUE_ROUTE_QUERY_SRC_AND_DST"},
        {ACL_TDT_QUEUE_ROUTE_QUERY_ABNORMAL,  "ACL_TDT_QUEUE_ROUTE_QUERY_ABNORMAL"},
    };

    auto it = queueRouteQueryModeDescMap.find(mode);
    return (it != queueRouteQueryModeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetQueueAttrTypeDesc(acltdtQueueAttrType type) {
    static const std::unordered_map<acltdtQueueAttrType, const char*> queueAttrTypeDescMap = {
        {ACL_TDT_QUEUE_NAME_PTR,    "ACL_TDT_QUEUE_NAME_PTR"},
        {ACL_TDT_QUEUE_DEPTH_UINT32, "ACL_TDT_QUEUE_DEPTH_UINT32"},
    };

    auto it = queueAttrTypeDescMap.find(type);
    return (it != queueAttrTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetQueueRouteParamTypeDesc(acltdtQueueRouteParamType type) {
    static const std::unordered_map<acltdtQueueRouteParamType, const char*> queueRouteParamTypeDescMap = {
        {ACL_TDT_QUEUE_ROUTE_SRC_UINT32,   "ACL_TDT_QUEUE_ROUTE_SRC_UINT32"},
        {ACL_TDT_QUEUE_ROUTE_DST_UINT32,   "ACL_TDT_QUEUE_ROUTE_DST_UINT32"},
        {ACL_TDT_QUEUE_ROUTE_STATUS_INT32, "ACL_TDT_QUEUE_ROUTE_STATUS_INT32"},
    };

    auto it = queueRouteParamTypeDescMap.find(type);
    return (it != queueRouteParamTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetQueueRouteQueryInfoParamTypeDesc(acltdtQueueRouteQueryInfoParamType type) {
    static const std::unordered_map<acltdtQueueRouteQueryInfoParamType, const char*> queueRouteQueryInfoParamTypeDescMap = {
        {ACL_TDT_QUEUE_ROUTE_QUERY_MODE_ENUM,    "ACL_TDT_QUEUE_ROUTE_QUERY_MODE_ENUM"},
        {ACL_TDT_QUEUE_ROUTE_QUERY_SRC_ID_UINT32, "ACL_TDT_QUEUE_ROUTE_QUERY_SRC_ID_UINT32"},
        {ACL_TDT_QUEUE_ROUTE_QUERY_DST_ID_UINT32, "ACL_TDT_QUEUE_ROUTE_QUERY_DST_ID_UINT32"},
    };

    auto it = queueRouteQueryInfoParamTypeDescMap.find(type);
    return (it != queueRouteQueryInfoParamTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetAllocBufTypeDesc(acltdtAllocBufType type) {
    static const std::unordered_map<acltdtAllocBufType, const char*> allocBufTypeDescMap = {
        {ACL_TDT_NORMAL_MEM, "ACL_TDT_NORMAL_MEM"},
        {ACL_TDT_DVPP_MEM,   "ACL_TDT_DVPP_MEM"},
    };

    auto it = allocBufTypeDescMap.find(type);
    return (it != allocBufTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetTensorTypeDesc(acltdtTensorType type) {
    static const std::unordered_map<acltdtTensorType, const char*> tensorTypeDescMap = {
        {ACL_TENSOR_DATA_UNDEFINED,    "ACL_TENSOR_DATA_UNDEFINED"},
        {ACL_TENSOR_DATA_TENSOR,       "ACL_TENSOR_DATA_TENSOR"},
        {ACL_TENSOR_DATA_END_OF_SEQUENCE, "ACL_TENSOR_DATA_END_OF_SEQUENCE"},
        {ACL_TENSOR_DATA_ABNORMAL,     "ACL_TENSOR_DATA_ABNORMAL"},
        {ACL_TENSOR_DATA_SLICE_TENSOR, "ACL_TENSOR_DATA_SLICE_TENSOR"},
        {ACL_TENSOR_DATA_END_TENSOR,   "ACL_TENSOR_DATA_END_TENSOR"},
    };

    auto it = tensorTypeDescMap.find(type);
    return (it != tensorTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetSysParamOptDesc(aclSysParamOpt opt) {
    static const std::unordered_map<aclSysParamOpt, const char*> sysParamOptDescMap = {
        {ACL_OPT_DETERMINISTIC,       "ACL_OPT_DETERMINISTIC"},
        {ACL_OPT_ENABLE_DEBUG_KERNEL, "ACL_OPT_ENABLE_DEBUG_KERNEL"},
        {ACL_OPT_STRONG_CONSISTENCY,  "ACL_OPT_STRONG_CONSISTENCY"},
    };

    auto it = sysParamOptDescMap.find(opt);
    return (it != sysParamOptDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetCallbackBlockTypeDesc(aclrtCallbackBlockType type) {
    static const std::unordered_map<aclrtCallbackBlockType, const char*> callbackBlockTypeDescMap = {
        {ACL_CALLBACK_NO_BLOCK, "ACL_CALLBACK_NO_BLOCK"},
        {ACL_CALLBACK_BLOCK,    "ACL_CALLBACK_BLOCK"},
    };

    auto it = callbackBlockTypeDescMap.find(type);
    return (it != callbackBlockTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetMemLocationTypeDesc(aclrtMemLocationType type) {
    static const std::unordered_map<aclrtMemLocationType, const char*> memLocationTypeDescMap = {
        {ACL_MEM_LOCATION_TYPE_HOST,         "ACL_MEM_LOCATION_TYPE_HOST"},
        {ACL_MEM_LOCATION_TYPE_DEVICE,       "ACL_MEM_LOCATION_TYPE_DEVICE"},
        {ACL_MEM_LOCATION_TYPE_UNREGISTERED, "ACL_MEM_LOCATION_TYPE_UNREGISTERED"},
        {ACL_MEM_LOCATION_TYPE_HOST_NUMA,    "ACL_MEM_LOCATION_TYPE_HOST_NUMA"},
    };

    auto it = memLocationTypeDescMap.find(type);
    return (it != memLocationTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetMemAllocationTypeDesc(aclrtMemAllocationType type) {
    static const std::unordered_map<aclrtMemAllocationType, const char*> memAllocationTypeDescMap = {
        {ACL_MEM_ALLOCATION_TYPE_PINNED, "ACL_MEM_ALLOCATION_TYPE_PINNED"},
    };

    auto it = memAllocationTypeDescMap.find(type);
    return (it != memAllocationTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetTdtDataTypeDesc(tdt::TdtDataType type) {
    static const std::unordered_map<tdt::TdtDataType, const char*> tdtDataTypeDescMap = {
        {tdt::TDT_IMAGE_LABEL,      "TDT_IMAGE_LABEL"},
        {tdt::TDT_TFRECORD,         "TDT_TFRECORD"},
        {tdt::TDT_DATA_LABEL,       "TDT_DATA_LABEL"},
        {tdt::TDT_END_OF_SEQUENCE,  "TDT_END_OF_SEQUENCE"},
        {tdt::TDT_TENSOR,           "TDT_TENSOR"},
        {tdt::TDT_ABNORMAL,         "TDT_ABNORMAL"},
        {tdt::TDT_DATATYPE_MAX,     "TDT_DATATYPE_MAX"},
    };

    auto it = tdtDataTypeDescMap.find(type);
    return (it != tdtDataTypeDescMap.end()) ? it->second : "UNKNOWN";
}

inline const char* GetTdtDataTypeDescV2(int32_t type) {
    static const std::unordered_map<int32_t, const char*> tdtDataTypeDescV2Map = {
        {0, "TDT_TENSOR(0)"},
        {1, "TDT_END_OF_SEQUENCE(1)"},
        {2, "TDT_ABNORMAL(2)"},
        {3, "TDT_SLICE_TENSOR(3)"},
        {4, "TDT_END_TENSOR(4)"},
    };

    auto it = tdtDataTypeDescV2Map.find(type);
    return (it != tdtDataTypeDescV2Map.end()) ? it->second : "UNKNOWN";
}

} // namespace acl

#endif // ACL_UTILS_DATA_TYPE_UTILS_H
