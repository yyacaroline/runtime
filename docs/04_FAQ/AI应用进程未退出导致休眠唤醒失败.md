# AI应用进程未退出，导致休眠唤醒失败

## 问题现象描述

休眠失败。

查看应用类日志，系统内部的任务分发模块hwts正处于busy状态，检查发现不满足休眠条件，日志片段示例如下：

```
[ERROR] TSCH(-1,null):2023-01-01-02:53:45.850.781 1 (dieid:0,cpuid:0) device_management_plat.c:563 suspend_ack: suspend pre check fail, hwts is busy
[EVENT] TSCH(-1,null):2023-01-01-02:53:45.850.803 2 (dieid:0,cpuid:0) device_management.c:411 process_low_power_cmd: ts suspend ack ret=1.
```

## 原因分析

根据休眠唤醒的流程，休眠前AI应用进程必须先退出，相关硬件资源处于idle态，才允许休眠。不满足休眠条件，会有相关报错，本案例中因为AI应用进程未退出，在休眠唤醒时检测到hwts处于busy状态，因此休眠失败。

## 解决办法

用户需要确保AI应用进程已经运行结束或者优雅退出，推荐使用**kill -2  _PID_**退出相关进程，**_PID_**需替换为实际进程ID。
