#!/usr/bin/env python3

from dom0_client import *
from dom0_sql import *
import time

session = Dom0_session('192.168.0.14', 3001)
session.read_tasks(script_dir + 'tasks.xml')
session.send_descs()
session.send_bins()
session.start()

print('Waiting for 10 seconds...')
time.sleep(10)

session.stop()

time.sleep(1)

session.profile(script_dir + 'log.xml')
xml2sql(script_dir + 'log.xml', script_dir + 'dom0.db')

help()
code.interact(local=locals())
