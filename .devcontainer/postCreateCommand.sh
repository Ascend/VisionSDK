#!/bin/bash
# .devcontainer/postCreateCommand.sh
# VisionSDK Dev Container 初始化脚本

set -e

echo "=========================================="
echo "VisionSDK Dev Container post-create setup..."
echo "=========================================="

# 1. 验证 NPU 环境
if command -v npu-smi &> /dev/null; then
    echo "NPU environment detected:"
    npu-smi info || true
else
    echo "Warning: npu-smi not found. NPU may not be available."
fi

# 2. 验证 Ascend Toolkit
if [ -f "/usr/local/Ascend/ascend-toolkit/set_env.sh" ]; then
    echo "Ascend Toolkit detected at /usr/local/Ascend/ascend-toolkit"
else
    echo "Warning: Ascend Toolkit not found."
fi

echo "=========================================="
echo "VisionSDK Dev Container setup completed!"
echo "Run 'bash build_all.sh <system> <platform> <version> notest' to build."
echo "=========================================="
