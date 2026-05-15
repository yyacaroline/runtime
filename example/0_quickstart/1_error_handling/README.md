# 1_error_handling

## 描述

本示例演示 Runtime 错误处理的基础模式，参考 CUDA `checkCudaErrors` 的写法，展示如何统一检查 ACL 返回值，并组合使用 `aclrtPeekAtLastError`、`aclrtGetLastError`、`aclGetRecentErrMsg` 与 `aclrtGetErrorVerbose` 获取更完整的诊断信息。

## 产品支持情况

本样例支持以下产品：

| 产品 | 是否支持 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

## 编译运行

环境安装详情以及通用运行步骤请见 example 目录下的 [README](../../README.md)。

## CANN RUNTIME API

在该Sample中，涉及的关键功能点及其关键接口，如下所示：

- 初始化
    - 调用 `aclInit` 接口进行初始化配置。
    - 调用 `aclFinalize` 接口实现去初始化。
- Device 管理
    - 调用 `aclrtSetDevice` 接口指定用于运算的 Device。
    - 调用 `aclrtResetDeviceForce` 接口强制复位当前运算的 Device，回收 Device 上的资源。
    - 调用 `aclrtGetRunMode` 接口查询当前运行模式。
- 错误诊断
    - 调用 `aclrtPeekAtLastError` 接口查询线程级 Runtime 错误码而不清空状态。
    - 调用 `aclrtGetLastError` 接口获取并清空线程级 Runtime 错误码。
    - 调用 `aclGetRecentErrMsg` 接口获取最近一次 ACL 调用失败的错误描述。
    - 调用 `aclrtGetErrorVerbose` 接口查询详细的错误摘要信息。该接口为预留接口，当前环境可能返回非成功错误码，样例会记录 WARN 并继续完成错误诊断流程。

## 示例输出

```text
[INFO]  ACL init and set device successfully
[INFO]  Current run mode: ACL_HOST
[INFO]  Triggering an expected invalid-parameter error with aclrtGetRunMode(nullptr)
[ERROR] Diagnostics: ret=..., peekErr=..., lastErr=..., recentErrMsg=...
[INFO]  Verbose error info: errorType=..., tryRepair=..., hasDetail=...
[INFO]  After diagnostics are consumed once: peekErr=0, lastErr=0, recentErrMsg=<null>
```
