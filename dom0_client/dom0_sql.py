import os
import sqlite3
import xml.etree.ElementTree as ET

script_dir = os.path.dirname(os.path.realpath(__file__)) + '/'

def xml2sql(xml_file=script_dir+'log.xml', sql_file=script_dir+'dom0.db'):
	"""Parse the dom0 event log and store it in an SQL database file."""
	print('Generating SQLite database at {}'.format(sql_file))
	# Open XML file and connect to SQL database.
	tree = ET.parse(xml_file)
	root = tree.getroot()

	if os.path.exists(sql_file):
		os.remove(sql_file)

	conn = sqlite3.connect(sql_file)
	c = conn.cursor()


	# Create and fill task table.
	c.execute('''CREATE TABLE tasks
	(
	id INT NOT NULL PRIMARY KEY,
	name STRING,
	critical_time INT,
	priority INT,
	period INT,
	quota INT,
	binary VARCHAR
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
		task_inserts.append((a['id'], name, a['critical-time'], a['priority'], a['period'], a['quota'], a['binary']))

	c.executemany('''INSERT INTO tasks VALUES (?,?,?,?,?,?,?)''', task_inserts)


	# Create and fill event table.
	c.execute('''CREATE TABLE events
	(
		id INT NOT NULL PRIMARY KEY,
		time_stamp INT,
		type VARCHAR,
		task_id INT,
		FOREIGN KEY (task_id) REFERENCES tasks(id)
	)''')

	events = root.find('events')
	event_inserts = []
	i = 0
	for event in events:
			a = event.attrib
			id = a['task-id'] if int(a['task-id']) >= 0 else ''
			event_inserts.append((i, a['time-stamp'], a['type'], id))
			i += 1

	c.executemany('''INSERT INTO events VALUES (?,?,?,?)''', event_inserts)


	# Create and fill managed task tables (performance snapshots).
	c.execute('''CREATE TABLE snapshots
	(
		task_id INT NOT NULL,
		event_id INT NOT NULL,
		execution_time INT NOT NULL,
		quota INT NOT NULL,
		used INT NOT NULL,
		iteration INT NOT NULL,
		FOREIGN KEY (task_id) REFERENCES tasks(id),
		FOREIGN KEY (event_id) REFERENCES events(id)
	)''')

	snapshot_inserts = []
	i = 0
	for event in events:
		for task in event:
			a = task.attrib
			if a['managed'] == 'yes' and a['state'] != 'DEAD':
				am = task.find('managed-task').attrib
				snapshot_inserts.append((am['id'], i, a['execution-time'], am['quota'], am['used'], am['iteration']))
		i += 1

	c.executemany('''INSERT INTO snapshots VALUES (?,?,?,?,?,?)''', snapshot_inserts)

	conn.commit()
	conn.close()
