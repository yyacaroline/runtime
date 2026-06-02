# Model 模块架构

## 1. 模块概述

- **功能介绍**：Model 模块负责管理计算图的执行模型，包括模型创建、Stream 绑定、任务加载、模型执行和销毁等全生命周期管理。支持普通模型（Normal Model）和捕获模型（Capture Model）两种类型，通过 SQ/CQ 机制实现任务调度和执行。
- **设计目标**：
  - 提供统一的模型管理接口
  - 支持多 Stream 绑定和调度
  - 实现 Task 的批量提交和执行
  - 支持 Label 分配和跳转控制
  - 提供模型执行同步/异步模式

## 2. 使用场景与对外接口

### 2.1 使用场景

- **场景一**：创建并执行计算模型（使用 ACL 接口）
  ```cpp
  aclmdlRI modelRI;
  aclmdlRIBuildBegin(&modelRI, 0);  // 开始构建模型
  aclmdlRIBindStream(modelRI, stream, ACL_MODEL_STREAM_FLAG_HEAD);  // 绑定 Stream
  // 添加任务到 stream
  aclmdlRIEndTask(modelRI, stream);  // 标记任务结束
  aclmdlRIBuildEnd(modelRI, nullptr);  // 结束构建
  aclmdlRIExecute(modelRI, -1);  // 同步执行
  aclmdlRIDestroy(modelRI);  // 销毁模型
  ```

- **场景二**：异步模型执行（使用 ACL 接口）
  ```cpp
  aclmdlRIExecuteAsync(modelRI, execStream);
  // 通过 Stream 同步等待执行完成
  aclrtSynchronizeStream(execStream);
  ```

### 2.2 对外接口

| ACL 接口 | 文件位置 | 说明 |
|----------|----------|------|
| `aclmdlRIBuildBegin()` | `include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 创建模型（开始构建） |
| `aclmdlRIDestroy()` | `include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 销毁模型 |
| `aclmdlRIBindStream()` | `include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 绑定 Stream 到模型 |
| `aclmdlRIUnbindStream()` | `include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 解绑 Stream |
| `aclmdlRIExecute()` | `include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 同步执行模型 |
| `aclmdlRIExecuteAsync()` |`include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 异步执行模型 |
| `aclmdlRIBuildEnd()` | `include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 完成模型构建（LoadComplete） |
| `aclmdlRIEndTask()` | `include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 标记任务结束 |
| `aclmdlRISetName()` | `include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 设置模型名称 |
| `aclmdlRIAbort()` |`include/external/acl/acl_rt.h`<br>`src/acl/aclrt_impl/model_ri.cpp` | 终止模型执行 |

## 3. 架构总览

### 3.1 整体设计思路

Model 采用两层继承结构：基类 Model 提供普通模型的完整功能。每个 Model 绑定多个 Stream，通过 ModelExecuteTask 提交执行任务，使用 Notify 实现执行完成通知。

| 设计点 | 说明 |
|--------|------|
| **Model 作为任务容器** | 绑定多个 Stream，统一管理 SQE 下发和执行 |
| **Stream 作为任务队列** | 每个 Stream 维护 SQ 任务序列，支持任务回收 |
| **Label 实现控制流** | Model 分配 Label ID，Stream 记录跳转点 |
| **headStreams 机制** | 标记入口 Stream，Execute 时从这里激活执行 |

### 3.2 架构分层图

```mermaid
graph TB
    subgraph ContextLayer["Context 运行环境"]
        Context["Context<br/>管理多个 Model"]
    end
    
    subgraph ModelLayer["Model 核心容器"]
        Model["Model<br/>计算图容器"]
        ModelStreams["streams_<br/>所有绑定的 Stream"]
        ModelHeadStreams["headStreams_<br/>入口 Stream"]
        ModelLabelMap["labelMap_<br/>Label ID → Label"]
        ModelLabelAllocator["labelAllocator_<br/>Label ID 分配器"]
        ModelArgRecord["argLoaderRecord_<br/>参数句柄记录"]
        
        Model --> ModelStreams
        Model --> ModelHeadStreams
        Model --> ModelLabelMap
        Model --> ModelLabelAllocator
        Model --> ModelArgRecord
    end
    
    subgraph StreamLayer["Stream 任务队列层"]
        Stream1["Stream 1<br/>任务队列"]
        Stream2["Stream 2<br/>任务队列"]
        Stream3["Stream 3<br/>Slave Stream"]
        
        StreamModel["Model*<br/>反向引用"]
        StreamTasks["delayRecycleTaskid_<br/>任务 ID 列表"]
        StreamSQ["SQ 队列<br/>任务执行队列"]
        StreamCQ["CQ 队列<br/>完成队列"]
        StreamAutoSplit["AutoSplitSqContext<br/>自动切分上下文"]
        
        Stream1 --> StreamModel
        Stream1 --> StreamTasks
        Stream1 --> StreamSQ
        Stream1 --> StreamCQ
        
        Stream3 --> StreamAutoSplit
    end
    
    subgraph LabelLayer["Label 控制流层"]
        Label1["Label 1"]
        Label2["Label 2"]
        
        LabelModel["Model*<br/>所属 Model"]
        LabelStream["Stream*<br/>所在 Stream"]
        LabelId["labelId_<br/>由 Model 分配"]
        LabelDevAddr["devDstAddr_<br/>设备跳转地址"]
        
        Label1 --> LabelModel
        Label1 --> LabelStream
        Label1 --> LabelId
        Label1 --> LabelDevAddr
    end
    
    subgraph TaskLayer["任务层"]
        TaskInfo["TaskInfo<br/>任务信息"]
        TaskId["id: 任务 ID"]
        TaskPos["pos: SQE 位置"]
        TaskStream["Stream*<br/>所属 Stream"]
        TaskUpdateFlag["updateFlag<br/>KEEP/UPDATE/DISABLE"]
        
        TaskInfo --> TaskId
        TaskInfo --> TaskPos
        TaskInfo --> TaskStream
        TaskInfo --> TaskUpdateFlag
    end
    
    %% 关系连接
    Context --> Model
    
    ModelStreams --> Stream1
    ModelStreams --> Stream2
    ModelStreams --> Stream3
    ModelHeadStreams --> Stream1
    
    ModelLabelMap --> Label1
    ModelLabelMap --> Label2
    ModelLabelAllocator --> LabelId
    
    StreamTasks --> TaskInfo
    TaskStream --> Stream1
    
    %% Stream 与 Label 的跳转关系
    LabelStream -.跳转.-> Stream2
    
    %% AutoSplitSq 关系
    StreamAutoSplit -.Master/Slave.-> Stream1
    StreamAutoSplit -.Slave.-> Stream3
