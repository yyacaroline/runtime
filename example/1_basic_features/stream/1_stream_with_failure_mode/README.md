# 1_stream_with_failure_mode

## 描述
本样例展示 Stream 遇错即停模式，并通过核函数任务模拟 Stream 执行过程中发生错误的场景。

本样例包含三部分内容：普通 Stream 上的遇错即停演示、`aclrtStreamStop` 辅助演示、`aclrtStreamAbort` 辅助演示。

## 产品支持情况

本样例支持以下产品：

| 产品 | 是否支持 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

## 编译运行

如果当前环境不满足 `stop/abort` 辅助演示的额外前置条件，样例会打印 `WARN` 并跳过对应辅助演示；主流程中的 `aclrtSetStreamFailureMode` 演示仍然会继续执行。

1.下载样例代码至安装CANN软件的环境，切换到样例目录。
```bash
cd ${git_clone_path}/example/1_basic_features/stream/1_stream_with_failure_mode
```

2.设置环境变量。
```bash
# ${install_root} 替换为 CANN 安装根目录，默认安装在`/usr/local/Ascend`目录
source ${install_root}/cann/set_env.sh
export ASCEND_INSTALL_PATH=${install_root}/cann

# 设置 SOC_VERSION 和 ASCENDC_CMAKE_DIR
# -SOC_VERSION: 昇腾AI处理器的型号，如 Ascend910_9362，Ascend910B2等
# -ASCENDC_CMAKE_DIR: 样例中涉及调用AscendC算子，需配置AscendC编译器 ascendc.cmake 路径，如 /usr/local/Ascend/cann/x86_64-linux/tikcpp/ascendc_kernel_cmake
source ${git_clone_path}/example/set_sample_env.sh
```

3.执行以下命令运行样例。
```bash
bash run.sh
```
## CANN RUNTIME API

在该Sample中，涉及的关键功能点及其关键接口，如下所示：
- 初始化
    - 调用 `aclInit` 接口进行初始化配置。
    - 调用 `aclFinalize` 接口实现去初始化。
- Device 管理
    - 调用 `aclrtSetDevice` 接口指定用于运算的 Device。
    - 调用 `aclrtResetDeviceForce` 接口强制复位当前运算的 Device，回收 Device 上的资源。
- Context 管理
    - 调用 `aclrtCreateContext` 接口创建 Context。
    - 调用 `aclrtDestroyContext` 接口销毁 Context。
- Stream 管理
    - 调用 `aclrtCreateStream` 接口创建 Stream。
    - 调用 `aclrtCreateStreamWithConfig` 接口创建 `ACL_STREAM_DEVICE_USE_ONLY` 类型的 Stream；在 Atlas A2/Atlas A3 上，`aclrtStreamStop` 要求目标 Stream 通过 `aclrtCreateStreamWithConfig(..., ACL_STREAM_DEVICE_USE_ONLY)` 创建。
    - 调用 `aclrtSynchronizeStream` 接口可以阻塞等待 Stream 上任务的完成。
    - 调用 `aclrtSetStreamFailureMode` 接口可以设置 Stream 执行任务遇到错误的操作，默认为遇错继续，可以设置为遇错即停。
    - 调用 `aclrtRegStreamStateCallback` 接口注册 Stream 状态回调。
    - 调用 `aclrtStreamStop` 接口停止辅助 Stream；样例会单独创建一条 device-use-only Stream，并在其上提交长时核任务后调用 `aclrtStreamStop`。
    - 调用 `aclrtStreamAbort` 接口中止辅助 Stream。`aclrtStreamAbort` 不支持 `ACL_STREAM_DEVICE_USE_ONLY` Stream，因此样例会在普通 Stream 上单独演示；演示后再次调用 `aclrtSynchronizeStream` 时，可能返回 stream abort 类状态码，属于预期结果，样例会记录日志而不会将整个流程判定为失败。
    - 调用 `aclrtDestroyStreamForce` 接口强制销毁 Stream，丢弃所有任务。
- 内存管理
    - 调用 `aclrtMalloc` 接口申请 Device 上的内存。
    - 调用 `aclrtFree` 接口释放 Device 上的内存。
- 数据传输
    - 调用 `aclrtMemcpy` 接口通过内存复制的方式实现数据传输。

## 示例输出

```text
[INFO]  Assigning task without failure mode.
[ERROR]  Operation failed: aclrtSynchronizeStream(stream) returned error code 507035
[INFO]  Without failure mode, the result is 2.
[INFO]  Assigning task with failure mode.
[ERROR]  Operation failed: aclrtSynchronizeStream(stream) returned error code 507035
[INFO]  After set failure mode, the current result is: 1.
[INFO]  aclrtStreamStop returned 0.
[WARN]  aclrtSynchronizeStream(after stop) returned 507000.
[INFO]  aclrtStreamAbort returned 0.
[INFO]  aclrtSynchronizeStream(after abort) returned 507035 after stream abort, which is expected.
[INFO]  Resource cleanup completed.
[INFO]  Run the stream_with_failure_mode sample successfully.
```
