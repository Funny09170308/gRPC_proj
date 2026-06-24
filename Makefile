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
PROTOC := /home/huangwei/drivers/grpc-1.54.0/grpc/install_host/bin/protoc
GRPC_CPP_PLUGIN := /home/huangwei/drivers/grpc-1.54.0/grpc/install_host/bin/grpc_cpp_plugin
GRPC_PY_PLUGIN := /home/huangwei/drivers/grpc-1.54.0/grpc/install_host/bin/grpc_python_plugin
OUT_DIR := ./proto_cc_gen
PY_OUT_DIR := ./proto_py_gen
PROTO_DIR := ./proto
PROTO_FILES := $(wildcard $(PROTO_DIR)/*.proto)

# -------------------------------
# gRPC 1.54.0 配置
# -------------------------------
GRPC_ROOT = /home/huangwei/drivers/grpc-1.54.0/grpc/install_arm
INCLUDE_GRPC = $(GRPC_ROOT)/include
LIB_GRPC = $(GRPC_ROOT)/lib

# -------------------------------
# 交叉编译器
# -------------------------------
CC  = aarch64-xilinx-linux-gcc
CXX = aarch64-xilinx-linux-g++
SYSROOT = /opt/petalinux/2020.1/sysroots/aarch64-xilinx-linux

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

# 使用 --start-group/--end-group 处理所有循环依赖
LDFLAGS += -Wl,--start-group
# --------------------------
# 1. gRPC 核心库
# --------------------------
LDFLAGS += -lgrpc++ -lgrpc -lgpr -lupb -laddress_sorting -lre2 -lcares
# --------------------------
# 2. Protobuf 库
# --------------------------
LDFLAGS += -lprotobuf
# --------------------------
# 3. Abseil 库
# --------------------------
# 随机数相关库
LDFLAGS += -labsl_random_distributions -labsl_random_seed_sequences -labsl_random_seed_gen_exception -labsl_random_internal_seed_material -labsl_random_internal_pool_urbg -labsl_random_internal_randen -labsl_random_internal_randen_hwaes -labsl_random_internal_randen_hwaes_impl -labsl_random_internal_randen_slow -labsl_random_internal_platform -labsl_random_internal_distribution_test_util
# 性能分析相关库
LDFLAGS += -labsl_exponential_biased -labsl_periodic_sampler -labsl_graphcycles_internal -labsl_hashtablez_sampler
# Flags 相关库
LDFLAGS += -labsl_flags -labsl_flags_internal -labsl_flags_reflection -labsl_flags_config -labsl_flags_program_name -labsl_flags_marshalling -labsl_flags_parse -labsl_flags_commandlineflag -labsl_flags_commandlineflag_internal -labsl_flags_private_handle_accessor -labsl_flags_usage -labsl_flags_usage_internal
# Logging 相关库
LDFLAGS += -labsl_log_entry -labsl_log_flags -labsl_log_globals -labsl_log_initialize -labsl_log_internal_check_op -labsl_log_internal_conditions -labsl_log_internal_format -labsl_log_internal_globals -labsl_log_internal_log_sink_set -labsl_log_internal_message -labsl_log_internal_nullguard -labsl_log_internal_proto -labsl_log_severity -labsl_log_sink -labsl_raw_logging_internal
# CRC 相关库
LDFLAGS += -labsl_crc32c -labsl_crc_cord_state -labsl_crc_internal -labsl_crc_cpu_detect
# Cord 相关库
LDFLAGS += -labsl_cord -labsl_cord_internal -labsl_cordz_functions -labsl_cordz_info -labsl_cordz_handle -labsl_cordz_sample_token
# Strings 相关库
LDFLAGS += -labsl_strings -labsl_strings_internal -labsl_str_format_internal -labsl_strerror
# Status 相关库
LDFLAGS += -labsl_status -labsl_statusor
# 基础类型库
LDFLAGS += -labsl_bad_variant_access -labsl_bad_optional_access -labsl_bad_any_cast_impl
# Time 相关库
LDFLAGS += -labsl_time -labsl_time_zone -labsl_civil_time
# 同步相关库
LDFLAGS += -labsl_synchronization -labsl_stacktrace -labsl_symbolize -labsl_debugging_internal -labsl_demangle_internal -labsl_malloc_internal -labsl_failure_signal_handler -labsl_leak_check -labsl_examine_stack
# 基础
LDFLAGS += -labsl_spinlock_wait -labsl_base
# Hash 相关库
LDFLAGS += -labsl_hash -labsl_city -labsl_low_level_hash -labsl_raw_hash_set
# 工具库
LDFLAGS += -labsl_int128 -labsl_throw_delegate -labsl_die_if_null -labsl_scoped_set_env
# --------------------------
# 4. SSL 库
# --------------------------
LDFLAGS += -lssl -lcrypto

# --------------------------
# 5. 系统库
# --------------------------
LDFLAGS += -lpthread -ldl -lz -lm

# 结束循环依赖组
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