```

### 3.3 Model-Stream-Label 三层架构关系

```mermaid
graph LR
    subgraph Layer1["第一层：Model 容器"]
        ModelContainer["Model<br/><b>核心职责</b><br/>1. 绑定多个 Stream<br/>2. 分配 Label ID<br/>3. 管理 SQE 下发<br/>4. 执行入口管理"]
    end
    
    subgraph Layer2["第二层：Stream 队列"]
        StreamQueue["Stream<br/><b>核心职责</b><br/>1. 维护 SQ 任务队列<br/>2. 记录任务 ID 列表<br/>3. 反向引用 Model<br/>4. 支持任务回收"]
    end
    
    subgraph Layer3["第三层：Label 控制流"]
        LabelCtrl["Label<br/><b>核心职责</b><br/>1. 设置跳转目标<br/>2. 实现流间跳转<br/>3. 控制流分支<br/>4. 循环控制"]
    end
    
    %% 层间关系
    ModelContainer -->|"绑定 (binds)"| StreamQueue
    ModelContainer -->|"分配 (allocates)"| LabelCtrl
    StreamQueue -->|"设置跳转点 (set)"| LabelCtrl
    LabelCtrl -->|"跳转 (goto)"| StreamQueue
    
    %% 反向引用
    StreamQueue -.反向引用.-> ModelContainer
    LabelCtrl -.反向引用.-> ModelContainer
