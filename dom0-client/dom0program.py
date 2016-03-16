#!/usr/bin/env python3

from dom0client import *
import time

session = Dom0_session('192.168.0.14', 3001, 'tasks.xml')
session.start_ex()

time.sleep(10)

session.stop()

time.sleep(1)

session.profile('log.xml')

help()
code.interact(local=locals())
