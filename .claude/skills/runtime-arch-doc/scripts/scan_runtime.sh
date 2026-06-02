#!/bin/bash
# Runtime 源码结构扫描脚本
# 用于快速分析 runtime 目录结构

set -e

RUNTIME_SRC="${1: runtime/src/runtime/}"
OUTPUT="${2:-runtime_structure.txt}"

echo "=== Runtime 源码结构扫描 ===" > "$OUTPUT"
echo "源码路径: $RUNTIME_SRC" >> "$OUTPUT"
echo "扫描时间: $(date)" >> "$OUTPUT"
echo "" >> "$OUTPUT"

# 目录结构
echo "## 目录结构" >> "$OUTPUT"
find "$RUNTIME_SRC" -type d | sort >> "$OUTPUT"
echo "" >> "$OUTPUT"

# 头文件统计
echo "## 头文件 (.hpp/.h)" >> "$OUTPUT"
find "$RUNTIME_SRC" -name "*.hpp" -o -name "*.h" | wc -l >> "$OUTPUT"
echo "" >> "$OUTPUT"

# 源文件统计
echo "## 源文件 (.cc/.cpp)" >> "$OUTPUT"
find "$RUNTIME_SRC" -name "*.cc" -o -name "*.cpp" | wc -l >> "$OUTPUT"
echo "" >> "$OUTPUT"

# 各模块文件数
echo "## 各模块文件统计" >> "$OUTPUT"
for dir in device stream task context memory kernel event engine; do
    if [ -d "$RUNTIME_SRC/$dir" ]; then
        count=$(find "$RUNTIME_SRC/$dir" -name "*.cc" -o -name "*.hpp" -o -name "*.h" | wc -l)
        echo "$dir: $count 文件" >> "$OUTPUT"
    fi
done

echo "" >> "$OUTPUT"
echo "## 关键类定义" >> "$OUTPUT"
grep -r "^class " "$RUNTIME_SRC" --include="*.hpp" --include="*.h" | head -30 >> "$OUTPUT"

echo "扫描完成，输出: $OUTPUT"
cat "$OUTPUT"