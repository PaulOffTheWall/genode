# options: x86 arm
TOOLCHAIN_TARGET    ?= arm

# options: see tool/create_builddir
GENODE_TARGET       ?= foc_pbxa9

BUILD_DIR           ?= build
TOOLCHAIN_BUILD_DIR ?= $(BUILD_DIR)/toolchain-$(TOOLCHAIN_TARGET)
GENODE_BUILD_DIR    ?= $(BUILD_DIR)/genode-$(GENODE_TARGET)
BUILD_CONF           = $(GENODE_BUILD_DIR)/etc/build.conf

.PHONY: all toolchain ports foc libports genode genode_build_dir clean

all: toolchain ports genode_build_dir genode

toolchain:
	mkdir -p $(TOOLCHAIN_BUILD_DIR)
	cd $(TOOLCHAIN_BUILD_DIR);\
	../../tool/tool_chain $(TOOLCHAIN_TARGET)

ports: foc libports

foc:
	$(MAKE) -C repos/base-foc prepare

libports:
	$(MAKE) -C repos/libports prepare

genode_build_dir:
	tool/create_builddir $(GENODE_TARGET) BUILD_DIR=$(GENODE_BUILD_DIR)
	printf 'REPOSITORIES += $$(GENODE_DIR)/repos/libports\n' >> $(BUILD_CONF)
	printf 'REPOSITORIES += $$(GENODE_DIR)/repos/dom0\n' >> $(BUILD_CONF)

genode:
	$(MAKE) -j10 -C $(GENODE_BUILD_DIR) run/dom0

clean:
	rm -rf $(BUILD_DIR)
