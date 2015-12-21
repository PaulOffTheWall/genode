TARGET = dom0
SRC_CC = main.cc tcp_client_socket.cc tcp_socket.cc tcp_server_socket.cc
LIBS = base lwip libc stdcxx

INC_DIR += $(REP_DIR)/../libports/include/lwip
