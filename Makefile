# options: x86 arm
TOOLCHAIN_TARGET    ?= arm

# options: see tool/create_builddir
GENODE_TARGET       ?= foc_pbxa9

BUILD_DIR           ?= build
TOOLCHAIN_BUILD_DIR ?= $(BUILD_DIR)/toolchain-$(TOOLCHAIN_TARGET)
GENODE_BUILD_DIR    ?= $(BUILD_DIR)/genode-$(GENODE_TARGET)
BUILD_CONF           = $(GENODE_BUILD_DIR)/etc/build.conf

all: tms descs toolchain ports platform


# ================================================================
# TMS tool required for task descriptions.
tms:
	@mkdir -p tms-sim-2014-12/build
	@cd tms-sim-2014-12/build && ../build-local.sh .. && $(MAKE) && $(MAKE) install

descs:
	@tms-sim-2014-12/build/bin/generator -o dom0-client/tasks.xml -n 5
#
# ================================================================


# ================================================================
# Genode toolchain. Only needs to be done once per target (x86/arm).
toolchain:
	mkdir -p $(TOOLCHAIN_BUILD_DIR)
	cd $(TOOLCHAIN_BUILD_DIR);\
	../../tool/tool_chain $(TOOLCHAIN_TARGET)
#
# ================================================================


# ================================================================
# Download Genode external sources. Only needs to be done once per system.
ports: foc libports

foc:
	$(MAKE) -C repos/base-foc prepare

libports:
	$(MAKE) -C repos/libports prepare
#
# ================================================================


# ================================================================
# Genode build process. Rebuild subtargets as needed.
platform: genode_build_dir tasks dom0

genode_build_dir:
	tool/create_builddir $(GENODE_TARGET) BUILD_DIR=$(GENODE_BUILD_DIR)
	printf 'REPOSITORIES += $$(GENODE_DIR)/repos/libports\n' >> $(BUILD_CONF)
	printf 'REPOSITORIES += $$(GENODE_DIR)/repos/dom0\n' >> $(BUILD_CONF)

tasks: hey idle namaste tumatmul

hey:
	$(MAKE) -j10 -C $(GENODE_BUILD_DIR) dom0/hey
	cp $(GENODE_BUILD_DIR)/dom0/hey/hey dom0-client/

idle:
	$(MAKE) -j10 -C $(GENODE_BUILD_DIR) dom0/idle
	cp $(GENODE_BUILD_DIR)/dom0/idle/idle dom0-client/

namaste:
	$(MAKE) -j10 -C $(GENODE_BUILD_DIR) dom0/namaste
	cp $(GENODE_BUILD_DIR)/dom0/namaste/namaste dom0-client/

tumatmul:
	$(MAKE) -j10 -C $(GENODE_BUILD_DIR) dom0/tumatmul
	cp $(GENODE_BUILD_DIR)/dom0/tumatmul/tumatmul dom0-client/

dom0:
	$(MAKE) -j10 -C $(GENODE_BUILD_DIR) dom0/server

task-manager:
	$(MAKE) -j10 -C $(GENODE_BUILD_DIR) dom0/task-manager

# Delete build directory for all target systems. In some cases, subfolders in the contrib directory might be corrupted. Remove manually and re-prepare if necessary.
clean:
	rm -rf $(BUILD_DIR)
#
# ================================================================


# ================================================================
# Run Genode with an active dom0 server.
run:
	$(MAKE) -j10 -C $(GENODE_BUILD_DIR) run/dom0
#
# ================================================================


# ================================================================
# VDE setup. Do once per system session. DHCP is optional.
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

# Cleanup network shenanigans.
clean-network: dhcp-stop vde-stop
#
# ================================================================
