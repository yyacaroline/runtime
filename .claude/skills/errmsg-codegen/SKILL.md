# 错误码自动生成 Skill

## 触发场景

用户说「新增错误码 EE1021」「自动生成错误码代码」→ 自动生成 X-Macro 表行 + UT 测试数据。

## 输入

用户提供一个错误码编号，如 `EE1021`。

### Step 1: 查找 JSON

```bash
grep -A10 "\"ErrCode\": \"EE1021\"" src/dfx/error_manager/error_code.json src/conf/error_manager/error_code.json 2>/dev/null
```

如果找不到 → 提示用户先在 `error_code.json` 中补充该错误码定义，然后重试。

### Step 2: 解析 JSON 条目

从 JSON 提取：

| JSON 字段 | 用途 | 示例 |
|-----------|------|------|
| `ErrCode` | 枚举名、字符串名 | `EE1021` |
| `ErrMessage` | 消息模板（不含 `ErrorCode=` 后缀） | `The argument is invalid.Reason: %s` |
| `Arglist` | 参数名列表（逗号分隔） | `extend_info` 或 `func,value,param,expect` |

### Step 3: 推导日志级别

- `ErrCode` 以 `W` 开头 → `DLOG_WARN`
- 其他 → `DLOG_ERROR`

### Step 4: 生成 X-Macro 表行

生成一行代码，格式：

```cpp
/* EEXXXX - ErrTitle */                                                       \
X(EEXXXX, "EEXXXX",                                                           \
  ("param1", "param2", ...),                                                  \
  "ErrMessage. ErrorCode=EEXXXX.\n",                                          \
  DLOG_ERROR)
```

**规则**：
- `Arglist` 中逗号分隔的参数名，每个用 `"param"` 包裹，整体用 `()` 包裹
- `ErrMessage` 末尾追加 `. ErrorCode=EEXXXX.\n`
- 如果 `ErrMessage` 末尾已有句号，不加额外句号

### Step 5: 插入表

在 `src/runtime/core/inc/common/error_code_meta.h` 的 `RUNTIME_ERROR_CODE_TABLE` 宏中，按错误码编号顺序插入到合适位置。

### Step 6: 更新 UT 数据

在 `tests/ut/runtime/runtime/test/rt_error_code_test.cc` 的 `ErrorCodeTableParamCountMatchesMessageFormat` 测试的 `allCodes` 数组中追加一行：

```cpp
{ErrorCode::EEXXXX, N},  // N = Arglist 中的参数个数
```

### Step 7: 验证

```bash
make -C build runtime_utest -j4 && ./build/tests/ut/runtime/runtime/runtime_utest --gtest_filter='*ErrorCodeTable*'
```

## 完整示例

用户输入「新增错误码 EE1021」：

**JSON 中**
```json
{
  "ErrCode": "EE1021",
  "ErrMessage": "Failed to process request. Reason: %s",
  "Arglist": "reason",
  "suggestion": { ... }
}
```

**生成代码插入 X-Macro 表**
```cpp
    /* EE1021 - Some_Error */                                                 \
    X(EE1021, "EE1021",                                                       \
      ("reason"),                                                             \
      "Failed to process request. Reason: %s. ErrorCode=EE1021.\n",           \
      DLOG_ERROR)
```

**更新 UT**
```cpp
{ErrorCode::EE1021, 1},
```
