# options: x86 arm
TOOLCHAIN_TARGET    ?= arm

# options: see tool/create_builddir
GENODE_TARGET       ?= foc_pbxa9

BUILD_DIR           ?= build
TOOLCHAIN_BUILD_DIR ?= $(BUILD_DIR)/toolchain-$(TOOLCHAIN_TARGET)
GENODE_BUILD_DIR    ?= $(BUILD_DIR)/genode-$(GENODE_TARGET)
BUILD_CONF           = $(GENODE_BUILD_DIR)/etc/build.conf

.PHONY: all toolchain ports foc libports dom0 genode_build_dir clean vde

all: toolchain ports genode_build_dir dom0 platform tasks

platform: genode_build_dir dom0

tasks: hey namaste tumatmul

hey:
	$(MAKE) -C $(GENODE_BUILD_DIR) dom0/hey
	cp $(GENODE_BUILD_DIR)/dom0/hey/hey dom0-client/

namaste:
	$(MAKE) -C $(GENODE_BUILD_DIR) dom0/namaste
	cp $(GENODE_BUILD_DIR)/dom0/namaste/namaste dom0-client/

tumatmul:
	$(MAKE) -C $(GENODE_BUILD_DIR) dom0/tumatmul
	cp $(GENODE_BUILD_DIR)/dom0/tumatmul/tumatmul dom0-client/

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

dom0:
	$(MAKE) -j10 -C $(GENODE_BUILD_DIR) run/dom0

vde: vde-stop
	@vde_switch -d -s /tmp/switch1
	@sudo vde_tunctl -u $(USER) -t tap0
	@sudo ifconfig tap0 192.168.0.254 up
	@sudo route add -host 192.168.0.14 dev tap0
	@vde_plug2tap --daemon -s /tmp/switch1 tap0

vde-stop:
	@-pkill vde_switch
	@-sudo vde_tunctl -d tap0
	@-rm -rf /tmp/switch1

dhcp: dhcp-stop
	@slirpvde -d -s /tmp/switch1 -dhcp

dhcp-stop:
	@-pkill slirpvde

clean-network: dhcp-stop vde-stop

clean:
	rm -rf $(BUILD_DIR)
