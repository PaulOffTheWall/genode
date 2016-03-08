# Packet contains task descriptions as XML. uint32_t after tag indicates size in bytes.
SEND_DESCS = 0xDE5

# Multiple binaries are to be sent. uint32_t after tag indicates number of binaries. Each binary packet contains another leading uint32_t indicating binary size.
SEND_BINARIES = 0xDE5F11E

# Binary received, send next one.
GO_SEND = 0x90

# Start queued tasks.
START = 0x514DE5
