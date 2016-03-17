import os
import sqlite3
import xml.etree.ElementTree as ET

script_dir = os.path.dirname(os.path.realpath(__file__)) + '/'

def xml_to_sql(xml_file='log.xml', sql_file='dom0.db'):
	"""Parse the dom0 event log and store it in an SQL database file."""
	# Open XML file and connect to SQL database.
	tree = ET.parse(script_dir + xml_file)
	root = tree.getroot()

	conn = sqlite3.connect(script_dir + sql_file)
	c = conn.cursor()


	# Create and fill task table.
	c.execute('''CREATE TABLE tasks
	(
	id INTEGER NOT NULL PRIMARY KEY,
	name STRING,
	critical_time INTEGER,
	priority INTEGER,
	period INTEGER,
	quota INTEGER,
	binary STRING,
	iterations INTEGER
	)''')

	tasks = root.find('task-descriptions')
	task_inserts = []
	for task in tasks:
		a = task.attrib
		# see Task::_make_name()
		if a['id'] == '0':
			name = 'task-manager'
		else:
			name = "%.2d.%s" % (int(a['id']), a['binary']);
		task_inserts.append((a['id'], name, a['critical-time'], a['priority'], a['period'], a['quota'], a['binary'], 0))

	c.executemany('''INSERT INTO tasks VALUES (?,?,?,?,?,?,?,?)''', task_inserts)


	# Create and fill event table.
	c.execute('''CREATE TABLE events
	(
		id INTEGER NOT NULL PRIMARY KEY,
		time_stamp INTEGER,
		type STRING,
		task_id INTEGER
	)''')

	events = root.find('events')
	event_inserts = []
	i = 0
	for event in events:
			a = event.attrib
			event_inserts.append((i, a['time-stamp'], a['type'], a['task-id']))
			i += 1

	c.executemany('''INSERT INTO events VALUES (?,?,?,?)''', event_inserts)

	conn.commit()
	conn.close()

# TEMP
xml_to_sql()