```

### 3.4 核心模块交互图（含硬件交互）

```mermaid
sequenceDiagram
    participant App as 应用程序
    participant ACL as ACL 接口层
    participant RT as Runtime 核心层
    participant Model as Model 容器
    participant Stream as Stream 队列
    participant Notify as Notify 同步对象
    participant Device as Device 设备管理
    participant Driver as Driver 驱动层
    participant HW as 硬件层<br/>TS/SQ/CQ
    
    %% 模型创建阶段
    Note over App,HW: 阶段 1: Model 创建与 Stream 绑定
    App->>ACL: aclmdlRIBuildBegin()
    ACL->>RT: rtModelCreate()
    RT->>Model: new Model()
    Model->>Device: ModelIdAlloc()
    Device->>Driver: ModelIdAlloc(deviceId)
    Driver-->>Device: modelId
    Device-->>Model: modelId
    Model->>Model: 创建 Notifier/LabelAllocator
    Model-->>RT: Model 对象
    RT-->>ACL: modelRI handle
    ACL-->>App: modelRI
    
    %% Stream 绑定阶段
    App->>ACL: aclmdlRIBindStream(modelRI, stream)
    ACL->>RT: rtModelBindStream()
    RT->>Model: BindStream(stream)
    Model->>Stream: SetModel(this)
    Stream-->>Model: bindFlag=true
    Model->>Device: SubmitTask(ModelMaintainceTask)
    Device->>Driver: SubmitTask(taskInfo)
    Driver->>HW: 写入 SQ<br/>MSG: MMT_STREAM_ADD
    HW-->>Driver: CQE (完成响应)
    Driver-->>Device: RT_ERROR_NONE
    Device-->>Model: 绑定成功
    Model-->>RT: streams_ 更新
    RT-->>ACL: success
    ACL-->>App: ACL_SUCCESS
    
    %% 任务提交阶段
    Note over App,HW: 阶段 2: 任务提交与 SQE 下发
    App->>ACL: rtKernelLaunch(kernel, stream)
    ACL->>RT: rtKernelLaunch()
    RT->>Stream: AllocTask()
    Stream->>Stream: 记录到 delayRecycleTaskid_
    Stream->>Stream: SQE 写入 sqeBuffer_
    Stream-->>RT: TaskInfo
    RT-->>ACL: success
    
    %% EndGraph 标记
    App->>ACL: aclmdlRIEndTask(modelRI, stream)
    ACL->>RT: rtEndGraph()
    RT->>Stream: AddEndGraphTask()
    Stream->>Device: SubmitTask(EndGraphTask)
    Device->>Driver: SubmitTask()
    Driver->>HW: 写入 SQ<br/>MSG: END_GRAPH
    HW-->>Driver: CQE
    Driver-->>Device: success
    Model->>Notify: 创建 endGraphNotify_
    Notify-->>Model: notifyId
    
    %% LoadComplete 阶段
    App->>ACL: aclmdlRIBuildEnd(modelRI)
    ACL->>RT: rtModelLoadComplete()
    RT->>Model: LoadComplete()
    Model->>Model: SendSqe()
    Model->>Device: 遍历 StreamList_
    Device->>Driver: StreamTaskFill(deviceId, streamId, sqAddr, sqeBuffer, sqeNum)
    Driver->>HW: 批量写入 SQ<br/>MSG: SQE_BATCH<br/>DATA: [SQE1, SQE2, ...]
    HW-->>Driver: CQE_BATCH
    Driver-->>Device: RT_ERROR_NONE
    Model->>Device: ConfigSqTail()
    Device->>Driver: SetSqTail(deviceId, tsId, sqId, sqTailPos)
    Driver->>HW: 配置 SQ Tail<br/>MSG: SQ_TAIL_UPDATE
    HW-->>Driver: success
    Driver-->>Device: success
    Model->>Model: isModelComplete_=true
    Model-->>RT: success
    RT-->>ACL: success
    ACL-->>App: ACL_SUCCESS
    
    %% 模型执行阶段
    Note over App,HW: 阶段 3: Model 执行与硬件交互
    App->>ACL: aclmdlRIExecute(modelRI)
    ACL->>RT: rtModelExecute()
    RT->>Model: ExecuteSync()
    Model->>Model: SubmitExecuteTask()
    Model->>Device: SubmitTask(ModelExecuteTask)
    Device->>Driver: SubmitTask(exeTask)
    Driver->>HW: 写入 SQ<br/>MSG: MODEL_EXECUTE<br/>DATA: modelId, headStreams
    Driver->>HW: 激活 TS 硬件
    HW->>HW: TS 从 SQ 读取 SQE
    HW->>HW: AICore/AIVector 执行计算
    HW->>HW: 写入 CQ<br/>MSG: CQE_COMPLETION<br/>DATA: taskId, errorCode
    HW-->>Driver: CQE (执行完成)
    Driver-->>Device: completion event
    Device-->>Notify: 触发 endGraphNotify_
    Notify->>Notify: Notify::Signal()
    Model->>Notify: NtyWait(endGraphNotify_, stream)
    Notify-->>Model: wait success
    Model->>Stream: Synchronize()
    Stream->>Device: StreamSync()
    Device->>Driver: GetCqHead()
    Driver->>HW: 读取 CQ<br/>MSG: CQ_POLL
    HW-->>Driver: CQ status
    Driver-->>Device: all completed
    Device-->>Stream: sync success
    Stream-->>Model: execution done
    Model-->>RT: success
    RT-->>ACL: success
    ACL-->>App: ACL_SUCCESS
    
    %% 模型销毁阶段
    Note over App,HW: 阶段 4: Model 销毁
    App->>ACL: aclmdlRIDestroy(modelRI)
    ACL->>RT: rtModelDestroy()
    RT->>Model: TearDown()
    Model->>Notify: DELETE_O(endGraphNotify_)
    Model->>Model: ClearMemory()
    Model->>Device: SubmitTask(ModelDestroyTask)
    Device->>Driver: SubmitTask()
    Driver->>HW: 写入 SQ<br/>MSG: MODEL_DESTROY
    HW-->>Driver: CQE
    Model-->>RT: destroyed
    RT-->>ACL: success
    ACL-->>App: ACL_SUCCESS
```

## 4. 详细设计

### 4.1 核心流程

#### 模型创建与初始化流程

```mermaid
flowchart TD
    A["用户调用 aclmdlRIBuildBegin()"] --> B["创建 Model 对象"]
    B --> C["初始化基本属性<br/>modelId, name"]
    C --> D["创建核心管理对象"]
    D --> E["分配 LabelAllocator<br/>用于控制流跳转"]
    D --> F["创建 Notifier<br/>用于执行完成通知"]
    E --> H["准备 argLoaderRecord_<br/>参数句柄管理"]
    F --> H
    H --> I["Model 对象就绪<br/>"]
    
    style A fill:#e1f5ff
    style I fill:#c8e6c9
