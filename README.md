# genode-dom0
Central network module for Genode OS Framework

### Building
The custom Makefile builds the entire Fiasco.OC and Genode system including its toolchain and external dependencies into a `build/` and `contrib/` directory respectively. To manually execute steps within the build process see the Makefile or the Genode documentation:
http://genode.org/documentation/developer-resources/getting_started

The toolchain installs executables to `/usr/local/genode-gcc` as part of the `tool_chain` script. Everything else stays within the repository.

Inter-target dependencies are not resolved automatically (yet).

### Required packages
The following packages are required for building the Genode toolchain:

`sudo apt-get install libncurses5-dev texinfo autogen autoconf2.64 g++ libexpat1-dev flex bison gperf`

For building Genode:

`sudo apt-get install make pkg-config gawk subversion expect git`

For running Genode in QEMU:

`sudo apt-get install libxml2-utils syslinux qemu`

For some additional ports you may need:

`sudo apt-get install xsltproc yasm iasl lynx`

### Folder structure
Custom repos:

| Folder         | Description                               |
| -------------- | ----------------------------------------- |
| `repos/dom0/`  | central network module; port from L4RE    |
| `dom0-client/` | Python client for dom0 test communication |

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
The python script Dom0Client provides a simple class to communicate with Genode's dom0 server.

In order to run it you need the module `xmltodict`:

`sudo apt-get install python3-pip`

`sudo python3 -m pip install xmltodict`

Run the script and use `session`'s functions `sendDescs`, `sendTascs`, and `start` to communicate with the Genode server.
