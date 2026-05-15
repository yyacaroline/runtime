# 2_device_P2P

## 描述
本样例展示两个 Device 之间的 P2P 数据传输流程。样例先查询 Device 0 与 Device 1 是否支持数据交互，能力可用时分别使能双向 P2P 访问，再完成 Device 间内存复制和结果校验。

## 产品支持情况

本样例支持以下产品：

| 产品 | 是否支持 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | √ |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | √ |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | √ |

## 编译运行

本样例至少需要 2 个可用 Device，且 Device 0 与 Device 1 之间需支持 P2P 数据交互。

1.下载样例代码至安装CANN软件的环境，切换到样例目录。
```bash
cd ${git_clone_path}/example/1_basic_features/device/2_device_P2P
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
    - 调用 `aclrtDeviceCanAccessPeer` 接口查询 Device 之间是否支持数据交互；若查询结果为不支持，样例会打印错误日志并结束。
    - 调用 `aclrtDeviceEnablePeerAccess` 接口使能当前 Device 与指定 Device 之间的数据交互。
    - 调用 `aclrtDeviceDisablePeerAccess` 接口关闭当前 Device 与指定 Device 之间的数据交互。
    - 调用 `aclrtSynchronizeDevice` 接口阻塞等待当前 Device 上的任务完成。
    - 调用 `aclrtResetDeviceForce` 接口强制复位当前运算的 Device，回收 Device 上的资源。
- 内存管理
    - 调用 `aclrtMalloc` 接口申请 Device 上的内存。
    - 调用 `aclrtMallocHost` 接口申请 Host 上的内存。
    - 调用 `aclrtMemset` 接口初始化 Host 或 Device 内存。
    - 调用 `aclrtFree` 接口释放 Device 上的内存。
    - 调用 `aclrtFreeHost` 接口释放 Host 上的内存。
- 数据传输
    - 调用 `aclrtMemcpy` 接口通过内存复制的方式实现数据传输。


## 示例输出

```text
[INFO]  Start to run device_P2P sample.
[INFO]  Device 0 to device 1 memcpy success.
[INFO]  Device 1 to host memcpy success.
[INFO]  The data read from the memory is 103, verify success!
[INFO]  Run the device_P2P sample successfully.
```