```

**流程说明**：
1. 用户通过 ACL 接口发起模型创建请求
2. Runtime 创建 Model 对象，分配唯一的 modelId
3. 初始化 Label 分配器，支持后续的分支跳转控制
4. 创建 Notifier 对象，用于模型执行完成后的通知机制
5. 申请设备侧内存，用于存储模型元数据
6. 准备参数句柄记录表，管理任务的参数内存

#### Stream 绑定流程

```mermaid
flowchart TD
    A["用户调用 aclmdlRIBindStream()"] --> B["检查 Stream 状态"]
    B --> C{"Stream 是否已绑定?"}
    C -->|已绑定| D["返回错误<br/>不可重复绑定"]
    C -->|未绑定| E["添加到 Model 的 streams_ 列表"]
    
    E --> F{"是否为入口 Stream?<br/>flag=HEAD_STREAM"}
    F -->|是| G["添加到 headStreams_<br/>作为模型执行起点"]
    F -->|否| H["仅添加到 streams_<br/>普通执行流"]
    
    G --> I["建立model、stream双向引用关系"]
    H --> I
    I --> K["创建维护任务<br/>"]
    K --> L["将任务提交到设备"]
    L --> M["硬件确认绑定成功<br/>返回 CQE"]
    M --> O["绑定完成<br/>可开始提交任务"]
    
    style A fill:#e1f5ff
    style D fill:#ffcdd2
    style O fill:#c8e6c9
```

**流程说明**：
1. 用户请求将 Stream 绑定到 Model
2. 检查 Stream 状态，防止重复绑定
3. 将 Stream 添加到 Model 的 Stream 列表
4. 判断是否为入口 Stream（HEAD_STREAM），入口 Stream 是模型执行的起点
5. 建立双向引用：Stream 反向指向 Model，Model 持有 Stream 列表
6. 创建维护任务，通知硬件建立 Stream 与 Model 的关系
7. 提交任务到设备，硬件返回确认（CQE）
8. 设置 Stream 的绑定标志，准备接收任务

#### EndGraph 标记流程

```mermaid
flowchart TD
    A["用户调用 aclmdlRIEndTask()"] --> B["创建 EndGraph 任务"]
    B --> C["生成 Notify 对象<br/>endGraphNotify_"]
    C --> D["申请 NotifyId<br/>硬件完成通知标识"]
    
    D --> E["将 EndGraph 任务写入 SQ"]
    E --> F["任务包含 NotifyId<br/>硬件完成时触发通知"]
    F --> G["EndGraph 标记成功"]
    
    style A fill:#e1f5ff
    style G fill:#c8e6c9
```

**流程说明**：
1. 用户标记任务下发结束
2. 创建 EndGraph 任务，作为模型的结束标记
3. 创建 Notify 对象，申请唯一的 NotifyId
4. EndGraph 任务写入 SQ，包含 NotifyId
5. 硬件执行到 EndGraph 任务时，触发 Notify Signal
6. 标记成功，模型进入可执行状态

#### Model LoadComplete 流程

```mermaid
flowchart TD
    A["用户调用 aclmdlRIBuildEnd()"] --> B["触发 LoadComplete"]
    
    B --> C{"是否启用 AutoSplitSq?"}
    C -->|是| D["为所有 Stream 分配 SQ 地址"]
    D --> E["处理 Master/Slave Stream 关系"]
    E --> F["配置 Stream 切换信息"]
    F --> G
    C -->|否| G["遍历 Model 的 StreamList_"]
    
    G --> H["读取每个 Stream 的 sqeBuffer_"]
    H --> I["计算总 SQE 数量<br/>totalSqeNum"]
    
    I --> J["批量下发 SQE 到设备 SQ"]
    J --> K["调用 StreamTaskFill<br/>写入设备 SQ 内存"]
    
    K --> M["配置 SQ Tail 指针"]
    M --> O["设置 isModelComplete_=true"]
    
    O --> P["模型加载完成<br/>可执行模型"]
    
    style A fill:#e1f5ff
    style P fill:#c8e6c9
