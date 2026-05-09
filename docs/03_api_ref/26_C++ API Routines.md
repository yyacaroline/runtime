# 26. C++ API Routines

本章节描述 C++ 扩展接口，包括函数重载和模板封装，用于简化 C++ 场景下的 API 调用并提供类型安全的内存操作。

> **说明：**
> - 本章节接口仅适用于 C++ 程序，需在编译时启用 `__cplusplus` 宏。
> - 头文件：`include/external/acl/acl_rt_api.h`
> - 本章节接口是对 `acl_rt.h` 纯 C API 的 C++ 封装，内部调用底层 C API。

## 函数重载

- [`aclError aclrtSynchronizeDevice(int32_t timeout)`](#aclrtSynchronizeDevice)：设备同步，简化接口。
- [`aclError aclrtSynchronizeStream(aclrtStream stream, int32_t timeout)`](#aclrtSynchronizeStream)：流同步，简化接口。
- [`aclError aclrtSynchronizeEvent(aclrtEvent event, int32_t timeout)`](#aclrtSynchronizeEvent)：事件同步，简化接口。
- [`aclError aclrtStreamWaitEvent(aclrtStream stream, aclrtEvent event, int32_t timeout)`](#aclrtStreamWaitEvent)：流等待事件，简化接口。
- [`aclError aclrtCreateStream(aclrtStream *stream, uint32_t priority, uint32_t flag)`](#aclrtCreateStream)：创建流，简化接口。
- [`aclError aclrtSetOpExecuteTimeOut(uint64_t timeout, uint64_t *actualTimeout)`](#aclrtSetOpExecuteTimeOut)：设置算子执行超时，简化接口。
- [`aclError aclrtCreateEvent(aclrtEvent *event, uint32_t flag)`](#aclrtCreateEvent)：创建事件，简化接口。

## 模板函数

- [`template <typename T> aclError aclrtMalloc(T **devPtr, size_t size, aclrtMallocConfig *cfg = nullptr)`](#aclrtMalloc)：类型安全的设备内存分配。
- [`template <typename T> aclError aclrtMallocHost(T **hostPtr, size_t size, aclrtMallocConfig *cfg = nullptr)`](#aclrtMallocHost)：类型安全的主机内存分配。
- [`template <typename T, typename U> aclError aclrtMemcpy(T *dst, size_t destMax, const U *src, size_t count, aclrtMemcpyKind kind)`](#aclrtMemcpy)：类型安全的内存拷贝。
- [`template <typename T, typename U> aclError aclrtMemcpyAsync(T *dst, size_t destMax, const U *src, size_t count, aclrtMemcpyKind kind, aclrtStream stream)`](#aclrtMemcpyAsync)：异步内存拷贝。
- [`template <typename T, typename U> aclError aclrtMemcpy2d(T *dst, size_t dpitch, const U *src, size_t spitch, size_t width, size_t height, aclrtMemcpyKind kind)`](#aclrtMemcpy2d)：2D内存拷贝。
- [`template <typename T, typename U> aclError aclrtMemcpy2dAsync(T *dst, size_t dpitch, const U *src, size_t spitch, size_t width, size_t height, aclrtMemcpyKind kind, aclrtStream stream)`](#aclrtMemcpy2dAsync)：异步2D内存拷贝。
- [`template <typename T, typename U> aclError aclrtMemcpyBatch(...)`](#aclrtMemcpyBatch)：批量内存拷贝。
- [`template <typename T, typename U> aclError aclrtMemcpyBatchAsync(...)`](#aclrtMemcpyBatchAsync)：异步批量内存拷贝。
- [`template <typename T> aclError aclrtPointerGetAttributes(const T *ptr, aclrtPtrAttributes *attributes)`](#aclrtPointerGetAttributes)：类型安全的指针属性查询。
- [`template <typename T> aclError aclrtHostRegister(...)`](#aclrtHostRegister)：类型安全的主机内存注册。
- [`template <typename T> aclError aclrtHostGetDevicePointer(T *pHost, T **pDevice, uint32_t flag)`](#aclrtHostGetDevicePointer)：主机内存到设备指针映射。
- [`template <typename T> aclError aclrtHostUnregister(T *ptr)`](#aclrtHostUnregister)：类型安全的主机内存注销。
- [`template <typename T> aclError aclrtMemAllocManaged(T **devPtr, size_t size, uint32_t flags = ACL_RT_MEM_ATTACH_GLOBAL)`](#aclrtMemAllocManaged)：类型安全的统一内存分配。
- [`template <typename T> aclError aclrtMemManagedPrefetchAsync(const T *ptr, size_t size, aclrtMemManagedLocation location, uint32_t flags, aclrtStream stream)`](#aclrtMemManagedPrefetchAsync)：预取统一内存。
- [`template <typename T> aclError aclrtMemManagedPrefetchBatchAsync(...)`](#aclrtMemManagedPrefetchBatchAsync)：批量预取统一内存。

---

<br>
<br>
<br>

<a id="aclrtSynchronizeDevice"></a>

## aclrtSynchronizeDevice

```cpp
aclError aclrtSynchronizeDevice(int32_t timeout)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

设备同步，阻塞当前主机线程直到Device上所有显式或隐式创建的Stream都完成所有先前下发的任务。本接口为简化版本，内部调用 [aclrtSynchronizeDeviceWithTimeout](04_Device管理.md#aclrtSynchronizeDeviceWithTimeout)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| timeout | 输入 | 同步等待的超时时间，单位ms。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtSynchronizeStream"></a>

## aclrtSynchronizeStream

```cpp
aclError aclrtSynchronizeStream(aclrtStream stream, int32_t timeout)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

流同步接口的简化重载版本。内部调用 [aclrtSynchronizeStreamWithTimeout](06_Stream管理.md#aclrtSynchronizeStreamWithTimeout)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| stream | 输入 | 指定Stream指针。 |
| timeout | 输入 | 同步等待超时时间，单位ms。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtSynchronizeEvent"></a>

## aclrtSynchronizeEvent

```cpp
aclError aclrtSynchronizeEvent(aclrtEvent event, int32_t timeout)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

事件同步接口的简化重载版本。内部调用 [aclrtSynchronizeEventWithTimeout](07_Event管理.md#aclrtSynchronizeEventWithTimeout)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| event | 输入 | 指定Event指针。 |
| timeout | 输入 | 同步等待超时时间，单位ms。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtStreamWaitEvent"></a>

## aclrtStreamWaitEvent

```cpp
aclError aclrtStreamWaitEvent(aclrtStream stream, aclrtEvent event, int32_t timeout)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

流等待事件接口的简化重载版本。内部调用 [aclrtStreamWaitEventWithTimeout](06_Stream管理.md#aclrtStreamWaitEventWithTimeout)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| stream | 输入 | 指定Stream指针。 |
| event | 输入 | 指定Event指针。 |
| timeout | 输入 | 等待超时时间，单位ms。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   本接口为异步接口，下发等待任务后立即返回，需确保Event已完成。

---

<br>
<br>
<br>

<a id="aclrtCreateStream"></a>

## aclrtCreateStream

```cpp
aclError aclrtCreateStream(aclrtStream *stream, uint32_t priority, uint32_t flag)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

创建Stream接口的简化重载版本。内部调用 [aclrtCreateStreamWithConfig](06_Stream管理.md#aclrtCreateStreamWithConfig)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| stream | 输出 | 创建的Stream指针。 |
| priority | 输入 | Stream优先级。 |
| flag | 输入 | Stream配置标志。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtSetOpExecuteTimeOut"></a>

## aclrtSetOpExecuteTimeOut

```cpp
aclError aclrtSetOpExecuteTimeOut(uint64_t timeout, uint64_t *actualTimeout)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

设置算子执行超时时间的简化重载版本。内部调用 [aclrtSetOpExecuteTimeOutV2](03_运行时配置.md#aclrtSetOpExecuteTimeOutV2)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| timeout | 输入 | 设置的超时时间，单位ms。 |
| actualTimeout | 输出 | 返回实际生效的超时时间。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtCreateEvent"></a>

## aclrtCreateEvent

```cpp
aclError aclrtCreateEvent(aclrtEvent *event, uint32_t flag)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

创建Event接口的简化重载版本。内部调用 [aclrtCreateEventExWithFlag](07_Event管理.md#aclrtCreateEventExWithFlag)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| event | 输出 | 创建的Event指针。 |
| flag | 输入 | Event配置标志。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtMemcpyBatch"></a>

## aclrtMemcpyBatch

```cpp
template <typename T, typename U>
aclError aclrtMemcpyBatch(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes, size_t numBatches, aclrtMemcpyBatchAttr attr)

template <typename T, typename U>
aclError aclrtMemcpyBatch(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes, size_t numBatches, aclrtMemcpyBatchAttr *attrs, size_t *attrsIndexes, size_t numAttrs)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的批量内存拷贝模板函数。内部调用 [aclrtMemcpyBatchV2](11-03_内存拷贝与设置.md#aclrtMemcpyBatchV2)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| dsts | 输出 | 目标内存地址指针数组。 |
| destMaxs | 输入 | 目标内存最大长度数组，单位Byte。 |
| srcs | 输入 | 源内存地址指针数组。 |
| sizes | 输入 | 拷贝内存大小数组，单位Byte。 |
| numBatches | 输入 | 批量拷贝数量。 |
| attr | 输入 | 单个拷贝属性。 |
| attrs | 输入 | 拷贝属性数组。 |
| attrsIndexes | 输入 | 属性索引数组。 |
| numAttrs | 输入 | 属性数量。 |

### 返回值说明

返回0表示成功，返回其他值表示失败。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtMemcpyBatchAsync"></a>

## aclrtMemcpyBatchAsync

```cpp
template <typename T, typename U>
aclError aclrtMemcpyBatchAsync(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes, size_t numBatches, aclrtMemcpyBatchAttr attr, aclrtStream stream)

template <typename T, typename U>
aclError aclrtMemcpyBatchAsync(T **dsts, size_t *destMaxs, U **srcs, size_t *sizes, size_t numBatches, aclrtMemcpyBatchAttr *attrs, size_t *attrsIndexes, size_t numAttrs, aclrtStream stream)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的异步批量内存拷贝模板函数。内部调用 [aclrtMemcpyBatchAsyncV2](11-03_内存拷贝与设置.md#aclrtMemcpyBatchAsyncV2)。

### 参数说明

参数同 [aclrtMemcpyBatch](#aclrtMemcpyBatch)，额外增加 stream 参数。

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| stream | 输入 | 指定Stream指针。 |

### 返回值说明

返回0表示成功，返回其他值表示失败。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   本接口为异步接口，调用后需同步等待。

---

<br>
<br>
<br>

<a id="aclrtPointerGetAttributes"></a>

## aclrtPointerGetAttributes

```cpp
template <typename T>
aclError aclrtPointerGetAttributes(const T *ptr, aclrtPtrAttributes *attributes)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的指针属性查询模板函数。内部调用 [aclrtPointerGetAttributes](11-05_统一寻址.md#aclrtPointerGetAttributes)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| ptr | 输入 | 指定查询的指针地址。 |
| attributes | 输出 | 返回指针的属性信息。 |

### 返回值说明

返回0表示成功，返回其他值表示失败。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtHostRegister"></a>

## aclrtHostRegister

```cpp
template <typename T>
aclError aclrtHostRegister(T *ptr, uint64_t size, aclrtHostRegisterType type, T **devPtr)

template <typename T>
aclError aclrtHostRegister(T *ptr, uint64_t size, uint32_t flag)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的主机内存注册模板函数。内部调用 [aclrtHostRegister](11-02_主机内存管理.md#aclrtHostRegister) 或 [aclrtHostRegisterV2](11-02_主机内存管理.md#aclrtHostRegisterV2)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| ptr | 输入 | Host内存指针。 |
| size | 输入 | 注册内存大小，单位Byte。 |
| type | 输入 | 注册类型。 |
| devPtr | 输出 | 返回Device指针地址。 |
| flag | 输入 | 注册标志位。 |

### 返回值说明

返回0表示成功，返回其他值表示失败。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   注册的内存需通过 [aclrtHostUnregister](#aclrtHostUnregister) 注销。

---

<br>
<br>
<br>

<a id="aclrtHostGetDevicePointer"></a>

## aclrtHostGetDevicePointer

```cpp
template <typename T>
aclError aclrtHostGetDevicePointer(T *pHost, T **pDevice, uint32_t flag)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的主机内存到设备指针映射模板函数。内部调用 [aclrtHostGetDevicePointer](11-05_统一寻址.md#aclrtHostGetDevicePointer)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| pHost | 输入 | Host内存指针。 |
| pDevice | 输出 | 返回Device指针地址。 |
| flag | 输入 | 预留参数，配置为0。 |

### 返回值说明

返回0表示成功，返回其他值表示失败。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtHostUnregister"></a>

## aclrtHostUnregister

```cpp
template <typename T>
aclError aclrtHostUnregister(T *ptr)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的主机内存注销模板函数。内部调用 [aclrtHostUnregister](11-02_主机内存管理.md#aclrtHostUnregister)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| ptr | 输入 | Host内存指针。 |

### 返回值说明

返回0表示成功，返回其他值表示失败。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtMemAllocManaged"></a>

## aclrtMemAllocManaged

```cpp
template <typename T>
aclError aclrtMemAllocManaged(T **devPtr, size_t size, uint32_t flags = ACL_RT_MEM_ATTACH_GLOBAL)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的统一内存分配模板函数。内部调用 [aclrtMemAllocManaged](11-05_统一寻址.md#aclrtMemAllocManaged)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| devPtr | 输出 | 返回统一内存指针。 |
| size | 输入 | 申请内存大小，单位Byte。 |
| flags | 输入 | 内存附加标志，默认ACL_RT_MEM_ATTACH_GLOBAL。 |

### 返回值说明

返回0表示成功，返回其他值表示失败。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   申请的内存需通过 [aclrtFree](11-01_设备内存分配与释放.md#aclrtFree) 释放。

---

<br>
<br>
<br>

<a id="aclrtMemManagedPrefetchAsync"></a>

## aclrtMemManagedPrefetchAsync

```cpp
template <typename T>
aclError aclrtMemManagedPrefetchAsync(const T *ptr, size_t size, aclrtMemManagedLocation location, uint32_t flags, aclrtStream stream)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的统一内存预取模板函数。内部调用 [aclrtMemManagedPrefetchAsync](11-05_统一寻址.md#aclrtMemManagedPrefetchAsync)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| ptr | 输入 | 统一内存指针。 |
| size | 输入 | 预取内存大小，单位Byte。 |
| location | 输入 | 预取目标位置。 |
| flags | 输入 | 预取附加标志。 |
| stream | 输入 | 指定Stream指针。 |

### 返回值说明

返回0表示成功，返回其他值表示失败。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   本接口为异步接口，调用后需同步等待。

---

<br>
<br>
<br>

<a id="aclrtMemManagedPrefetchBatchAsync"></a>

## aclrtMemManagedPrefetchBatchAsync

```cpp
template <typename T>
aclError aclrtMemManagedPrefetchBatchAsync(const T **ptrs, size_t *sizes, size_t count, aclrtMemManagedLocation prefetchLoc, uint64_t flags, aclrtStream stream)

template <typename T>
aclError aclrtMemManagedPrefetchBatchAsync(const T **ptrs, size_t *sizes, size_t count, aclrtMemManagedLocation *prefetchLocs, size_t *prefetchLocIdxs, size_t numPrefetchLocs, uint64_t flags, aclrtStream stream)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的批量统一内存预取模板函数。内部调用 [aclrtMemManagedPrefetchBatchAsync](11-05_统一寻址.md#aclrtMemManagedPrefetchBatchAsync)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| ptrs | 输入 | 统一内存指针数组。 |
| sizes | 输入 | 预取内存大小数组，单位Byte。 |
| count | 输入 | 批量预取数量。 |
| prefetchLoc | 输入 | 单个预取目标位置。 |
| prefetchLocs | 输入 | 预取目标位置数组。 |
| prefetchLocIdxs | 输入 | 预取位置索引数组。 |
| numPrefetchLocs | 输入 | 预取位置数量。 |
| flags | 输入 | 预取附加标志。 |
| stream | 输入 | 指定Stream指针。 |

### 返回值说明

返回0表示成功，返回其他值表示失败。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   本接口为异步接口，调用后需同步等待。

---

<br>
<br>
<br>

<a id="aclrtMalloc"></a>

## aclrtMalloc

```cpp
template <typename T>
aclError aclrtMalloc(T **devPtr, size_t size, aclrtMallocConfig *cfg = nullptr)

template <typename T>
aclError aclrtMalloc(T **devPtr, size_t size, aclrtMemMallocPolicy policy, aclrtMallocConfig *cfg = nullptr)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的设备内存分配模板函数。内部调用 [aclrtMallocWithCfg](11-01_设备内存分配与释放.md#aclrtMallocWithCfg)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| devPtr | 输出 | 返回设备内存指针。类型自动推导。 |
| size | 输入 | 申请内存大小，单位Byte。 |
| cfg | 输入 | 内存分配配置，默认nullptr。 |
| policy | 输入 | 内存分配策略。类型定义请参见[aclrtMemMallocPolicy](25_数据类型及其操作接口.md#aclrtMemMallocPolicy)。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   申请的内存需通过 [aclrtFree](11-01_设备内存分配与释放.md#aclrtFree) 释放。

---

<br>
<br>
<br>

<a id="aclrtMallocHost"></a>

## aclrtMallocHost

```cpp
template <typename T>
aclError aclrtMallocHost(T **hostPtr, size_t size, aclrtMallocConfig *cfg = nullptr)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的主机内存分配模板函数。内部调用 [aclrtMallocHostWithCfg](11-02_主机内存管理.md#aclrtMallocHostWithCfg)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| hostPtr | 输出 | 返回主机内存指针。类型自动推导。 |
| size | 输入 | 申请内存大小，单位Byte。 |
| cfg | 输入 | 内存分配配置，默认nullptr。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   申请的内存需通过 [aclrtFreeHost](11-02_主机内存管理.md#aclrtFreeHost) 释放。

---

<br>
<br>
<br>

<a id="aclrtMemcpy"></a>

## aclrtMemcpy

```cpp
template <typename T, typename U>
aclError aclrtMemcpy(T *dst, size_t destMax, const U *src, size_t count, aclrtMemcpyKind kind)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的内存拷贝模板函数。内部调用 [aclrtMemcpy](11-03_内存拷贝与设置.md#aclrtMemcpy)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| dst | 输出 | 目标内存地址指针。类型自动推导。 |
| destMax | 输入 | 目标内存最大长度，单位Byte。 |
| src | 输入 | 源内存地址指针。类型自动推导。 |
| count | 输入 | 拷贝内存大小，单位Byte。 |
| kind | 输入 | 拷贝类型。类型定义请参见[aclrtMemcpyKind](25_数据类型及其操作接口.md#aclrtMemcpyKind)。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtMemcpyAsync"></a>

## aclrtMemcpyAsync

```cpp
template <typename T, typename U>
aclError aclrtMemcpyAsync(T *dst, size_t destMax, const U *src, size_t count, aclrtMemcpyKind kind, aclrtStream stream)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的异步内存拷贝模板函数。内部调用 [aclrtMemcpyAsync](11-03_内存拷贝与设置.md#aclrtMemcpyAsync)。

### 参数说明

参数同 [aclrtMemcpy](#aclrtMemcpy)，额外增加 stream 参数。

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| stream | 输入 | 指定Stream指针。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   本接口为异步接口，调用后需同步等待拷贝完成。

---

<br>
<br>
<br>

<a id="aclrtMemcpy2d"></a>

## aclrtMemcpy2d

```cpp
template <typename T, typename U>
aclError aclrtMemcpy2d(T *dst, size_t dpitch, const U *src, size_t spitch, size_t width, size_t height, aclrtMemcpyKind kind)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的2D内存拷贝模板函数。将src指向的2D内存数据拷贝到dst指向的2D内存。本接口内部调用 [aclrtMemcpy2d](11-03_内存拷贝与设置.md#aclrtMemcpy2d)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| dst | 输出 | 目标内存地址指针。类型自动推导。 |
| dpitch | 输入 | 目标内存的宽度pitch，单位Byte。 |
| src | 输入 | 源内存地址指针。类型自动推导。 |
| spitch | 输入 | 源内存的宽度pitch，单位Byte。 |
| width | 输入 | 拷贝内存的宽度，单位Byte。 |
| height | 输入 | 拷贝内存的高度，单位Byte。 |
| kind | 输入 | 拷贝类型。类型定义请参见[aclrtMemcpyKind](25_数据类型及其操作接口.md#aclrtMemcpyKind)。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。

---

<br>
<br>
<br>

<a id="aclrtMemcpy2dAsync"></a>

## aclrtMemcpy2dAsync

```cpp
template <typename T, typename U>
aclError aclrtMemcpy2dAsync(T *dst, size_t dpitch, const U *src, size_t spitch, size_t width, size_t height, aclrtMemcpyKind kind, aclrtStream stream)
```

### 产品支持情况

| 产品 | 是否支持 |
| --- | :---: |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

### 功能说明

类型安全的异步2D内存拷贝模板函数。将src指向的2D内存数据异步拷贝到dst指向的2D内存。本接口内部调用 [aclrtMemcpy2dAsync](11-03_内存拷贝与设置.md#aclrtMemcpy2dAsync)。

### 参数说明

| 参数名 | 输入/输出 | 说明 |
| --- | :---: | --- |
| dst | 输出 | 目标内存地址指针。类型自动推导。 |
| dpitch | 输入 | 目标内存的宽度pitch，单位Byte。 |
| src | 输入 | 源内存地址指针。类型自动推导。 |
| spitch | 输入 | 源内存的宽度pitch，单位Byte。 |
| width | 输入 | 拷贝内存的宽度，单位Byte。 |
| height | 输入 | 拷贝内存的高度，单位Byte。 |
| kind | 输入 | 拷贝类型。类型定义请参见[aclrtMemcpyKind](25_数据类型及其操作接口.md#aclrtMemcpyKind)。 |
| stream | 输入 | 指定Stream指针。 |

### 返回值说明

返回0表示成功，返回其他值表示失败，请参见[aclError](25_数据类型及其操作接口.md#aclError)。

### 约束说明

-   本接口仅适用于 C++ 程序。
-   本接口为异步接口，调用后需同步等待拷贝完成。