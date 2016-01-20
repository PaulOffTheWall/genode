TARGET = dom0
SRC_CC = main.cc tcp_client_socket.cc tcp_socket.cc dom0_server.cc task_manager.cc config.cc
LIBS = base lwip libc stdcxx launchpad

INC_DIR += $(REP_DIR)/../libports/include/lwip