```

**流程说明**：
1. 用户调用 LoadComplete，触发批量下发
2. 判断是否启用 AutoSplitSq（自动 SQ 切分）：
   - 启用：为所有 Stream 分配 SQ 地址，处理 Master/Slave 关系
   - 不启用：直接遍历 StreamList_
3. 读取每个 Stream 的 sqeBuffer_，计算总 SQE 数量
4. 批量下发 SQE 到设备 SQ 内存（StreamTaskFill）
5. 硬件 SQ 准备就绪，任务驻留在设备侧
6. 配置 SQ Tail 指针，激活 TS（Task Scheduler）读取任务
7. 设置模型完成标志，进入可执行状态

#### 模型执行流程

```mermaid
flowchart TD
    A["用户调用 aclmdlRIExecute()"] --> B["触发模型执行"]
    
    B --> C["准备执行环境"]
    C --> D["获取执行 Stream<br/>使用默认或指定 Stream"]
    D --> E["检查模型状态<br/>isModelComplete_=true"]
    
    E --> F{"模型是否 LoadComplete?"}
    F -->|未完成| G["返回错误<br/>模型未就绪"]
    F -->|已完成| H["创建 ExecuteTask"]
    
    H --> J["提交 ExecuteTask 到设备"]
    
    J --> K["写入 SQ: MODEL_EXECUTE"]
    K --> L["硬件 TS 接收到执行指令"]
    L --> M["激活 headStreams_<br/>从入口 Stream 开始"]
    
    M --> N["TS 从 SQ 读取 SQE"]
    N --> O["逐个执行任务计算任务"]
    
    O --> P["执行到 EndGraph 任务"]
    P --> Q["触发 Notify Signal"]
    Q --> S["Host 端等待完成"]
    S --> T["调用 NtyWait 等待 Notify"]
    T --> U["等待硬件返回完成信号"]
    
    U --> V{"收到完成信号?"}
    V -->|成功| W["Stream 同步完成"]
    V -->|超时| X["返回超时错误"]
    
    W --> Y["轮询 CQ 状态"]
    Y --> Z["确认所有任务完成"]
    Z --> AA["模型执行完成<br/>返回成功"]
    
    style A fill:#e1f5ff
    style G fill:#ffcdd2
    style X fill:#ffcdd2
    style AA fill:#c8e6c9
```

**流程说明**：
1. 用户调用 Execute，触发模型执行
2. 准备执行环境，获取执行 Stream
3. 检查模型状态，确保 LoadComplete 已完成
4. 创建 ExecuteTask，设置执行参数（modelId、入口 Stream）
5. 提交 ExecuteTask 到设备，写入 SQ
6. 硬件 TS 接收执行指令，激活 headStreams_
7. TS 从 SQ 读取 SQE，逐个执行任务
8. AICore/AIVector 执行计算任务
9. 执行到 EndGraph 任务，触发 Notify Signal
10. Host 端等待完成（NtyWait）
11. 收到完成信号后，轮询 CQ 状态
12. 确认所有任务完成，返回成功

#### Label 控制流设置流程

```mermaid
flowchart TD
    A["用户创建 Label"] --> B["向 Model 申请 LabelId"]
    B --> C["Model 分配唯一 LabelId"]
    C --> D["记录到 labelMap_<br/>LabelId → Label 对象"]
    
    D --> E["Label 绑定到 Model"]
    E --> F["设置 Label 所在 Stream"]
    
    F --> G["用户调用 Label::Set()"]
    G --> H["在 Stream 上设置跳转点"]
    H --> I["记录当前位置到 devDstAddr_<br/>硬件跳转目标地址"]
    I --> J["生成 SetLabel SQE"]
    J --> K["SQE 写入 Stream 缓存"]
    K --> L["跳转点设置完成"]
    
    style A fill:#e1f5ff
    style L fill:#c8e6c9
```

**流程说明**：
1. 用户创建 Label，向 Model 申请唯一的 LabelId
2. Model 通过 LabelAllocator 分配 ID，记录映射关系
3. Label 绑定到 Model 和 Stream
4. 用户调用 Label::Set，在 Stream 上设置跳转目标点
5. 记录当前位置到设备侧地址（devDstAddr_）
6. 生成 SetLabel SQE，写入 Stream 缓存
7. 跳转点设置完成，等待后续跳转

#### Label 控制流跳转流程

```mermaid
flowchart TD
    A["用户调用 Label::Goto()"] --> B["准备跳转指令"]
    B --> C["获取目标 Label 信息"]
    
    C --> D["读取 Label 的 devDstAddr_<br/>目标跳转地址"]
    D --> E["生成 GotoLabel SQE"]
    E --> F["SQE 包含目标地址"]
    
    F --> G["写入当前 Stream 的 SQ"]
    G --> H["硬件 TS 执行到 Goto 指令"]
    
    H --> I["TS 跳转到目标 Stream"]
    I --> J["从 devDstAddr_ 继续执行"]
    
    J --> K{"目标 Stream 是否相同?"}
    K -->|相同 Stream| L["Stream 内跳转<br/>循环/分支"]
    K -->|不同 Stream| M["跨 Stream 跳转<br/>流间切换"]
    
    L --> N["跳转完成<br/>继续执行任务"]
    M --> N
    
    style A fill:#e1f5ff
    style N fill:#c8e6c9
