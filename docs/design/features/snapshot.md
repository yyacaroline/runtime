# Snapshot 特性

## 1. 特性概述

- **特性介绍**：Snapshot 特性支持 NPU 进程状态保存和恢复，用于进程级快照备份/恢复场景（故障恢复）。通过锁定进程、备份关键状态、恢复资源，实现进程状态的完整保存和恢复。
- **问题背景**：进程在运行过程中需要保存完整状态以便后续恢复。大规模AI集群的故障恢复效率直接影响训练任务的可用性与资源利用率。传统故障恢复链路涉及容器调度、进程重建、主机与设备建链、算子编译、权重加载等多个串行环节，单次恢复耗时可达十余分钟。
- **设计目标**：
  - 支持进程锁定/解锁控制
  - 支持进程状态备份
  - 支持进程状态恢复
  - 提供多阶段回调机制

## 2. 使用场景与对外接口

### 2.1 使用场景

- **场景一**：进程快照备份
  ```cpp
  // 1. 锁定进程
  aclError ret = aclrtSnapShotProcessLock(pid, nullptr);
  // 2. 备份进程状态
  aclrtSnapShotBackupArgs backupArgs;
  ret = aclrtSnapShotProcessBackup(pid, &backupArgs);
  // 3. 在目标端恢复进程状态
  aclrtSnapShotRestoreArgs restoreArgs;
  ret = aclrtSnapShotProcessRestore(pid, &restoreArgs);
  // 4. 解锁进程
  ret = aclrtSnapShotProcessUnlock(pid, nullptr);
  ```

- **场景二**：注册回调快照阶段
  ```cpp
  // 注册备份前回调
  aclrtSnapShotCallBack callback = MyBackupPreCallback;
  aclrtSnapShotCallbackRegister(ACL_RT_SNAPSHOT_BACKUP_PRE, callback, nullptr);
  // 注册恢复后回调
  aclrtSnapShotCallbackRegister(ACL_RT_SNAPSHOT_RESTORE_POST, MyRestorePostCallback, nullptr);
  ```

### 2.2 对外接口

| 接口 | 文件位置 | 说明 |
|------|----------|------|
| `aclrtSnapShotProcessLock()` | `include/external/acl/acl_rt.h` | 锁定进程 |
| `aclrtSnapShotProcessUnlock()` | `include/external/acl/acl_rt.h` | 解锁进程 |
| `aclrtSnapShotProcessBackup()` | `include/external/acl/acl_rt.h` | 备份进程状态 |
| `aclrtSnapShotProcessRestore()` | `include/external/acl/acl_rt.h` | 从备份点恢复进程 |
| `aclrtSnapShotCallbackRegister()` | `include/external/acl/acl_rt.h` | 注册快照阶段回调 |
| `aclrtSnapShotCallbackUnregister()` | `include/external/acl/acl_rt.h` | 注销快照阶段回调 |

### 2.3 快照阶段定义

```cpp
// 文件位置：include/external/acl/acl_rt.h
typedef enum {
    ACL_RT_SNAPSHOT_LOCK_PRE = 0,
    ACL_RT_SNAPSHOT_BACKUP_PRE,
    ACL_RT_SNAPSHOT_BACKUP_POST,
    ACL_RT_SNAPSHOT_RESTORE_PRE,
    ACL_RT_SNAPSHOT_RESTORE_POST,
    ACL_RT_SNAPSHOT_UNLOCK_POST,
} aclrtSnapShotStage;
```

## 3. 架构总览

### 整体设计思路

Snapshot 特性采用分层架构设计：
- **API 层**：提供 C 语言接口
- **核心协调层**：GlobalStateManager 管理进程状态，SnapshotCallbackManager 管理回调
- **备份恢复层**：命名空间级别函数实现备份恢复流程
- **设备操作层**：DeviceSnapshot 实现设备内存备份/恢复，通过 IDeviceSnapshotOps 接口抽象

### 架构分层图

