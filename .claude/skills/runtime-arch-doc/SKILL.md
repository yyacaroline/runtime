---
name: runtime-arch-doc
description: CANN Runtime 架构文档生成与刷新。Use when 用户要求 (1) 生成 Runtime 架构说明文档，(2) 基于最新代码刷新/更新架构说明文档，(3) 分析 runtime 模块架构并输出 md 文档。支持基于源码分析的文档生成、模块拆分、mermaid 图表生成。
---
# Runtime 架构文档生成 Skill

本 skill 用于生成和刷新 CANN Runtime 架构说明文档。

## 快速开始

**生成完整架构文档：**

1. 确认源码路径（默认 `~/cann/runtime/src/runtime`）
2. 确认输出路径（默认 `~/cann/runtime-doc/`）
3. 按模版格式生成文档

**刷新特定模块：**

指定模块名（device/context/stream/task/memory/kernel/event/model/runtime），基于最新源码更新对应文档。

**刷新特定特性：**

指定特性功能（如：ACL Graph/snapshot），基于最新的源码更新对应文档。

## 文档格式规范

输出文档遵循 [模版规范](references/template.md)，核心要点：

- **README.md** — 导航索引，表格化文档列表
- **architecture.md** — 系统架构总览，mermaid 架构图
- **modules/[name]/[name].md** — 模块详细架构
- **features/[name].md** — 特性功能详细说明

**必须包含：**

- mermaid 架构分层图（graph TB）
- mermaid 核心类图（classDiagram）
- mermaid 流程图（flowchart TD/sequenceDiagram）
- 文件路径标注（如 `device/device.hpp`）
- 代码片段示例
- 严格按照代码逻辑，禁止捏造
- 更新文档时，要尊重原文档内容。当发现与原文档内容有冲突时，输出原文和要修改的内容后提示用户，确认后再进行修改

**详见模版：** [references/template.md](references/template.md)

## 工作流程

### 生成完整文档

1. 确认源码路径
2. 扫描目录 **runtime/src/runtime/**
3. 分析Runtime代码结构，识别核心功能模块 (位于core目录) 和关键特性 (位于feature目录)
4. 读取已有文档作为参考
5. 分析核心模块功能，输出模块详细架构，**modules/[name]/[name].md**
6. 分析关键特性，输出特性功能详细说明，**features/[name].md**
7. 生成 **architecture.md**
8. 生成 **README.md**
9. 验证格式正确

### 刷新单个模块或特性

1. 确认模块名或特性名
2. 扫描对应源码目录
3. 对比已有文档
4. 识别变更点
5. 更新文档内容
6. 验证 mermaid 图表
7. 刷新 architecture.md
8. 刷新 README.md

## 源码分析要点

### 目录结构扫描

```bash
# Runtime 源码位置
runtime/
├── include/
├── src/
│   ├── acl/
│   └── runtime/
│       ├── api             # API 接口层
│       ├── core/src/       # 核心模块实现
│       │   ├── runtime.cc  # Runtime 全局管理
│       │   ├── device/     # Device 设备管理
│       │   ├── stream/     # Stream 流管理
│       │   └── ...
│       ├── feature         # 功能特性实现
│       │   ├── aclgraph/
│       │   └── ...
│       ├── config          # 硬件差异配置
│       └── driver          # 驱动适配层
└── tests/
```

### 关键文件定位


| 模块    | 头文件              | 实现文件      |
| ------- | ------------------- | ------------- |
| Runtime | `inc/runtime.hpp`   | `runtime.cc`  |
| Device  | `device/device.hpp` | `device/*.cc` |
| Stream  | `stream/stream.hpp` | `stream/*.cc` |
| Task    | `task/inc/*.h`      | `task/*.cc`   |

### 类关系分析

重点提取：

- 类继承关系（Device → RawDevice, GroupDevice）
- 组合关系（Device 包含 TaskFactory, MemoryPool）
- 核心成员变量（streamId_, sqId_, cqId_）
- 关键方法（Init(), Start(), SubmitTask()）

## 输出内容要求

### architecture.md 必备章节

1. **系统架构总览** — 整体架构图（API/Core/Driver 三层）
2. **核心组件介绍** — 各模块职责
3. **架构设计原则** — 分层、单例、异步等
4. **项目结构** — 目录树

### modules/[name].md 必备章节

1. **模块概述** — 功能和目标
2. **使用场景与对外接口** — 表格化接口列表
3. **架构总览** — 架构分层图 + 交互图
4. **详细设计** — 流程图 + 类图 + 代码片段
5. **关键文件索引** — 文件路径表格

### features/[name].md 必备章节

1. **特性概述** — 功能和目标
2. **使用场景与对外接口** — 表格化接口列表
3. **架构总览** — 架构分层图 + 交互图
4. **详细设计** — 流程图 + 类图 + 代码片段
5. **关键文件索引** — 文件路径表格

## 禁止捏造

**严格要求：**

- 所有类名、方法名、成员变量必须来自源码
- 流程图必须基于实际代码逻辑
- 文件路径必须真实存在
- 不存在的功能不要写入文档

**验证方法：**

```bash
# 验证文件存在
ls ~/cann/runtime/src/runtime/core/src/device/device.hpp

# 验证类定义
grep "class Device" ~/cann/runtime/src/runtime/core/src/device/device.hpp
```

## 参考资源

- **模版规范**：[references/template.md](references/template.md) — 完整文档格式模版
- **已有文档**：`~/cann/runtime-doc/` — 作为参考和对比基础
- **GE 文档示例**：https://gitcode.com/cann/ge/tree/master/docs/architecture

## 命令示例

```bash
# 初始化 skill 后首次生成
# 用户请求："生成 runtime 架构文档"
# 输出路径：~/cann/runtime-doc/

# 刷新 device 模块
# 用户请求："刷新 device 模块文档"
# 更新：~/cann/runtime-doc/modules/device/device.md
```