```

**流程说明**：
1. 用户调用 Label::Goto，发起跳转指令
2. 获取目标 Label 的设备侧地址（devDstAddr_）
3. 生成 GotoLabel SQE，包含目标跳转地址
4. 写入当前 Stream 的 SQ
5. 硬件 TS 执行到 Goto 指令，跳转到目标位置
6. 从目标地址继续执行任务
7. 判断跳转类型：
   - 相同 Stream：实现循环或分支控制
   - 不同 Stream：实现跨 Stream 任务切换

#### 模型销毁流程

```mermaid
flowchart TD
    A["用户调用 aclmdlRIDestroy()"] --> B["触发 TearDown"]
    
    B --> C["释放 Notify 资源"]
    C --> D["删除 endGraphNotify_"]
    D --> E["释放设备内存"]
    
    E --> F["清理 argLoaderRecord_<br/>释放参数句柄"]
    F --> G["清理 dmaAddrRecord_<br/>释放 DMA 地址"]
    G --> H["解绑所有 Label"]
    
    H --> I["遍历 labelMap_<br/>解除 Label 与 Model 关系"]
    I --> J["释放 LabelAllocator"]
    J --> K["创建销毁任务"]
    
    K --> L["提交 ModelDestroyTask"]
    L --> M["通知硬件销毁模型"]
    M --> N["硬件确认销毁<br/>返回 CQE"]
    
    N --> O["清理 Model 对象"]
    O --> P["从 Context 移除 Model"]
    P --> Q["模型销毁完成"]
    
    style A fill:#e1f5ff
    style Q fill:#c8e6c9
```

**流程说明**：
1. 用户调用 Destroy，触发模型销毁
2. 释放 Notify 资源，删除 endGraphNotify_
3. 释放设备内存，清理参数句柄记录
4. 释放 DMA 地址记录
5. 解绑所有 Label，解除与 Model 的关系
6. 释放 LabelAllocator 资源
7. 创建销毁任务，通知硬件销毁模型
8. 硬件确认销毁，返回 CQE
9. 清理 Model 对象，从 Context 移除
10. 模型销毁完成，释放所有资源

### 4.2 核心机制详解

#### Model 容器管理机制

**设计思想**：Model 作为计算图的容器，统一管理所有绑定的 Stream 和 Label，实现任务批量下发和执行入口控制。

```mermaid
graph TB
    subgraph ModelManagement["Model 容器管理"]
        StreamBinding["Stream 绑定管理<br/>streams_: 所有 Stream<br/>headStreams_: 入口 Stream"]
        LabelMgmt["Label 控制流管理<br/>labelMap_: Label 映射<br/>labelAllocator_: ID 分配"]
        ArgMgmt["参数内存管理<br/>argLoaderRecord_: 参数句柄"]
        ExecMgmt["执行控制<br/>endGraphNotify_: 完成通知<br/>isModelComplete_: 状态标记"]
    end
    
    Model["Model 容器"]
    
    Model --> StreamBinding
    Model --> LabelMgmt
    Model --> ArgMgmt
    Model --> ExecMgmt
    
    StreamBinding -->|"批量下发"| SQE["SQE 批量下发<br/>StreamTaskFill"]
    StreamBinding -->|"入口激活"| Head["执行起点<br/>headStreams_"]
    
    LabelMgmt -->|"ID 分配"| LabelId["LabelId<br/>唯一标识"]
    LabelMgmt -->|"跳转控制"| Jump["Stream 跳转<br/>跨流控制"]
    
    ExecMgmt -->|"完成通知"| Notify["Notify 等待<br/>NtyWait"]
    ExecMgmt -->|"状态检查"| Ready["就绪检查<br/>LoadComplete"]
```

**机制说明**：
- **Stream 绑定管理**：维护所有 Stream 列表，headStreams_ 作为执行起点
- **Label 控制流管理**：分配 Label ID，管理跳转关系
- **参数内存管理**：记录任务参数句柄，避免重复分配
- **执行控制**：Notify 机制实现异步等待，状态标记确保执行顺序

#### Stream 任务队列机制

**设计思想**：Stream 作为任务队列，缓存 SQE，支持任务回收和自动 SQ 切分。

```mermaid
graph TB
    subgraph StreamQueue["Stream 任务队列"]
        TaskCache["任务缓存<br/>delayRecycleTaskid_<br/>任务 ID 列表"]
        SQEBuffer["SQE 缓冲<br/>sqeBuffer_<br/>任务描述符"]
        PosMap["位置映射<br/>posToTaskIdMap_<br/>位置→任务"]
        AutoSplit["自动切分<br/>AutoSplitSqContext<br/>Master/Slave"]
    end
    
    Submit["任务提交"]
    
    Submit --> TaskCache
    TaskCache --> SQEBuffer
    SQEBuffer --> PosMap
    
    TaskCache -->|"缓存数量"| Count["任务计数<br/>curStreamSqeCount"]
    Count -->|"超过 32K"| AutoSplit
    AutoSplit -->|"创建 Slave"| Slave["Slave Stream"]
    AutoSplit -->|"Master 管理"| Master["Master Stream"]
    
    SQEBuffer -->|"批量下发"| Batch["LoadComplete<br/>批量下发"]
    PosMap -->|"任务回收"| Recycle["任务回收<br/>TaskReclaim"]
