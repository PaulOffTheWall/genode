#!/usr/bin/env python3

import socket
import code
import struct
import magicnumbers
import os
import re

scriptDir = os.path.dirname(os.path.realpath(__file__)) + '/'

class Dom0Session:
	"""Manager for a connection to the dom0 server."""
	def __init__(self, host='192.168.0.14', port=3001, tasksFile='tasks.xml'):
		"""Initialize connection and parse tasks description."""
		self.connect(host, port)
		self.readTasks(tasksFile)

	def connect(self, host='192.168.0.14', port=3001):
		"""Connect to the Genode dom0 server."""
		self.conn = socket.create_connection((host, port))
		print('Connected.')

	def readTasks(self, tasksFile='tasks.xml'):
		"""Read XML file and enumerate binaries."""
		# Read XML file and discard meta data.
		self.tasks = open(scriptDir + tasksFile, 'rb').read()
		tasksAscii = self.tasks.decode('ascii')

		# Enumerate binaries.
		self.binaries = re.findall('<\s*pkg\s*>\s*(.+)\s*<\s*/pkg\s*>', tasksAscii)
		self.binaries = list(set(self.binaries))

		# Genode XML parser can't handle a lot of header things, so skip them.
		firstNode = re.search('<\w+', tasksAscii)
		self.tasks = self.tasks[firstNode.start():]

	def sendDescs(self):
		"""Send task descriptions to the dom0 server."""
		meta = struct.pack('II', magicnumbers.SEND_DESCS, len(self.tasks))
		print('Sending tasks description.')
		self.conn.send(meta)
		self.conn.send(self.tasks)

	def sendBins(self):
		"""Send binary files to the dom0 server."""
		meta = struct.pack('II', magicnumbers.SEND_BINARIES, len(self.binaries))
		print('Sending {} binar{}.'.format(len(self.binaries), 'y' if len(self.binaries) == 1 else 'ies'))
		self.conn.send(meta)

		for name in self.binaries:
			# Wait for 'go' message.
			msg = int.from_bytes(self.conn.recv(4), 'little')
			if msg != magicnumbers.GO_SEND:
				print('Invalid answer received, aborting: {}'.format(msg))
				break

			print('Sending {}.'.format(name))
			file = open(scriptDir + name, 'rb').read()
			size = os.stat(scriptDir + name).st_size
			meta = struct.pack('15scI', name.encode('ascii'), b'\0', size)
			self.conn.send(meta)
			self.conn.send(file)

	def start(self):
		"""Send message to start the tasks on the server."""
		print('Starting tasks on server.')
		meta = struct.pack('I', magicnumbers.START)
		self.conn.send(meta)

	def startEx(self):
		"""Send task descriptions and binaries, and start the execution."""
		self.sendDescs()
		self.sendBins()
		self.start()

	def stop(self):
		"""Send message to kill all tasks."""
		print('Stopping tasks on server.')
		meta = struct.pack('I', magicnumbers.STOP)
		self.conn.send(meta)

	def clear(self):
		"""Send command to stop all tasks and clear the current task set."""
		print('Resetting all tasks on server.')
		meta = struct.pack('I', magicnumbers.CLEAR)
		self.conn.send(meta)

	def getProfile(self):
		"""Get profiling information about all running tasks."""
		print('Requesting profile data.')
		meta = struct.pack('I', magicnumbers.GET_PROFILE)
		self.conn.send(meta)

		size = int.from_bytes(self.conn.recv(4), 'little')
		xml = b''
		print('Receiving profiling data of size {}'.format(size))
		while len(xml) < size:
			xml += self.conn.recv(size)
		print(xml.decode('utf-8'))

	def close(self):
		"""Close connection."""
		self.conn.close();

session = Dom0Session()

print('''
Available commands:
	session.sendDescs()		: Send task descriptions to server.
	session.sendBins()		: Send binaries to server.
	session.start()			: Start tasks on server.
	session.startEx()		: Do all of the above in order.
	session.stop()			: Stop all running tasks on server.
	session.clear()			: Stop and clear all tasks on the server. Binaries will be kept.
	session.getProfile()		: Request profiling data from server.

	session.readTasks([tasksFile])	: Load tasks file (default tasks.xml).
	session.connect([host, port])	: Connect to dom0 server (default 192.168.0.14:3001).
	session.close()			: Close connection.
''')

code.interact(local=locals())