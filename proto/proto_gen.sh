#!/bin/bash

# ===================== 配置路径 =====================
PROTOC="/home/huangwei/drivers/grpc-1.54.0/grpc/install_host/bin/protoc"
GRPC_CPP_PLUGIN="/home/huangwei/drivers/grpc-1.54.0/grpc/install_host/bin/grpc_cpp_plugin"
GRPC_PY_PLUGIN="/home/huangwei/drivers/grpc-1.54.0/grpc/install_host/bin/grpc_python_plugin"
OUT_DIR="../proto_gen"
PY_OUT_DIR="../py_proto_gen"
PROTO_DIR="."
# ====================================================

# 备份旧文件
if [ -d "${OUT_DIR}" ]; then
    echo "📦 备份旧文件..."
    mv "${OUT_DIR}" "${OUT_DIR}.bak"
fi

# 创建输出目录
mkdir -p "${OUT_DIR}"
mkdir -p "${PY_OUT_DIR}"

# 遍历编译 proto
PROTO_FILES=$(find "${PROTO_DIR}" -maxdepth 1 -name "*.proto" | sort)

for proto in ${PROTO_FILES}; do
    echo "→ 编译 C++: $proto"
    ${PROTOC} -I"${PROTO_DIR}" \
        --cpp_out="${OUT_DIR}" \
        --grpc_out="${OUT_DIR}" \
        --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}" \
        "${proto}"

    echo "→ 编译 Python: $proto"
    ${PROTOC} -I"${PROTO_DIR}" \
        --python_out="${PY_OUT_DIR}" \
        --grpc_out="${PY_OUT_DIR}" \
        --plugin=protoc-gen-grpc="${GRPC_PY_PLUGIN}" \
        "${proto}"
done

# 删除备份
rm -rf "${OUT_DIR}.bak"

echo -e "\n✅ C++ 和 Python 全部编译完成！"
