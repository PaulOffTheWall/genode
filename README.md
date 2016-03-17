# genode-dom0
dom0 is a network module enabling the dynamic management of tasks over TCP/IP. The dom0 server accepts binaries and task descriptions over the network which can then be started by the dom0 task manager. The dom0 module also provides event-driven profiling information on all processes running on Genode, with more detailed information on tasks started by the task manager.

### Building
The custom Makefile builds the entire Fiasco.OC and Genode system including its toolchain and external dependencies into a `build/` and `contrib/` directory respectively. To manually execute steps within the build process see the Makefile or the Genode documentation:
http://genode.org/documentation/developer-resources/getting_started

The toolchain installs executables to `/usr/local/genode-gcc` as part of the Genode `tool_chain` script. Everything else stays within the repository.

After a first clone of this branch run `make` once. This will do the following:

* build tms-sim tool (target `tms`)
* generate tms-sim xml task descriptions (target `descs`)
* build and install Genode toolchain for `arm` or `x86` (target `toolchain`)
* download required external libraries into the `contrib` folder (target `ports`)
* create the target-specific build directory (target `genode_build_dir`)
* build task binaries (target `tasks`)
* build dom0 (target `dom0`)
* setup VDE configuration for static ip (target `vde`)

After this, only build the targets you need. Inter-target dependencies are not set properly. When changing target platform, use the make target `platform` to only rebuild `genode_build_dir`, `tasks`, and `dom0`.

To run the Genode dom0 server type `make run`

The Makefile will need additional adjustments for other kernels than Fiasco.OC.

### Required packages
The following packages are required for building the Genode toolchain:

`sudo apt-get install libncurses5-dev texinfo autogen autoconf2.64 g++ libexpat1-dev flex bison gperf`

For building Genode:

`sudo apt-get install make pkg-config gawk subversion expect git`

For running Genode in QEMU:

`sudo apt-get install libxml2-utils syslinux`

For some additional ports you may need:

`sudo apt-get install xsltproc yasm iasl lynx`

You will also need a manual build of QEMU (with VDE support enabled) and VDE.

### Folder structure
Custom repos:

| Folder                              | Description                                      |
| ----------------------------------- | ------------------------------------------------ |
| `repos/dom0/`                       | central network module: server and task manager  |
| `repos/dom0/run/`                   | run file configuration for dom0                  |
| `repos/dom0/src/dom0/server/`       | dom0 TCP server                                  |
| `repos/dom0/src/dom0/task-manager/` | dom0 task manager and profiler                   |
| `dom0_client/`                      | Python module and example for dom0 communication |

The provided Makefile creates the following directories:

| Folder                      | Description               |
| --------------------------- | ------------------------- |
| `build/`                    | all build files           |
| `build/toolchain-TARGET/`   | Genode toolchain          |
| `build/genode-TARGET/`      | Genode build dir          |
| `contrib/`                  | external Genode libraries |

### VDE
The VDE network can be set up using `make vde`. VDE functionality requires a manual build of QEMU with VDE support enabled. Reset the VDE network using `make vde-stop`.

### DHCP
This port of dom0 supports DHCP. For this, change the `<dhcp>` field in `dom0.run` to `yes` and make sure to run a DHCP server in the network. The make target `dhcp` starts a Slirp DHCP server in the VDE network. Kill it using the `dhcp-stop` target. Tap interfaces and routes in the VDE network have to be changed accordingly (see `vde` target).

### dom0 Client
The python module `dom0_client.py` provides a simple class to communicate with Genode's dom0 server. The class will attempt connection on startup. Further instructions are provided at run-time by using the `help()` function.

An example of how to use this class is in the script `dom0_program.py` which interacts with the dom0 server using the client module.

### Fine-grained CPU time
The Fiasco.OC kernel by default only returns timing information at a resolution of 1ms. Granularity can be increased by changing line 51 of `build/genode-TARGET/kernel/fiasco.oc/globalconfig.out` to
`CONFIG_FINE_GRAINED_CPUTIME=y`


### Genode configuration
There are several configuration options for the dom0 server and task manager which can be defined in the `<config>` section of `dom0` and `task-manager` of `dom0.run`.

Server:

| Node     | Attribute         | Value (type) | Description                            |
| -------- | ----------------- | ------------ | -------------------------------------- |
| `server` | `dhcp`            | "yes","no"   | use static or dynamic server ip        |
| `server` | `listen-address`  | string       | server listening ip                    |
| `server` | `network-mask`    | string       | server network submask (255.255.255.0) |
| `server` | `network-gateway` | string       | network default gateway                |
| `server` | `port`            | number       | server listening port                  |

Task Manager:

| Node      | Attribute  | Value (type)    | Description                       |
| --------- | ---------- | --------------- | --------------------------------- |
| `trace`   | `quota`    | number (prefix) | RAM reserved for tracing purposes |
| `trace`   | `buf-size` | number (prefix) | size of trace argument buffer     |
| `profile` | `ds-size`  | number (prefix) | max size of the XML event log     |

Additionally, XML task descriptions generated by the modified tms-sim tool contain a `config` node which is forwarded to the started task. Currently, this is only used by the task `tumatmul` which calculates all primes up to the number specified in `arg1`.
