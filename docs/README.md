<!-- ==================== HERO 区域 ==================== -->

<h1 align="center">CANN Runtime</h1>

<p align="center">
  <b>昇腾 AI 处理器轻量级运行时环境 · 端云一致 · 高性能推理</b><br>
  CANN Runtime 是昇腾 AI 处理器的核心运行时底座，它通过提供统一的API，使得上层应用、AI框架、加速库能够高效利用AI处理器的硬件计算资源。
</p>

---

<!-- ==================== 文档导航 ==================== -->
## 📚 文档导航

<table width="100%" style="table-layout: fixed; width: 100%; border-collapse: collapse;">
<colgroup>
<col width="15%">
<col width="55%">
<col width="30%">
</colgroup>
<thead>
<tr>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">文档</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">定位与内容</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">入口</th>
</tr>
</thead>
<tbody>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>⚡ 快速入门</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">零基础起步，第一次接触 Runtime 的开发者。Runtime简介 + 编程模型讲解，建议顺序阅读</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="01_quick_start/Runtime简介.md">Runtime 简介</a> · <a href="01_quick_start/Runtime编程模型.md">编程模型</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>📖 编程指南</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">深度开发手册，已跑通入门阶段 Hello CANN 后需要深入理解原理的开发者。原理解析 + 示例代码</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="02_dev_guide/00_dev_guide.md">编程指南</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>📋 API 参考</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">接口字典，开发过程中随时查阅函数定义。函数签名 + 参数说明 + 返回值</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="03_api_ref/01_概述.md">头文件说明</a> · <a href="03_api_ref/api_ref.md">API 参考</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>🏗️ 架构指南</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">面向贡献者的架构文档。Runtime整体架构、模块设计、核心组件解析</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="design/README.md">架构指南</a></td>
</tr>
</tbody>
</table>

---

<!-- ==================== 成长地图 ==================== -->
## 🗺️ 成长地图

按以下路径循序渐进，从环境准备到独立开发：

### 🌱 入门阶段

<table width="100%" style="table-layout: fixed; width: 100%; border-collapse: collapse;">
<colgroup>
<col width="15%">
<col width="55%">
<col width="30%">
</colgroup>
<thead>
<tr>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">学习步骤</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">内容说明</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">入口</th>
</tr>
</thead>
<tbody>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>环境准备</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">CANN 一键安装</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="https://www.hiascend.com/cann/download">CANN 一键安装</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>概念原理</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">Runtime 核心概念与编程模型：Host-Device 架构、Context、Stream、同步异步、典型执行流程</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="01_quick_start/Runtime简介.md">Runtime 简介</a> · <a href="01_quick_start/Runtime编程模型.md">编程模型</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>Hello CANN</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">第一个可运行的 Runtime 程序，完成最小计算闭环</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="../example/0_quickstart/0_hello_cann/README.md">Hello CANN</a></td>
</tr>
</tbody>
</table>

---

### 🚀 进阶阶段

<table width="100%" style="table-layout: fixed; width: 100%; border-collapse: collapse;">
<colgroup>
<col width="15%">
<col width="55%">
<col width="30%">
</colgroup>
<thead>
<tr>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">主题</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">核心内容</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">对应样例</th>
</tr>
</thead>
<tbody>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>初始化</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">包括环境初始化、设备资源配置、日志管理等</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="../example/1_basic_features/device/0_device_normal/README.md">device_normal</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>内存管理</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">包括Host内存管理、Device内存管理、多流同步内存、内存拷贝（同步/异步）、物理内存共享(pid)等</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="../example/1_basic_features/memory/1_h2d_sync_memory_copy/README.md">h2d_sync_memory_copy</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>异步任务</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">包括Stream管理、Event管理、Kernel加载与执行、内存语义同步等</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="../example/1_basic_features/stream/0_simple_stream/README.md">simple_stream</a></td>
</tr>
</tbody>
</table>

---

### 🔥 高级阶段

<table width="100%" style="table-layout: fixed; width: 100%; border-collapse: collapse;">
<colgroup>
<col width="15%">
<col width="55%">
<col width="30%">
</colgroup>
<thead>
<tr>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">主题</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">核心内容</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">对应样例</th>
</tr>
</thead>
<tbody>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>ACL Graph</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">包括单流捕获、跨流捕获、任务更新等</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="../example/2_advanced_features/model_ri/1_model_update/README.md">model_update</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>多设备编程</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">包括跨Device数据交互、P2P内存访问、多卡并行调度等</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="../example/1_basic_features/device/2_device_P2P/README.md">device_P2P</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>进程间通信</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">包括IPC Event同步、IPC内存共享（指定PID/不指定PID）等</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="../example/2_advanced_features/ipcevent/0_ipcevent/README.md">ipc_event</a> · <a href="../example/1_basic_features/memory/11_ipc_memory_withoutpid/README.md">ipc_memory</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>性能调优</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">包括Profiling采集并落盘、获取网络模型中算子的性能数据、可视化展示原始性能数据解析结果等</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="../example/5_performance/profiling/0_create_config/README.md">create_config</a></td>
</tr>
</tbody>
</table>

---

<!-- ==================== 常见问题 ==================== -->
## ❓ 常见问题

<table width="100%" style="table-layout: fixed; width: 100%; border-collapse: collapse;">
<colgroup>
<col width="15%">
<col width="55%">
<col width="30%">
</colgroup>
<thead>
<tr>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">问题类型</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">典型场景</th>
<th align="left" style="word-wrap: break-word; overflow-wrap: break-word;">入口</th>
</tr>
</thead>
<tbody>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>进程重启失败</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">进程重启失败、资源申请不到</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="04_FAQ/用户进程异常退出后重启进程失败.md">用户进程异常退出后重启进程失败</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>内核版本不兼容</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">算子输出全为0、内核版本不兼容</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="04_FAQ/低版本内核使用asan导致算子执行失败.md">低版本内核使用asan导致算子执行失败</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>休眠唤醒失败</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">休眠失败、hwts busy</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="04_FAQ/AI应用进程未退出导致休眠唤醒失败.md">AI应用进程未退出导致休眠唤醒失败</a></td>
</tr>
<tr>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><b>析构崩溃</b></td>
<td style="word-wrap: break-word; overflow-wrap: break-word;">应用程序运行过程中出现coredump导致了崩溃，应用程序异常终止</td>
<td style="word-wrap: break-word; overflow-wrap: break-word;"><a href="04_FAQ/析构函数中调用aclFinalize导致应用进程coredump.md">析构函数中调用aclFinalize导致应用进程coredump</a></td>
</tr>
</tbody>
</table>

---

<!-- ==================== 底部链接 ==================== -->
<p align="center">
  <b>相关资源</b><br>
  <a href="https://www.hiascend.com">昇腾社区</a> ·
  <a href="../example/README.md">Runtime 样例仓库</a> ·
  <a href="https://www.hiascend.com/document">完整文档中心</a>
</p>

<p align="center">
  <sub>CANN Runtime 文档 | 华为昇腾</sub>
</p>