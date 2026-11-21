#!/usr/bin/env python3
#
# this file updates release relevant meta things:
# - updating READMEs with current cli --help output
# - updating READMEs with contributors [not present yet]
# - updating about Mandelbulber popup with contributors [not present yet]
#
# It is a port of the former updateREADMEandCo.php and writes the same output.
#
# Requires Python 3.6 or newer and the current mandelbulber executable (set MANDELBULBER_EXEC_PATH
# to its path when it is not mandelbulber2 on PATH; set locale to english).
#
# By default this script runs dry: it shows which READMEs would change. Run it with the argument
# "nondry" to write the changes, and with "verbose" to list every file.
#
# usage: mandelbulber2/tools/update_readme_and_co.py [nondry] [verbose]

import os
import shutil
import subprocess
import sys
import time

PROJECT_PATH = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
MANDELBULBER_EXEC_PATH = os.environ.get('MANDELBULBER_EXEC_PATH', 'mandelbulber2')
START_TIME = time.time()
NL = '\n'
OPTIONS_MARKER = 'Options:'

READMES = [
	('linux', os.path.join(PROJECT_PATH, 'deploy', 'README')),
	('osx', os.path.join(PROJECT_PATH, 'deploy', 'README-osx.txt')),
	('win', os.path.join(PROJECT_PATH, 'deploy', 'README-win32.txt')),
]


# --- console output --------------------------------------------------------------------------

def color(code, s):
	return '\033[' + code + 'm' + s + '\033[0m'


def error_string(s):
	return color('1;31', s)


def success_string(s):
	return color('1;32', s)


def notice_string(s):
	return color('1;34', s)


def php_str_pad(s, width):
	# PHP's str_pad counts bytes, so the tree characters (3 bytes in UTF-8) shift the column
	return s + ' ' * max(0, width - len(s.encode('utf-8')))


def is_dry_run():
	return 'nondry' not in sys.argv[1:]


def is_verbose():
	return 'verbose' in sys.argv[1:]


def print_start_group(title):
	print(NL + '\033[1m\033[44m ' + php_str_pad(title, 54) + ' \033[0m')


def print_end_group():
	print(' ╹')


def print_result_line(name, success, status):
	if success and not is_verbose() and not status:
		return
	line = php_str_pad(' ├── ' + name, 50)
	print(line + (success_string(' [ All OK ]') if success else error_string(' [ ERROR  ]')))
	for i, s in enumerate(status):
		print((' │   ╰── ' if i == len(status) - 1 else ' │   ├── ') + s)


def print_finish():
	print('script took ' + notice_string('%.2f seconds' % (time.time() - START_TIME)) + ' to complete.')
	if is_dry_run():
		print('This is a dry run.')
		print('If you want to apply the changes, execute with argument "nondry"')
	else:
		print('Changes applied')


# --- file access ---------------------------------------------------------------------------------

def read_text(path):
	with open(path, encoding='utf-8', errors='surrogateescape', newline='') as f:
		return f.read().replace('\r\n', '\n')


def write_text(path, content):
	with open(path, 'w', encoding='utf-8', errors='surrogateescape', newline='\n') as f:
		f.write(content)


# --- update ----------------------------------------------------------------------------------------

def get_help_output():
	out = subprocess.run([MANDELBULBER_EXEC_PATH, '--help'], stdout=subprocess.PIPE).stdout
	return out.decode('utf-8', errors='surrogateescape').replace('\r\n', '\n')


def update_readme(path, status, cli_option_string):
	old_content = read_text(path)
	index = old_content.find(OPTIONS_MARKER)
	if index < 0:
		status.append(error_string('"' + OPTIONS_MARKER + '" not found, file left unchanged'))
		return False
	content = old_content[:index + len(OPTIONS_MARKER)] + cli_option_string

	if content != old_content:
		if not is_dry_run():
			write_text(path, content)
		status.append(success_string('readme changed'))
	return True


def main():
	if hasattr(sys.stdout, 'reconfigure'):
		sys.stdout.reconfigure(encoding='utf-8')  # the tree characters, also on Windows consoles
	if shutil.which(MANDELBULBER_EXEC_PATH) is None:
		sys.exit(error_string('mandelbulber not found: ' + MANDELBULBER_EXEC_PATH
			+ ' (set MANDELBULBER_EXEC_PATH to its path)'))
	print('Processing...')

	# news_content = read_text(os.path.join(PROJECT_PATH, 'deploy', 'NEWS'))
	# contributors_content = read_text(os.path.join(PROJECT_PATH, 'contributors.txt'))

	help_output = get_help_output()
	index = help_output.find(OPTIONS_MARKER)
	if index < 0:
		sys.exit(error_string('"' + OPTIONS_MARKER + '" not found in the output of '
			+ MANDELBULBER_EXEC_PATH + ' --help'))
	cli_option_string = help_output[index + len(OPTIONS_MARKER):]

	print_start_group('WRITING HELP OUTPUT TO README')
	success_all = True
	for _, path in READMES:
		status = []
		success = update_readme(path, status, cli_option_string)
		success_all = success_all and success
		print_result_line(os.path.basename(path), success, status)
	print_end_group()
	print_finish()

	# contributors to README
	# TODO

	# contributors to about Mandelbulber popup
	# TODO

	return 0 if success_all else 1


if __name__ == '__main__':
	sys.exit(main())
