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

.DEFAULT_GOAL := all

# -------------------------------
# gRPC 安装路径
# -------------------------------
GRPC_INSTALL_PATH := /home/jinjiabin/drivers/grpc
GRPC_ROOT := $(GRPC_INSTALL_PATH)/install_arm
GRPC_HOST := $(GRPC_INSTALL_PATH)/install_host

# protoc（仅host）
PROTOC := $(GRPC_HOST)/bin/protoc
GRPC_CPP_PLUGIN := $(GRPC_HOST)/bin/grpc_cpp_plugin
GRPC_PY_PLUGIN := $(GRPC_HOST)/bin/grpc_python_plugin

# proto输出
OUT_DIR := ./proto_cc_gen
PY_OUT_DIR := ./proto_py_gen
PROTO_DIR := ./proto
PROTO_FILES := $(wildcard $(PROTO_DIR)/*.proto)

# -------------------------------
# 交叉编译工具链
# -------------------------------
CC  = aarch64-amd-linux-gcc
CXX = aarch64-amd-linux-g++

SYSROOT = /opt/petalinux/2025.2/aarch64/sysroots/cortexa72-cortexa53-amd-linux

# -------------------------------
# 编译选项
# -------------------------------
MODE ?= release

ifeq ($(MODE), debug)
    OPT_FLAGS = -O0 -g
else
    OPT_FLAGS = -O2
endif

CFLAGS = --sysroot=$(SYSROOT) -Wall -Wextra $(OPT_FLAGS) \
         -I$(INCLUDE_SCPI) -I$(INCLUDE_TIRPC) \
         -I$(GRPC_ROOT)/include \
         -MMD -MF $(@:.o=.d)

CXXFLAGS = --sysroot=$(SYSROOT) -Wall -Wextra -std=c++17 $(OPT_FLAGS) \
           -I$(INCLUDE_SCPI) -I$(INCLUDE_TIRPC) \
           -I$(GRPC_ROOT)/include \
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

# -------------------------------
# linker
# -------------------------------
LDFLAGS += --sysroot=$(SYSROOT)
LDFLAGS += -L$(GRPC_ROOT)/lib

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
# build
# -------------------------------
all: $(BIN_DIR)/$(PROJECT) copy_file
	@echo "✅ Build Success"

$(BIN_DIR)/$(PROJECT): $(OBJS) | $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS)

# -------------------------------
# copy json
# -------------------------------
copy_file: $(JSON_FILE) | $(BIN_DIR)
	@if [ -f "$<" ]; then \
		cp -f $< $(BIN_DIR)/; \
	fi

# -------------------------------
# proto
# -------------------------------
proto: $(PROTO_FILES) | $(OUT_DIR) $(PY_OUT_DIR)
	@for proto in $^; do \
		$(PROTOC) -I$(PROTO_DIR) \
			--cpp_out=$(OUT_DIR) \
			--grpc_out=$(OUT_DIR) \
			--plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN) \
			$$proto; \
		$(PROTOC) -I$(PROTO_DIR) \
			--python_out=$(PY_OUT_DIR) \
			--grpc_out=$(PY_OUT_DIR) \
			--plugin=protoc-gen-grpc=$(GRPC_PY_PLUGIN) \
			$$proto; \
	done

proto_clean:
	rm -rf $(OUT_DIR) $(PY_OUT_DIR)

# -------------------------------
# clean
# -------------------------------
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
