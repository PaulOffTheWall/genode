# Protocol magic numbers
control = 0xC047201 # client wants to send control message
task_desc = 0xDE5 # client wants to send task description
send_binaries = 0xDE5F11E # client watns to send binaries
lua = 0x10A # client wants to send LUA command
lua_ok = 0x10A900D # possible reply to LUA command
lua_err = 0x10ABAD # possible reply to LUA command
send_binary = 0xF11E # client wants to send binary
go_send = 0x90 # server tells client that he is ready to recieve
ok_received = 0x900D # server tells client that he has received the binary
start = 0x514DE5