```mermaid
graph TB
subgraph ACL["ACL 接口层"]
ACLSnapshot["aclrtSnapShotProcessLock/Unlock<br/>aclrtSnapShotProcessBackup/Restore<br/>aclrtSnapShotCallbackRegister"]
end
subgraph API["Runtime API 层"]
APISnapshot["rtSnapShotProcessLock/Unlock<br/>rtSnapShotProcessBackup/Restore<br/>rtSnapShotCallbackRegister"]
ApiImpl["ApiImpl::SnapShotProcess*"]
end
subgraph Core["核心管理层"]
GlobalStateManager["GlobalStateManager<br/>进程状态管理"]
SnapshotCallbackManager["SnapshotCallbackManager<br/>回调管理单例"]
end
subgraph Process["备份恢复协调层"]
SnapShotProcessBackup["SnapShotProcessBackup()<br/>进程备份流程"]
SnapShotProcessRestore["SnapShotProcessRestore()<br/>进程恢复流程"]
ModelBackupRestore["ModelBackup/ModelRestore()<br/>模型备份恢复"]
SnapShotPreProcessBackup["SnapShotPreProcessBackup()<br/>备份前处理"]
SnapShotDeviceRestore["SnapShotDeviceRestore()<br/>设备恢复"]
SnapShotResourceRestore["SnapShotResourceRestore()<br/>资源恢复"]
SnapShotAclGraphRestore["SnapShotAclGraphRestore()<br/>ACL Graph恢复"]
end
subgraph Feature["特性实现层"]
IDeviceSnapshotOps["IDeviceSnapshotOps<br/>设备快照操作接口"]
DeviceSnapshot["DeviceSnapshot<br/>内存备份/恢复实现"]
TaskHandlers["TaskHandlers<br/>任务处理器"]
end
ACL --> APISnapshot
APISnapshot --> ApiImpl
ApiImpl --> GlobalStateManager
ApiImpl --> SnapshotCallbackManager
ApiImpl --> SnapShotProcessBackup
ApiImpl --> SnapShotProcessRestore
SnapshotCallbackManager --> GlobalStateManager
SnapShotProcessBackup --> SnapShotPreProcessBackup
SnapShotProcessBackup --> ModelBackupRestore
SnapShotProcessBackup --> Runtime["Runtime::SaveModule"]
SnapShotProcessRestore --> SnapShotDeviceRestore
SnapShotProcessRestore --> SnapShotResourceRestore
SnapShotProcessRestore --> IDeviceSnapshotOps
SnapShotProcessRestore --> ModelBackupRestore
SnapShotProcessRestore --> SnapShotAclGraphRestore
SnapShotProcessRestore --> Runtime2["Runtime::RestoreModule"]
DeviceSnapshot --> IDeviceSnapshotOps
DeviceSnapshot --> TaskHandlers
```

### 核心模块交互图

```mermaid
sequenceDiagram
participant App as 应用层
participant ACL as ACL API
participant RT as Runtime API
participant ApiImpl as ApiImpl
participant SCM as SnapshotCallbackManager
participant GSM as GlobalStateManager
participant Process as SnapShotProcessBackup
participant Dev as Device
participant DS as DeviceSnapshot
App->>ACL: aclrtSnapShotProcessLock()
ACL->>RT: rtSnapShotProcessLock()
RT->>ApiImpl: SnapShotProcessLock()
ApiImpl->>SCM: InvokeCallbacks(RT_SNAPSHOT_LOCK_PRE)
SCM->>SCM: 遍历回调map，对每个设备调用回调
SCM-->>ApiImpl: 回调结果
ApiImpl->>GSM: Locked()
GSM->>GSM: 状态切换 RUNNING->LOCKED
GSM->>GSM: WaitForAllBackgroundThreadLocked()
GSM-->>ApiImpl: 锁定成功
ApiImpl-->>App: 返回结果
App->>ACL: aclrtSnapShotProcessBackup()
ACL->>RT: rtSnapShotProcessBackup()
RT->>ApiImpl: SnapShotProcessBackup()
ApiImpl->>SCM: InvokeCallbacks(RT_SNAPSHOT_BACKUP_PRE)
SCM-->>ApiImpl: 回调结果
ApiImpl->>Process: SnapShotProcessBackup()
Process->>Process: SnapShotPreProcessBackup(同步Context)
Process->>Dev: 遍历所有设备
Process->>Process: ModelBackup(devId)
Process->>Process: SinkTaskMemoryBackup(devId)
Process->>DS: OpMemoryBackup()
Process->>Process: Runtime::SaveModule()
ApiImpl->>SCM: InvokeCallbacks(RT_SNAPSHOT_BACKUP_POST)
ApiImpl->>GSM: SetCurrentState(BACKED_UP)
ApiImpl-->>App: 返回结果
App->>ACL: aclrtSnapShotProcessRestore()
ACL->>RT: rtSnapShotProcessRestore()
RT->>ApiImpl: SnapShotProcessRestore()
ApiImpl->>SCM: InvokeCallbacks(RT_SNAPSHOT_RESTORE_PRE)
ApiImpl->>Process: SnapShotProcessRestore()
Process->>Process: SnapShotDeviceRestore(ReOpen)
Process->>Process: SnapShotResourceRestore(Stream/Event/Notify)
Process->>Dev: 遍历所有设备
Process->>DS: OpMemoryRestore()
Process->>DS: ArgsPoolRestore()
Process->>DS: UbArgsPoolRestore()
Process->>Process: ModelRestore(devId)
Process->>Process: SnapShotAclGraphRestore(dev)
Process->>Process: Runtime::RestoreModule()
ApiImpl->>SCM: InvokeCallbacks(RT_SNAPSHOT_RESTORE_POST)
ApiImpl->>GSM: SetCurrentState(LOCKED)
ApiImpl-->>App: 返回结果
App->>ACL: aclrtSnapShotProcessUnlock()
ACL->>RT: rtSnapShotProcessUnlock()
RT->>ApiImpl: SnapShotProcessUnlock()
ApiImpl->>GSM: Unlocked()
GSM->>GSM: 状态切换 LOCKED->RUNNING
GSM->>GSM: 通知阻塞线程
GSM->>GSM: WaitForAllBackgroundThreadUnlocked()
ApiImpl->>SCM: InvokeCallbacks(RT_SNAPSHOT_UNLOCK_POST)
ApiImpl-->>App: 返回结果
```