```

**机制说明**：
- **任务缓存**：delayRecycleTaskid_ 记录任务 ID，等待批量下发
- **SQE 缓冲**：sqeBuffer_ 存储任务描述符，准备写入硬件 SQ
- **位置映射**：posToTaskIdMap_ 记录 SQE 位置与任务的对应关系
- **自动切分**：超过 32K 任务时自动创建 Slave Stream 扩展容量

#### Label 控制流跳转机制

**设计思想**：Label 实现跨 Stream 跳转和循环控制，通过设备侧地址实现硬件级跳转。

```mermaid
graph TB
    subgraph LabelAlloc["Label 分配"]
        Request["用户申请 Label"]
        Alloc["Model 分配 ID"]
        Record["记录到 labelMap_"]
    end
    
    subgraph LabelSet["Label 设置"]
        ChooseStream["选择目标 Stream"]
        RecordPos["记录当前位置"]
        DevAddr["设置 devDstAddr_<br/>设备跳转地址"]
    end
    
    subgraph LabelGoto["Label 跳转"]
        GetAddr["读取 devDstAddr_"]
        GenSQE["生成 Goto SQE"]
        HWJump["硬件跳转执行"]
    end
    
    Request --> Alloc --> Record
    Record --> ChooseStream --> RecordPos --> DevAddr
    
    DevAddr --> GetAddr --> GenSQE --> HWJump
    
    HWJump -->|"同 Stream"| Loop["循环控制"]
    HWJump -->|"跨 Stream"| Switch["流间切换"]
    
    style Request fill:#e1f5ff
    style HWJump fill:#c8e6c9
```

**机制说明**：
- **Label 分配**：Model 通过 LabelAllocator 分配唯一 ID
- **Label 设置**：在目标 Stream 上记录跳转位置，设置设备侧地址
- **Label 跳转**：读取设备地址，生成跳转 SQE，硬件执行跳转
- **控制流实现**：支持循环（同 Stream）和跨 Stream 任务切换

#### Notify 完成通知机制

**设计思想**：Notify 实现硬件完成通知，Host 通过等待 Notify 获取执行结果。

```mermaid
sequenceDiagram
    participant Model as Model
    participant EndGraph as EndGraphTask
    participant Notify as endGraphNotify_
    participant Driver as Driver
    participant HW as 硬件 TS
    
    %% EndGraph 创建 Notify
    Model->>EndGraph: rtEndGraph(model, stream)
    EndGraph->>Notify: 创建 Notify 对象
    Notify->>Driver: AllocNotifyId()
    Driver->>HW: 申请 notifyId
    HW-->>Driver: notifyId
    Driver-->>Notify: notifyId
    Notify-->>Model: endGraphNotify_
    
    %% 执行时写入 EndGraph Task
    Model->>EndGraph: Execute()
    EndGraph->>Driver: SubmitTask(EndGraphTask)
    Driver->>HW: SQE: END_GRAPH<br/>notifyId=notifyId_
    HW->>HW: 执行所有前置任务
    HW->>HW: 执行 EndGraph Task
    HW->>HW: Signal Notify<br/>notifyId ← value++
    
    %% Host 等待 Notify
    Model->>Notify: NtyWait(endGraphNotify_, stream)
    Notify->>Driver: Notify::Wait()
    Driver->>HW: Poll Notify Value<br/>MSG: NOTIFY_POLL
    HW-->>Driver: notifyValue
    Driver-->>Notify: value matched
    Notify-->>Model: wait success
    Model->>Model: 执行完成
