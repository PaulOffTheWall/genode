JSMN_DIR := $(call select_from_ports,jsmn)/src/lib/jsmn
SRC_C = jsmn.c
vpath %.c $(JSMN_DIR)
INC_DIR += $(JSMN_DIR)
LIBS += libc