## 4. 详细设计

### 4.1 核心流程

#### 进程锁定流程

```mermaid
flowchart TD
A[rtSnapShotProcessLock] --> B[ApiImpl::SnapShotProcessLock]
B --> C[SnapshotCallbackManager::InvokeCallbacks LOCK_PRE]
C --> D{回调是否成功}
D -->|失败| E[返回错误]
D -->|成功| F[GlobalStateManager.Locked]
F --> G[检查当前状态是否为 RUNNING]
G -->|非 RUNNING| H[返回锁定失败]
G -->|RUNNING| I[状态切换为 LOCKED]
I --> J[等待所有后台线程锁定]
J --> K{等待超时}
K -->|超时| L[返回锁定超时错误]
K -->|成功| M[锁定成功]
```

关键文件位置：`src/runtime/core/src/api_impl/api_impl.cc`

#### 进程备份流程

```mermaid
flowchart TD
A[rtSnapShotProcessBackup] --> B[ApiImpl::SnapShotProcessBackup]
B --> C[SnapshotCallbackManager::InvokeCallbacks BACKUP_PRE]
C --> D[cce::runtime::SnapShotProcessBackup]
D --> E[SnapShotPreProcessBackup]
E --> F[同步所有 Context]
F --> G[确保没有任务执行]
G --> H[遍历所有 Device]
H --> I[ModelBackup]
I --> J[SinkTaskMemoryBackup]
J --> K[DeviceSnapshot::OpMemoryBackup]
K --> L[Runtime::SaveModule]
L --> M[NpuDriver::ProcessResBackup]
M --> N[SnapshotCallbackManager::InvokeCallbacks BACKUP_POST]
N --> O[GlobalStateManager::SetCurrentState BACKED_UP]
```

关键文件位置：
- `src/runtime/core/src/api_impl/api_impl.cc`
- `src/runtime/feature/snapshot/snapshot_process_helper.cc`

#### 进程恢复流程

```mermaid
flowchart TD
A[rtSnapShotProcessRestore] --> B[ApiImpl::SnapShotProcessRestore]
B --> C[SnapshotCallbackManager::InvokeCallbacks RESTORE_PRE]
C --> D[cce::runtime::SnapShotProcessRestore]
D --> E[SnapShotDeviceRestore]
E --> F[重新打开所有 Device]
F --> G[恢复页表信息]
G --> H[SnapShotResourceRestore]
H --> I[清理并重新分配 Stream ID]
I --> J[重新分配 Event/Notify ID]
J --> K[Device ResourceRestore]
K --> L[遍历所有 Device]
L --> M[DeviceSnapshot::OpMemoryRestore]
M --> N[ArgsPoolRestore]
N --> O[UbArgsPoolRestore]
O --> P[ModelRestore]
P --> Q[SnapShotAclGraphRestore]
Q --> R[Runtime::RestoreModule]
R --> S[SnapshotCallbackManager::InvokeCallbacks RESTORE_POST]
S --> T[GlobalStateManager::SetCurrentState LOCKED]
```

