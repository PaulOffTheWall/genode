# options: x86 arm
TOOLCHAIN_TARGET    ?= x86

# options: see tool/create_builddir
GENODE_TARGET       ?= foc_x86_32

BUILD_DIR           ?= build
TOOLCHAIN_BUILD_DIR ?= $(BUILD_DIR)/toolchain-$(TOOLCHAIN_TARGET)
GENODE_BUILD_DIR    ?= $(BUILD_DIR)/genode-$(GENODE_TARGET)
BUILD_CONF           = $(GENODE_BUILD_DIR)/etc/build.conf

.PHONY: all toolchain foc genode genode_build_dir clean

all: toolchain foc genode_build_dir genode

toolchain:
	mkdir -p $(TOOLCHAIN_BUILD_DIR)
	cd $(TOOLCHAIN_BUILD_DIR);\
	../../tool/tool_chain $(TOOLCHAIN_TARGET)

foc:
	$(MAKE) -C repos/base-foc prepare

genode_build_dir:
	tool/create_builddir $(GENODE_TARGET) BUILD_DIR=$(GENODE_BUILD_DIR)
	printf '\n' >> $(BUILD_CONF)
	printf 'REPOSITORIES += $$(GENODE_DIR)/repos/dom0\n' >> $(BUILD_CONF)

genode:
	$(MAKE) -j10 -C ${GENODE_BUILD_DIR}

clean:
	rm -rf $(BUILD_DIR)