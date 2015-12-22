TARGET = dom0
SRC_CC = main.cc tcp_client_socket.cc tcp_socket.cc tcp_server_socket.cc json_util.cc
LIBS = base lwip libc stdcxx launchpad jsmn

INC_DIR += $(REP_DIR)/../libports/include/lwip