关键文件位置：
- `src/runtime/core/src/api_impl/api_impl.cc`
- `src/runtime/feature/snapshot/snapshot_process_helper.cc`

### 4.2 核心组件详解

#### GlobalStateManager 进程状态管理

**设计思想**：管理进程全局状态，控制 API 调用阻塞，协调后台线程锁定。位于 `cce::runtime` 命名空间。

```mermaid
classDiagram
class GlobalStateManager {
-mutex stateMtx_
-condition_variable globalLockCv_
-atomic~rtProcessState~ currentState_
-atomic~uint32_t~ backgroundThreadCount_
-atomic~uint32_t~ lockedBackgroundThreadCount_
+static GetInstance() GlobalStateManager&
+Locked() rtError_t
+Unlocked() rtError_t
+ForceUnlocked() void
+WaitIfLocked(name) void
+GetCurrentState() rtProcessState
+SetCurrentState(state) void
+IncBackgroundThreadCount(name) void
+DecBackgroundThreadCount(name) void
+BackgroundThreadWaitIfLocked(name) void
+static StateToString(state) const char*
-WaitForAllBackgroundThreadLocked() rtError_t
-WaitForAllBackgroundThreadUnlocked() rtError_t
}
```

关键文件位置：
- 头文件：`src/runtime/core/inc/common/global_state_manager.hpp`
- 实现文件：`src/runtime/core/src/common/global_state_manager.cc`

#### SnapshotCallbackManager 回调管理器

**设计思想**：独立单例管理快照回调函数，提供注册、注销、触发回调功能。位于 `cce::runtime` 命名空间。

```mermaid
classDiagram
class SnapshotCallbackManager {
-mutex mutex_
-map~rtSnapShotStage,list~SnapShotCallBackInfo~~ callbackMap_
+static GetInstance() SnapshotCallbackManager&
+RegisterCallback(stage, callback, args) rtError_t
+UnregisterCallback(stage, callback) rtError_t
+InvokeCallbacks(stage) rtError_t
-static GetStageString(stage) const char*
}
class SnapShotCallBackInfo {
+rtSnapShotCallBack callback
+void* args
}
SnapshotCallbackManager --> SnapShotCallBackInfo
```

关键文件位置：
- 头文件：`src/runtime/feature/snapshot/snapshot_callback_manager.hpp`
- 实现文件：`src/runtime/feature/snapshot/snapshot_callback_manager.cc`

#### IDeviceSnapshotOps 设备快照操作接口

**设计思想**：抽象接口定义设备快照核心操作，便于扩展和适配不同硬件版本。

```mermaid
classDiagram
class IDeviceSnapshotOps {
<<interface>>
+OpMemoryBackup() rtError_t
+OpMemoryRestore() rtError_t
+ArgsPoolRestore() rtError_t
+UbArgsPoolRestore() rtError_t
}
class DeviceSnapshot {
-vector~pair~void*,size_t~~ opVirtualAddrs_
-unique_ptr~uint8_t[]~ opBackUpAddrs_
-size_t opTotalHostMemSize_
-Device* device_
+AddOpVirtualAddr(addr, size) void
+GetOpVirtualAddrs() vector
+GetOpBackUpAddr() unique_ptr&
+SetOpBackUpAddr(hostAddr) void
+GetOpTotalHostMemSize() size_t
+RecordOpAddrAndSize(stm) void
+GetOpTotalMemoryInfo(mdl) void
+RecordFuncCallAddrAndSize(task) void
+RecordArgsAddrAndSize(task) void
+OpMemoryBackup() rtError_t
+OpMemoryRestore() rtError_t
+ArgsPoolRestore() rtError_t
+UbArgsPoolRestore() rtError_t
+ArgsPoolConvertAddr(mgr) rtError_t
+OpMemoryInfoInit() void
}
IDeviceSnapshotOps <|-- DeviceSnapshot
class TaskHandlers {
+HandleStreamSwitch(task, snapshot) void
+HandleStreamLabelSwitchByIndex(task, snapshot) void
+HandleMemWaitValue(task, snapshot) void
+HandleRdmaPiValueModify(task, snapshot) void
+HandleStreamActive(task, snapshot) void
+HandleModelTaskUpdate(task, snapshot) void
}
DeviceSnapshot --> TaskHandlers
```

