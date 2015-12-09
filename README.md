# genode-dom0
Central network module for Genode OS Framework

### Building
The custom Makefile builds the entire Fiasco.OC and Genode system including its toolchain and external dependencies into a `build/` and `contrib/` directory respectively. To manually execute steps within the build process see the Makefile or the Genode documentation:
http://genode.org/documentation/developer-resources/getting_started

The toolchain installs executables to `/usr/local/genode-gcc` as part of the `tool_chain` script. Everything else stays within the repository.

Possible targets are: `all` `toolchain` `foc` `genode_build_dir` `genode`

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

| Folder        | Description                            |
| ------------- | -------------------------------------- |
| `repos/dom0/` | central network module; port from L4RE |

The provided Makefile creates the following directories:

| Folder                      | Description               |
| --------------------------- | ------------------------- |
| `build/`                    | all build files           |
| `build/toolchain-TARGET/`   | Genode toolchain          |
| `build/genode-TARGET/`      | Genode build dir          |
| `contrib/`                  | external Genode libraries |
