# -------------------------------
# 工程配置
# -------------------------------
PROJECT = silicon_based
SRC_DIR = ./
INCLUDE_SCPI = scpi
INCLUDE_TIRPC = tirpc
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/app

# 显式指定默认目标
.DEFAULT_GOAL := all

# -------------------------------
# Proto 编译配置
# -------------------------------
GRPC_DRIVER := /home/jinjiabin/drivers/grpc-v1.71.0/grpc
PROTOC := $(GRPC_DRIVER)/install_host/bin/protoc
GRPC_CPP_PLUGIN := $(GRPC_DRIVER)/install_host/bin/grpc_cpp_plugin
GRPC_PY_PLUGIN := $(GRPC_DRIVER)/install_host/bin/grpc_python_plugin
OUT_DIR := ./proto_cc_gen
PY_OUT_DIR := ./proto_py_gen
PROTO_DIR := ./proto
PROTO_FILES := $(wildcard $(PROTO_DIR)/*.proto)

# -------------------------------
# gRPC 1.54.0 配置
# -------------------------------
GRPC_AARCH64_ROOT = $(GRPC_DRIVER)/install_arm
INCLUDE_GRPC = $(GRPC_AARCH64_ROOT)/include
LIB_GRPC = $(GRPC_AARCH64_ROOT)/lib

# -------------------------------
# 交叉编译器
# -------------------------------
CC  = aarch64-amd-linux-gcc
CXX = aarch64-amd-linux-g++
SYSROOT = /opt/petalinux/2025.2/aarch64/sysroots/cortexa72-cortexa53-amd-linux

# -------------------------------
# 编译选项
# -------------------------------
CFLAGS   = --sysroot=$(SYSROOT) -Wall -Wextra -O2 \
           -I$(INCLUDE_SCPI) -I$(INCLUDE_TIRPC) -I$(INCLUDE_GRPC) \
           -MMD -MF $(@:.o=.d)

# gRPC 1.54.0 要求 C++17，必须显式指定
CXXFLAGS = --sysroot=$(SYSROOT) -Wall -Wextra -std=c++17 -O2 \
           -I$(INCLUDE_SCPI) -I$(INCLUDE_TIRPC) -I$(INCLUDE_GRPC) \
           -MMD -MF $(@:.o=.d)

# -------------------------------
# 源文件
# -------------------------------
C_SRCS   = $(shell find $(SRC_DIR) -name '*.c' -not -path '*/$(BUILD_DIR)/*' | sed 's|^\./||')
CPP_SRCS = $(shell find $(SRC_DIR) -name '*.cpp' -not -path '*/$(BUILD_DIR)/*' | sed 's|^\./||')
CPP_SRCS += $(shell find $(SRC_DIR) -name '*.cc' -not -path '*/$(BUILD_DIR)/*' | sed 's|^\./||')

C_OBJS   = $(C_SRCS:%.c=$(OBJ_DIR)/%.o)
CPP_OBJS = $(CPP_SRCS:%.cpp=$(OBJ_DIR)/%.o)
CPP_OBJS := $(CPP_OBJS:%.cc=$(OBJ_DIR)/%.o)

OBJS = $(C_OBJS) $(CPP_OBJS)
DEPS = $(OBJS:.o=.d)

JSON_FILE := param_mgr/DeviceInfo.json

LDFLAGS  += --sysroot=$(SYSROOT)
LDFLAGS  += -L$(LIB_GRPC)
LDFLAGS  += -L$(SYSROOT)/usr/lib

# --------------------------
# LINK FLAGS (FIXED VERSION)
# --------------------------
LDFLAGS  += --sysroot=$(SYSROOT)
LDFLAGS  += -L$(LIB_GRPC)
LDFLAGS  += -L$(SYSROOT)/usr/lib
LDFLAGS  += -L$(GRPC_AARCH64_ROOT)/lib

GRPC_STATIC_LIBS := $(wildcard $(LIB_GRPC)/libgrpc++.a)
GRPC_STATIC_LIBS += $(wildcard $(LIB_GRPC)/libgrpc.a)
GRPC_STATIC_LIBS += $(wildcard $(LIB_GRPC)/libgpr.a)

PROTOBUF_STATIC_LIBS := $(wildcard $(LIB_GRPC)/libprotobuf.a)
ABSL_STATIC_LIBS := $(wildcard $(LIB_GRPC)/libabsl_*.a)
UPB_STATIC_LIBS := $(wildcard $(LIB_GRPC)/libupb*.a)
UTF8_STATIC_LIBS := $(wildcard $(LIB_GRPC)/libutf8*.a)

OTHER_GRPC_DEPS := $(wildcard $(LIB_GRPC)/libaddress_sorting.a)
OTHER_GRPC_DEPS += $(wildcard $(LIB_GRPC)/libre2.a)
OTHER_GRPC_DEPS += $(wildcard $(LIB_GRPC)/libcares.a)

SYS_LIBS := -lssl -lcrypto -lz -lpthread -ldl -lm

LDFLAGS += -Wl,--start-group
LDFLAGS += $(GRPC_STATIC_LIBS)
LDFLAGS += $(PROTOBUF_STATIC_LIBS)
LDFLAGS += $(UPB_STATIC_LIBS)
LDFLAGS += $(UTF8_STATIC_LIBS)
LDFLAGS += $(OTHER_GRPC_DEPS)
LDFLAGS += $(ABSL_STATIC_LIBS)
LDFLAGS += $(SYS_LIBS)
LDFLAGS += -Wl,--end-group

# -------------------------------
# 编译规则
# -------------------------------
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.cc | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# -------------------------------
# 目录
# -------------------------------
$(OBJ_DIR) $(BIN_DIR) $(OUT_DIR) $(PY_OUT_DIR):
	mkdir -p $@

# -------------------------------
# 默认目标
# -------------------------------
.PHONY: all
all: $(BIN_DIR)/$(PROJECT) copy_file
	@echo "✅ Compile succeed !"

$(BIN_DIR)/$(PROJECT): $(OBJS) | $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS)

# -------------------------------
# 拷贝 JSON
# -------------------------------
.PHONY: copy_file
copy_file: $(JSON_FILE) | $(BIN_DIR)
	@if [ -f "$<" ]; then \
		cp -f $< $(BIN_DIR)/; \
		echo "$< copied to $(BIN_DIR)"; \
	else \
		echo "Warning: $< not found, skipping copy"; \
	fi

# -------------------------------
# Proto 编译目标
# -------------------------------
.PHONY: proto
proto: $(PROTO_FILES) | $(OUT_DIR) $(PY_OUT_DIR)
	@for proto in $^; do \
		echo "→ 编译 C++: $$proto"; \
		$(PROTOC) -I$(PROTO_DIR) \
			--cpp_out=$(OUT_DIR) \
			--grpc_out=$(OUT_DIR) \
			--plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN) \
			$$proto; \
		echo "→ 编译 Python: $$proto"; \
		$(PROTOC) -I$(PROTO_DIR) \
			--python_out=$(PY_OUT_DIR) \
			--grpc_out=$(PY_OUT_DIR) \
			--plugin=protoc-gen-grpc=$(GRPC_PY_PLUGIN) \
			$$proto; \
	done
	@echo -e "\n✅ 所有 proto 文件编译完成！"

.PHONY: proto_clean
proto_clean:
	rm -rf $(OUT_DIR) $(PY_OUT_DIR)

# -------------------------------
# 清理
# -------------------------------
.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)
	@echo "✅ Clean succeed !"

# 包含依赖文件
-include $(DEPS)