关键文件位置：
- 接口定义：`src/runtime/core/inc/common/idevice_snapshot_ops.hpp`
- 实现类头文件：`src/runtime/feature/snapshot/device_snapshot.hpp`
- 实现文件：`src/runtime/feature/snapshot/device_snapshot.cc`

#### SnapShotProcessHelper 快照处理辅助

**设计思想**：提供备份前处理、设备恢复、资源恢复、模型备份恢复、ACL Graph恢复等辅助函数。位于 `cce::runtime` 命名空间。

关键文件位置：
- 头文件：`src/runtime/feature/snapshot/snapshot_process_helper.hpp`
- 实现文件：`src/runtime/feature/snapshot/snapshot_process_helper.cc`

### 4.3 模块职责划分

| 模块 | 职责 | 位置 |
|------|------|------|
| GlobalStateManager | 进程状态管理、API调用阻塞控制 | `core/inc/common/global_state_manager.hpp` |
| SnapshotCallbackManager | 回调管理单例 | `feature/snapshot/snapshot_callback_manager.hpp` |
| IDeviceSnapshotOps | 设备快照操作抽象接口 | `core/inc/common/idevice_snapshot_ops.hpp` |
| DeviceSnapshot | 设备内存备份/恢复实现 | `feature/snapshot/device_snapshot.hpp` |
| SnapShotProcessHelper | 备份恢复辅助函数 | `feature/snapshot/snapshot_process_helper.hpp` |
| ApiImpl | API 实现与流程协调 | `core/src/api_impl/api_impl.cc` |
| TaskHandlers | 任务类型处理器 | `feature/snapshot/device_snapshot.hpp` |

### 4.4 核心数据结构

```mermaid
classDiagram
class rtProcessState {
<<enumeration>>
RT_PROCESS_STATE_RUNNING
RT_PROCESS_STATE_LOCKED
RT_PROCESS_STATE_BACKED_UP
}
class rtSnapShotStage {
<<enumeration>>
RT_SNAPSHOT_LOCK_PRE
RT_SNAPSHOT_BACKUP_PRE
RT_SNAPSHOT_BACKUP_POST
RT_SNAPSHOT_RESTORE_PRE
RT_SNAPSHOT_RESTORE_POST
RT_SNAPSHOT_UNLOCK_POST
}
class SnapShotCallBackInfo {
+rtSnapShotCallBack callback
+void* args
}
class GlobalStateManager {
-mutex stateMtx_
-condition_variable globalLockCv_
-atomic~rtProcessState~ currentState_
-atomic~uint32_t~ backgroundThreadCount_
-atomic~uint32_t~ lockedBackgroundThreadCount_
}
class SnapshotCallbackManager {
-mutex mutex_
-map~rtSnapShotStage,list~SnapShotCallBackInfo~~ callbackMap_
}
class IDeviceSnapshotOps {
<<interface>>
}
class DeviceSnapshot {
-vector~pair~void*,size_t~~ opVirtualAddrs_
-unique_ptr~uint8_t[]~ opBackUpAddrs_
-size_t opTotalHostMemSize_
-Device* device_
}
GlobalStateManager --> rtProcessState
SnapshotCallbackManager --> SnapShotCallBackInfo
SnapshotCallbackManager --> rtSnapShotStage
Device --> IDeviceSnapshotOps
DeviceSnapshot --> IDeviceSnapshotOps
```

## 5. 关键设计思想

### 5.1 进程状态机

进程状态转换遵循严格的状态机：

```mermaid
stateDiagram-v2
[*] --> RUNNING: 初始化
RUNNING --> LOCKED: rtSnapShotProcessLock()
LOCKED --> BACKED_UP: rtSnapShotProcessBackup()
BACKED_UP --> LOCKED: rtSnapShotProcessRestore()
LOCKED --> RUNNING: rtSnapShotProcessUnlock()
BACKED_UP --> RUNNING: rtSnapShotProcessUnlock()
```

### 5.2 API 阻塞机制

进程锁定后，所有涉及设备资源修改的 API 会通过 `GLOBAL_STATE_WAIT_IF_LOCKED()` 宏阻塞等待。

