# 低版本内核使用asan导致算子执行失败

## 问题现象描述

执行算子时，算子输入数据正确，但输出数据异常，全为0，Host侧plog日志中的报错示例如下：

```
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.036.721 [stars_engine.cc:1321]2082291 ProcLogicCqReport:[INIT][DEFAULT]Task run failed, device_id=0, stream_id=2, task_id=1, sqe_type=0(ffts), errType=0x1(task exception), sqSwStatus=0
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.049.079 [device_error_proc.cc:1218]2082291 ProcessStarsCoreErrorInfo:[INIT][DEFAULT]report error module_type=5, module_name=EZ9999
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.049.115 [device_error_proc.cc:1218]2082291 ProcessStarsCoreErrorInfo:[INIT][DEFAULT]The error from device(chipId:3, dieId:0), serial number is 20, there is an aivec error exception, core id is 4, error code = 0, dump info: pc start: 0x12c0c001406c, current: 0x12c0c00140fc, vec error info: 0x600ed4063d, mte error info: 0x8d0600008c, ifu error info: 0x70f016e068500, ccu error info: 0x28000037, cube error info: 0, biu error info: 0, aic error mask: 0x6500020bd00028c, para base: 0x12c0803e5000.
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.049.300 [device_error_proc.cc:1230]2082291 ProcessStarsCoreErrorInfo:[INIT][DEFAULT]report error module_type=5, module_name=EZ9999
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.049.321 [device_error_proc.cc:1230]2082291 ProcessStarsCoreErrorInfo:[INIT][DEFAULT]The extend info: errcode:(0, 0x200000000000000, 0) errorStr: The MPU address access is invalid. fixp_error0 info: 0x600008c, fixp_error1 info: 0x8d fsmId:1, tslot:3, thread:0, ctxid:0, blk:0, sublk:0, subErrType:4.
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.049.519 [stream.cc:3084]2082291 EnterFailureAbort:[INIT][DEFAULT]stream_id=2 enter failure abort.
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.049.558 [davinic_kernel_task.cc:1321]2082291 SetStarsResultForDavinciTask:[INIT][DEFAULT]AIV Kernel happen error, retCode=0x31.
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.050.340 [davinic_kernel_task.cc:1219]2082291 PreCheckTaskErr:[INIT][DEFAULT]report error module_type=5, module_name=EZ9999
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.050.365 [davinic_kernel_task.cc:1219]2082291 PreCheckTaskErr:[INIT][DEFAULT]Kernel task happen error, retCode=0x31, [vector core exception].
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.050.474 [stream.cc:1079]2082291 GetError:[INIT][DEFAULT]Stream Synchronize failed, stream_id=2, retCode=0x31, [vector core exception].
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.050.496 [stream.cc:1082]2082291 GetError:[INIT][DEFAULT]report error module_type=5, module_name=EZ9999
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.050.517 [stream.cc:1082]2082291 GetError:[INIT][DEFAULT]AIV Kernel happen error, retCode=0x31.
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.050.941 [davinic_kernel_task.cc:1143]2082291 PrintErrorInfoForDavinciTask:[INIT][DEFAULT]Aicore kernel execute failed, device_id=0, stream_id=2, report_stream_id=2, task_id=1, flip_num=0, fault kernel_name=Add_ee98c6628030785f610b924ab1557b31_high_performance_210000000, fault kernel info ext=none, program id=0, hash=3838710036602041089.
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.051.013 [davinic_kernel_task.cc:1082]2082291 GetArgsInfo:[INIT][DEFAULT][AIC_INFO] args(0 to 9) after execute:0, 0, 0, 0, 0, 0, 0, 0, 0,  
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.051.046 [davinic_kernel_task.cc:1085]2082291 GetArgsInfo:[INIT][DEFAULT]tilingKey = 210000000, print 1 Times totalLen=(9*8)Bytes, argsSize=72, blockDim=1
[ERROR] RUNTIME(2082291,python3):2024-07-04-14:14:25.051.088 [davinic_kernel_task.cc:1147]2082291 PrintErrorInfoForDavinciTask:[INIT][DEFAULT][AIC_INFO] after execute:args print end
```

## 可能原因

用户程序编译选项中启动了地址消毒（-lasan），但低版本内核（5.10以下版本，不含5.10）不支持使用asan工具，导致执行算子时拷贝输出数据异常。

可使用**uname -r**命令查看内核版本。

## 处理步骤

-   解决方法1：升级内核版本到5.10或更高版本。
-   解决方法2：在用户程序编译选项中去掉地址消毒（-lasan）。