```

**机制说明**：
- **Notify 创建**：EndGraph 任务申请 NotifyId，绑定到 Model
- **Notify 等待**：Host 通过 NtyWait 轮询 Notify Value
- **Notify 触发**：硬件执行到 EndGraph，Signal Notify，更新 Value
- **完成确认**：Host 检测到 Value 匹配，确认执行完成

### 4.3 模块职责划分

Model 模块按照功能划分为核心容器、任务管理、接口层、辅助工具等子模块，各模块职责清晰，协同完成模型全生命周期管理。

#### 核心容器模块

| 模块名称 | 核心职责 | 代码位置 | 主要功能 |
|----------|----------|----------|----------|
| **Model 核心类** | 计算图容器，统一管理 Stream/Label/Notify | `src/runtime/core/inc/model/model.hpp`<br/>`src/runtime/feature/model/model.cc` | • 绑定/解绑 Stream<br/>• 分配/释放 Label<br/>• 执行同步/异步模型<br/>• 批量下发 SQE<br/>• 管理 argLoaderRecord |
| **Model C 接口** | 对外 C 风格 API 封装 | `src/runtime/feature/model/model_c.cc` | • rtModelCreate/Destroy<br/>• rtModelBindStream<br/>• rtModelExecute<br/>• rtEndGraph<br/>• rtModelLoadComplete |
| **Model Rebuild** | 模型重建与恢复 | `src/runtime/feature/model/model_rebuild.cc` | • ReBuild 重建模型<br/>• ReSetup 恢复状态<br/>• ReBindStreams 重新绑定<br/>• ReAllocStreamId 重分配 ID |

#### 任务管理模块

| 任务类型 | 任务职责 | 代码位置 | 触发时机 |
|----------|----------|----------|----------|
| **ModelExecuteTask** | 触发模型执行，激活 headStreams | `src/runtime/core/src/task/task_info/model/model_execute_task.cc`<br/>`src/runtime/core/src/task/task_info/model/model_execute_task_v100.cc`<br/>`src/runtime/core/src/task/task_info/model/model_execute_task_v200_base.cc` | Execute/ExecuteAsync 调用时 |
| **ModelMaintainceTask** | 维护 Stream 与 Model 关系 | `src/runtime/core/src/task/task_info/model/model_maintaince_task.cc`<br/>`src/runtime/core/src/task/task_info/model/model_maintaince_task_v100.cc`<br/>`src/runtime/core/src/task/task_info/model/model_maintaince_task_v200_base.cc` | BindStream/UnbindStream<br/>LoadComplete 时 |
| **ModelGraphTask** | EndGraph 标记，创建 Notify | `src/runtime/core/src/task/task_info/model/model_graph_task.cc`<br/>`src/runtime/core/src/task/task_info/model/model_graph_task_v100.cc`<br/>`src/runtime/core/src/task/task_info/model/model_graph_task_v200_base.cc` | rtEndGraph 调用时 |
| **ModelUpdateTask** | CaptureModel 参数更新 | `src/runtime/core/src/task/task_info/model/model_update_task.cc`<br/>`src/runtime/core/src/task/task_info/model/model_update_task_v100.cc`<br/>`src/runtime/core/src/task/task_info/model/model_update_task_v200_base.cc` | StreamBeginTaskUpdate<br/>StreamEndTaskUpdate |
| **KernelFusionTask** | Kernel 融合优化 | `src/runtime/core/src/task/task_info/model/kernel_fusion_task.cc` | 融合算子执行时 |

#### 辅助工具模块

| 工具模块 | 辅助职责 | 代码位置 | 服务对象 |
|----------|----------|----------|----------|
| **LabelAllocator** | Label ID 分配器 | `src/runtime/core/inc/launch/label.hpp` | Model Label 管理 |
| **Notify** | 硬件完成通知对象 | `src/runtime/core/inc/notify/notify.hpp`<br/>`src/runtime/core/src/notify/notify.cc` | EndGraph Notify |
| **Notifier** | 多 Notify 管理 | `src/runtime/core/inc/utils/osal.hpp` | Model 执行完成通知 |

#### 接口适配层

| 适配层 | 适配职责 | 代码位置 | 服务对象 |
|----------|----------|----------|----------|
| **ACL 接口封装** | ACL → RT 接口映射 | `src/acl/aclrt_impl/model_ri.cpp` | 用户调用 aclmdlRI 接口 |
| **API 装饰器** | API 错误处理装饰 | `src/runtime/core/src/api_impl/api_decorator.cc`<br/>`src/runtime/feature/aclgraph/api_decorator_aclgraph.cc` | ApiImpl 调用链 |
| **API 错误处理** | API 错误码封装 | `src/runtime/core/src/api_impl/api_error.cc`<br/>`src/runtime/feature/aclgraph/api_error_aclgraph.cc` | 错误码处理 |


### 5 关键文件路径索引

| 模块类别 | 文件路径 | 核心内容 |
|----------|----------|----------|
| **Model 头文件** | `src/runtime/core/inc/model/model.hpp` | Model 类定义、数据结构 |
| **Model 实现** | `src/runtime/feature/model/model.cc` | Model 核心实现 |
| **Model C 接口** | `src/runtime/feature/model/model_c.cc` | C 风格接口实现 |
| **Model Rebuild** | `src/runtime/feature/model/model_rebuild.cc` | 模型重建逻辑 |
| **Model Execute Task** | `src/runtime/core/src/task/task_info/model/model_execute_task.cc` | 执行任务实现 |
| **Model Execute Task V100** | `src/runtime/core/src/task/task_info/model/model_execute_task_v100.cc` | V100 版本适配 |
| **Model Execute Task V200** | `src/runtime/core/src/task/task_info/model/model_execute_task_v200_base.cc` | V200 版本适配 |
| **Model Maintaince Task** | `src/runtime/core/src/task/task_info/model/model_maintaince_task.cc` | 维护任务实现 |
| **Model Graph Task** | `src/runtime/core/src/task/task_info/model/model_graph_task.cc` | 图任务实现 |
| **Model To Aicpu Task** | `src/runtime/core/src/task/task_info/model/model_to_aicpu_task.cc` | AICPU 任务实现 |
| **Model Update Task** | `src/runtime/core/src/task/task_info/model/model_update_task.cc` | 更新任务实现 |
| **ACL 接口头文件** | `include/external/acl/acl_rt.h` | ACL 外部接口定义 |
| **ACL 接口实现** | `src/acl/aclrt_impl/model_ri.cpp` | ACL 接口实现（aclmdlRI 系列） |