关键文件位置：`src/runtime/core/inc/common/global_state_manager.hpp`

### 5.3 回调阶段设计

回调在各关键阶段触发，允许上层应用执行自定义逻辑：

| 阶段 | 触发时机 | 典型用途 |
|------|----------|----------|
| LOCK_PRE | 锁定前 | 准备工作 |
| BACKUP_PRE | 备份前 | 保存额外状态 |
| BACKUP_POST | 备份后 | 确认备份完成 |
| RESTORE_PRE | 恢复前 | 准备恢复环境 |
| RESTORE_POST | 恢复后 | 确认恢复完成 |
| UNLOCK_POST | 解锁后 | 清理工作 |

### 5.4 后台线程管理

锁定时需要等待所有后台线程进入阻塞状态：
- `IncBackgroundThreadCount(name)`：后台线程注册需要阻塞
- `BackgroundThreadWaitIfLocked(name)`：后台线程检查并阻塞
- `DecBackgroundThreadCount(name)`：后台线程退出时取消注册

### 5.5 命名空间设计

所有快照相关组件都在 `cce::runtime` 命名空间下：
- `cce::runtime::GlobalStateManager`
- `cce::runtime::SnapshotCallbackManager`
- `cce::runtime::DeviceSnapshot`
- `cce::runtime::SnapShotProcessBackup/Restore` 等辅助函数

## 6. 关键文件索引

| 模块 | 文件路径 | 核心内容 |
|------|----------|----------|
| API 头文件 | `pkg_inc/runtime/rts/rts_snapshot.h` | 对外接口定义、状态枚举 |
| ACL 实现 | `src/acl/aclrt_impl/snapshot.cpp` | ACL 层接口封装 |
| Runtime API | `src/runtime/api/api_c_snapshot.cc` | C API 实现 |
| API 实现 | `src/runtime/core/src/api_impl/api_impl.cc` | SnapShotProcess 接口实现 |
| 状态管理 | `src/runtime/core/inc/common/global_state_manager.hpp` | GlobalStateManager 类定义 |
| 状态管理实现 | `src/runtime/core/src/common/global_state_manager.cc` | 进程状态管理实现 |
| 回调管理 | `src/runtime/feature/snapshot/snapshot_callback_manager.hpp` | SnapshotCallbackManager 类定义 |
| 回调管理实现 | `src/runtime/feature/snapshot/snapshot_callback_manager.cc` | 回调注册/注销/触发实现 |
| 设备快照接口 | `src/runtime/core/inc/common/idevice_snapshot_ops.hpp` | IDeviceSnapshotOps 抽象接口 |
| 设备快照 | `src/runtime/feature/snapshot/device_snapshot.hpp` | DeviceSnapshot 类定义 |
| 设备快照实现 | `src/runtime/feature/snapshot/device_snapshot.cc` | 内存备份/恢复实现 |
| 快照辅助 | `src/runtime/feature/snapshot/snapshot_process_helper.hpp` | 处理辅助函数声明 |
| 快照辅助实现 | `src/runtime/feature/snapshot/snapshot_process_helper.cc` | 辅助函数实现 |
| 设备适配 v100 | `src/runtime/feature/snapshot/v100/device_snapshot_adapter.cc` | v100 版本适配 |
| 设备适配 v200 | `src/runtime/feature/snapshot/v200_base/device_snapshot_adapter.cc` | v200 版本适配 |

## 7. 典型问题与排查

### 7.1 关键日志点

- `locked success.`：锁定成功
- `unlocked success.`：解锁成功
- `start to restore resource`：开始恢复资源
- `the resource is restored successfully`：恢复成功
- `current state is not the running state, current state is %s`：状态错误
- `Timeout: only %u/%u background threads locked`：后台线程锁定超时

### 7.2 问题定位方法

1. **锁定失败**：检查当前进程状态是否为 RUNNING
2. **锁定超时**：检查后台线程是否正确注册和阻塞
3. **备份失败**：检查 Context 同步是否成功，检查模型是否为 AICPU 类型（不支持）
4. **恢复失败**：检查 Device ReOpen、Stream ID 重分配等步骤

### 7.3 状态检查

通过 `rtSnapShotProcessGetState()` 获取当前进程状态。

---

_本特性文档基于源码 `src/runtime/feature/snapshot/` 及 `src/runtime/core/src/common/global_state_manager.cc` 分析。_