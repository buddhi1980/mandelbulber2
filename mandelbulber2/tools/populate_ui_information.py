#!/usr/bin/env python3
#
# this file takes the formulas in formula/definition and does a couple of things:
# - generates the information boxes in the ui from the source code
# - generates opencl formulas
# - generates formula thumbnails and configs
# - write a csv file which contains a table of all formulas
#
# It is a port of the former populateUiInformation.php and generates the same output.
#
# Requires Python 3.6 or newer, highlight, clang-format and git
# (apt-get install highlight clang-format git).
# The programs are taken from PATH; set these environment variables to use others:
#   HIGHLIGHT               highlight (formats the code shown in the ui)
#   CLANG_FORMAT            clang-format (formats the opencl files)
#   MANDELBULBER_EXEC_PATH  mandelbulber2 (renders missing formula thumbnails)
#   CONVERT                 ImageMagick convert (magick on Windows)
#   CLANG                   clang with libclc (used with "checkCl")
#
# on default this script runs dry,
# it will try to parse all formulas and show which ui files would be modified
# this should always be run first, to see if any issues occur
# if you invoke this script with "nondry" as cli argument it will write changes to ui files
# if you invoke this script with "checkCl" as cli argument it will also check formula opencl file
# compilability (slow)
# "verbose" lists every formula, "warning" also shows warnings
#
# usage: mandelbulber2/tools/populate_ui_information.py [nondry] [verbose] [warning] [checkCl]

import decimal
import glob
import math
import os
import re
import shutil
import subprocess
import sys
import time

# the paths are built like in the php tools: <mandelbulber2>/tools/../formula/...
PROJECT_PATH = os.path.dirname(os.path.realpath(__file__)).replace(os.sep, '/') + '/../'
START_TIME = time.time()
NL = '\n'

HIGHLIGHT_EXEC_PATH = os.environ.get('HIGHLIGHT', 'highlight')
CLANG_FORMAT_EXEC_PATH = os.environ.get('CLANG_FORMAT', 'clang-format')
MANDELBULBER_EXEC_PATH = os.environ.get('MANDELBULBER_EXEC_PATH', 'mandelbulber2')
# on Windows "convert" is a system tool, ImageMagick is called as magick there
CONVERT_EXEC_PATH = os.environ.get('CONVERT', 'magick' if os.name == 'nt' else 'convert')
CLANG_EXEC_PATH = os.environ.get('CLANG', 'clang')

ENCODING = {'encoding': 'utf-8', 'errors': 'surrogateescape'}


# --- console output ------------------------------------------------------------------------------

def echo(s):
	sys.stdout.write(s)
	sys.stdout.flush()


def color(code, s):
	return '\033[' + code + 'm' + s + '\033[0m'


def error_string(s):
	return color('1;31', s)


def success_string(s):
	return color('1;32', s)


def notice_string(s):
	return color('1;34', s)


def progress_string(s):
	return color('1;35', s)


def warning_string(s):
	return color('1;33', s)


def argument_contains(needle):
	return needle in sys.argv[1:]


def is_dry_run():
	return not argument_contains('nondry')


def is_verbose():
	return argument_contains('verbose')


def is_warning():
	return argument_contains('verbose') or argument_contains('warning')


def die(message):
	# like php's die(): print the message and stop
	echo(message)
	sys.exit(0)


def str_pad(s, length):
	# php's str_pad counts bytes, so the tree characters take three places each
	return s + ' ' * max(0, length - len(s.encode('utf-8', 'surrogateescape')))


def number_format(value, decimals):
	# php rounds half away from zero and groups the thousands with ","
	quantum = decimal.Decimal(1).scaleb(-decimals)
	rounded = decimal.Decimal(repr(value)).quantize(quantum, rounding=decimal.ROUND_HALF_UP)
	return '{:,f}'.format(rounded)


def print_r(lines):
	# print_r() of a list of strings
	return 'Array' + NL + '(' + NL + ''.join(
		'    [' + str(i) + '] => ' + line + NL for i, line in enumerate(lines)) + ')' + NL


def print_start():
	echo('Processing...' + NL)


def print_finish():
	echo('\033[2K\r')
	sec_string = notice_string(number_format(time.time() - START_TIME, 2) + ' seconds')
	echo('script took ' + sec_string + ' to complete.' + NL)
	if is_dry_run():
		echo('This is a dry run.' + NL)
		echo('If you want to apply the changes, execute with argument "nondry"' + NL)
	else:
		echo('Changes applied' + NL)


def print_result_line(name, success, status, progress=-1):
	out = '\033[2K\r'
	item_line = str_pad(' ├── ' + name, 50)
	if not (success and not is_verbose() and len(status) == 0):
		if success:
			out += item_line + success_string(' [ All OK ]') + NL
		else:
			out += item_line + error_string(' [ ERROR  ]') + NL
		for i, s in enumerate(status):
			tree_str = ' │   ╰── ' if i == len(status) - 1 else ' │   ├── '
			out += tree_str + s + NL
	out += print_progress(progress) + ' handled: ' + name
	echo(out)


def print_progress(percent):
	if percent < 0:
		return ''
	free_width = 20
	int_progress = int(math.floor(free_width * percent))

	text = notice_string('[')
	text += progress_string('#' * int_progress)
	text += ' ' * (free_width - int_progress)
	text += '/ ' + progress_string(number_format(percent * 100, 1).rjust(4) + '%')
	text += notice_string(']')
	return text


def print_start_group(title):
	echo(NL + '\033[1m\033[44m ' + title.ljust(54) + ' \033[0m' + NL)


def print_end_group():
	echo('\033[2K\r')
	echo(' ╹' + NL)


# --- php string helpers --------------------------------------------------------------------------

def trim(s):
	# php's trim() strips only these characters
	return s.strip(' \t\n\r\0\x0b')


def strtolower(s):
	# php changes only the ASCII letters
	return ''.join(c.lower() if c < '' else c for c in s)


def strtoupper(s):
	return ''.join(c.upper() if c < '' else c for c in s)


def ucfirst(s):
	return strtoupper(s[:1]) + s[1:]


def lcfirst(s):
	return strtolower(s[:1]) + s[1:]


def substr(s, start, length):
	# php's substr() with a start from strpos() and a length that may be negative
	end = start + length if length >= 0 else len(s) + length
	return s[start:max(start, end)]


def substr_replace(s, replacement, start, length):
	end = start + length if length >= 0 else len(s) + length
	return s[:start] + replacement + s[max(start, end):]


def htmlspecialchars(s):
	# htmlspecialchars($s, ENT_QUOTES, 'UTF-8'): invalid UTF-8 gives an empty string
	if re.search('[\udc80-\udcff]', s):
		return ''
	return (s.replace('&', '&amp;').replace('"', '&quot;').replace("'", '&#039;')
		.replace('<', '&lt;').replace('>', '&gt;'))


def php_replacement(replacement):
	"""splits a php preg_replace() replacement into literal text and group numbers:
	$1, ${1} and \\1 insert a group, \\\\ and \\$ give a literal \\ and $"""
	parts = []
	literal = []
	i = 0
	last = ''
	while i < len(replacement):
		c = replacement[i]
		if c in '\\$':
			if last == '\\':
				literal[-1] = c
				i += 1
				last = ''
				continue
			m = re.match(r'\$\{(\d\d?)\}|[\\$](\d\d?)', replacement[i:])
			if m:
				if literal:
					parts.append(''.join(literal))
					literal = []
				parts.append(int(m.group(1) or m.group(2)))
				i += m.end()
				continue
		literal.append(c)
		last = c
		i += 1
	if literal:
		parts.append(''.join(literal))
	return parts


def preg_replace(pattern, replacement, subject, limit=-1, flags=0):
	"""preg_replace() with a php replacement string, returns the result and the count"""
	parts = php_replacement(replacement)
	regex = re.compile(pattern, re.ASCII | flags)

	def expand(m):
		out = []
		for part in parts:
			if isinstance(part, int):
				out.append((m.group(part) or '') if part <= regex.groups else '')
			else:
				out.append(part)
		return ''.join(out)

	return regex.subn(expand, subject, count=0 if limit < 0 else limit)


def preg_replace1(pattern, replacement, subject, limit=-1, flags=0):
	return preg_replace(pattern, replacement, subject, limit, flags)[0]


def preg_match(pattern, subject, flags=0):
	return re.search(pattern, subject, re.ASCII | flags)


def strpos(haystack, needle, offset=0):
	# None when not found (php: false)
	pos = haystack.find(needle, offset or 0)
	return None if pos < 0 else pos


# --- files and programs --------------------------------------------------------------------------

def read_file(path):
	"""file contents with CRLF normalized to LF, None when the file cannot be read"""
	try:
		with open(path, newline='', **ENCODING) as f:
			return f.read().replace('\r\n', '\n')
	except OSError:
		return None


def write_file(path, content):
	with open(path, 'w', newline='\n', **ENCODING) as f:
		f.write(content)


def php_glob(pattern):
	# php's glob() returns the matches sorted
	return sorted(p.replace(os.sep, '/') for p in glob.glob(pattern))


def run(args, **kwargs):
	"""runs a program and returns its stdout as text ('' when the program is missing)"""
	try:
		result = subprocess.run(args, stdout=subprocess.PIPE, **kwargs)
	except OSError:
		return ''
	return result.stdout.decode(*ENCODING.values())


# --- copyright years from git history ------------------------------------------------------------

_modification_intervals = None


def get_modification_intervals():
	"""first and last year in which each file was changed, from git log"""
	modification_file_info = {}
	raw = trim(run(['git', 'log',
		'--format=COMMIT_SEPARATOR_1____________%adCOMMIT_SEPARATOR_2____________%s', '--name-only'],
		cwd=PROJECT_PATH))
	for commit_string in raw.split('COMMIT_SEPARATOR_1____________'):
		if not commit_string:
			continue
		commit_lines = commit_string.split(NL)
		meta = commit_lines[0].split('COMMIT_SEPARATOR_2____________')
		date = meta[0][-10:][:4]
		title = meta[1] if len(meta) > 1 else ''

		# these commits contain only auto formatting code changes
		# and should not be counted for the modification invertal
		if 'source code' in title or 'nullptr' in title or 'remove src dir' in title:
			continue

		for commit_line in commit_lines:
			if commit_line and 'COMMIT_SEPARATOR_2____________' not in commit_line:
				entry = modification_file_info.setdefault(commit_line, {'start': date, 'end': date})
				entry['start'] = min(entry['start'], date)
				entry['end'] = max(entry['end'], date)
	return modification_file_info


def get_modification_interval(file_path, only_end_year=False):
	global _modification_intervals
	if not _modification_intervals:
		_modification_intervals = get_modification_intervals()

	file_path_relative = file_path.replace(PROJECT_PATH, 'mandelbulber2/')
	entry = _modification_intervals.get(file_path_relative)
	if not entry:
		return time.strftime('%Y')
	year_start, year_end = entry['start'], entry['end']
	if year_start == year_end:
		return year_start
	if only_end_year:
		return year_end
	return year_start + '-' + year_end[2:4]


# --- reading the formulas ------------------------------------------------------------------------

def get_formulas_data():
	formulas = {}
	index_id_lookup = get_index_id_look_up()

	for file in php_glob(PROJECT_PATH + 'formula/definition/*.cpp'):
		if 'abstract_fractal.cpp' in file:
			continue
		if 'all_fractal_list.cpp' in file:
			continue
		if 'legacy_fractal_transforrms.cpp' in file:
			continue
		if 'fractal_none.cpp' in file:
			continue

		# echo('Processing file ' + file + NL)

		fractal_cpp_file = read_file(file) or ''
		# read index and name from fractal_list
		match = preg_match(r'cAbstractFractal\(\)\n{([\s\S]*?)\n}', fractal_cpp_file)
		definition_part = match.group(1) if match else ''
		definition_part_lines_raw = trim(definition_part).split(NL)
		# strip comment lines
		definition_part_lines = [l for l in definition_part_lines_raw if not preg_match(r'\s*//.*', l)]
		if len(definition_part_lines) != 9:
			die('could not read index for formula : ' + file + ' --> ' + print_r(definition_part_lines_raw))
		f = {}
		for definition_part_line in definition_part_lines:
			match_line = preg_match(r'([a-zA-Z]+)\s*=\s*"?([\da-zA-Z\.: -_]*?)"?;', definition_part_line)
			if not match_line:
				die('Line wrong for ' + file + ' --> ' + definition_part_line)
			f[match_line.group(1)] = match_line.group(2)
		index = f.get('internalID', '').replace('fractal::', '')

		internal_name_new = from_camel_case(index)

		# echo(index + ' ' + internal_name_new + NL)

		# check for automatic renaming to fit naming convention
		if internal_name_new != f.get('internalName'):
			if not is_dry_run():
				upgrade_internal_name(f.get('internalName', ''), internal_name_new)
			echo(notice_string('internal name upgrade from ' + f.get('internalName', '') + ' to '
				+ internal_name_new) + NL)
			f['internalName'] = internal_name_new
			if not is_dry_run():
				die('Changes have been written, check changes and run script again for more changes.')

		# read function contents
		function_content_match_string = r'\+\+\+\n\s\*(\n[\s\S]+?\*/)[\S\s]*(FormulaCode\([\s\S]*})'

		# echo(function_content_match_string + NL)

		match_function_content = preg_match(function_content_match_string, fractal_cpp_file)
		if not match_function_content:
			echo(error_string('Warning, could not read code for index: ' + index) + NL)
			continue
			# die('could not read code for index: ' + index)

		raw_comment = match_function_content.group(1)
		comment = parse_comment(trim(match_function_content.group(1)))
		code = match_function_content.group(2)
		code = trim(code.replace('FormulaCode', 'void ' + ucfirst(index) + 'Iteration'))

		formula = dict(f)
		formula.update({
			'uiFile': PROJECT_PATH + 'formula/ui/' + f['internalName'] + '.ui',
			'definitionFileName': os.path.basename(file),
			'code': code,
			'comment': comment,
			'rawComment': raw_comment,
			'id': index_id_lookup.get(index),
			'openclFile': PROJECT_PATH + 'formula/opencl/' + f['internalName'] + '.cl',
			'definitionFile': PROJECT_PATH + 'formula/definition/fractal_' + f['internalName'] + '.cpp',
			'openclCode': parse_to_opencl(code),
			'type': 'transf' if 'transf_' in f['internalName'] else 'formula',
		})
		formulas[index] = formula
	return formulas


# --- information boxes in the ui -----------------------------------------------------------------

UI_CODE_REPLACEMENT = '''$1<item>
       <widget class="QLabel" name="label_information_general">
        <property name="text">
         <string notr="true">%s</string>
        </property>
        <property name="wordWrap">
         <bool>true</bool>
        </property>
        <property name="openExternalLinks">
         <bool>true</bool>
        </property>
        <property name="textInteractionFlags">
         <set>Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse</set>
        </property>
       </widget>
      </item>
      <item>
       <widget class="QLabel" name="label_code_content">
        <property name="styleSheet">
         <string notr="true">border-style: outset; border-width: 2px; border-radius: 3px; border-color: black; background-color: #fff5ee; padding: 4px;</string>
        </property>
        <property name="text">
         <string notr="true">%s</string>
        </property>
        <property name="wordWrap">
         <bool>true</bool>
        </property>
        <property name="openExternalLinks">
         <bool>true</bool>
        </property>
        <property name="textInteractionFlags">
         <set>Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse</set>
        </property>
       </widget>
      </item>
     </layout>'''

REPLACE_FORMULA_LOOKUP = [
	(r'>\s*([^<]+?)\s*</string>', '>$1</string>'),  # whitespace fix 1
	(r'>\s*([^<]+?)\s*:\s*</string>', '>$1:</string>'),  # whitespace fix 2
	(r'<string>(.*?)\s+(\s.*?)</string>', '<string>$1$2</string>'),  # whitespace fix 3, stop those whitespaces! :)
	(r'<string>fabs\s*\(\s*(.*?)\s*\)\s*</string>', '<string>abs($1)</string>'),
	(r'<string>abs\s*\(\s*(.*?)\s*\)\s*</string>', '<string>abs($1)</string>'),
	(r'<string>(.*?)::(.*?)</string>', '<string>$1:$2</string>'),
	(r'<string>Menger Scale</string>', '<string>Menger Scale:</string>'),
	(r'<string>Menger offsets:</string>', '<string>Menger Offset:</string>'),
	(r'<string>DE tweak temp</string>', '<string>DE tweak temp:</string>'),
	(r'<string>Offset</string>', '<string>Offset:</string>'),
	(r'Rotation;</string>', 'Rotation</string>'),
	(r'>Multiplier</string>', '>Multiplier:</string>'),
	(r'>Cpixel Multiplier</string>', '>Cpixel Multiplier:</string>'),
	(r'>Addition Constant</string>', '>Addition Constant:</string>'),
	(r'>DE Tweak</string>', '>DE Tweak:</string>'),
	(r'>Post_scale</string>', '>Post Scale:</string>'),
	(r'>Pre_scale</string>', '>Pre Scale:</string>'),
	(r'class="QCheckBox"', 'class="MyCheckBox"'),
]

# untranslatable and remove double point
NOTR_REMOVE_DOUBLE_POINT_CI = [
	r'z\.x', r'z\.y', r'z\.z', r'z\.w',
	r'c\.x', r'c\.y', r'c\.z', r'c\.w',
	'xyz', 'xzy', 'yxz', 'yzx', 'zxy', 'zyx',
	'x', 'y', 'z', 'w',
	'xy', 'yx', 'xz', 'zx', 'yz', 'zy',
	'xw', 'yw', 'zw',
	'alpha', 'beta', 'gamma',
	'fabs', r'fabs\(z\.x\)', r'fabs\(z\.y\)', r'fabs\(z\.z\)', r'fabs\(z\)',
	'abs', r'abs\(x\)', r'abs\(y\)', r'abs\(z\)',
	'T1', 'T1mod', 'T2', 'T3', 'T4', 'T5b',
	'asin', 'acos', 'atan', 'atan2',
]

# simple find and replace whole string
SIMPLE_REPLACE = [
	('Type 4 Fold Value', 'Type 4 Fold value:'),
	('Type 5 Fold2 Value', 'Type 5 Fold2 value:'),
]


# fractalforums.com is only partly online (fractalforums.org has a read-only copy and the active
# forum), so references to it are shown as text instead of links
FRACTALFORUMS_COM = re.compile(r'(?<![\w-])fractalforums\.com(?![\w.-])', re.I)


def is_text_reference(ref, previous_ref_is_text):
	"""True for a fractalforums.com reference and for the lines that continue it: a long address is
	wrapped over several comment lines, the next lines have no spaces and no scheme"""
	if FRACTALFORUMS_COM.search(ref):
		return True
	continued = ref.strip()
	return previous_ref_is_text and continued != '' and not re.search(r'\s|://', continued)


def reference_html(ref, previous_ref_is_text):
	if is_text_reference(ref, previous_ref_is_text):
		return ref
	return '<a href="' + ref + '">' + ref + '</a>'


def update_info_boxes(index, formula, status):
	"""update information boxes in the ui"""
	if index == 'custom':
		return True  # nothing to do for the custom opencl formula (ui is edited by hand)
	formatted_escaped_code = get_format_code(formula['code'])
	# remove hardcoded font-size, to use system defined font-size
	formatted_escaped_code = formatted_escaped_code.replace('font-size:10pt;', '')

	# extract only body element, since html part and comment change between versions of highlight
	start_code = strpos(formatted_escaped_code, '<body') or 0
	end_code = (strpos(formatted_escaped_code, '</body>') or 0) + len('</body>')
	formatted_escaped_code = substr(formatted_escaped_code, start_code, end_code - start_code)

	comment = formula['comment']
	information_text = ''
	if comment.get('description'):
		information_text = '<p>' + '<br>'.join(comment['description']) + '</p>'
	information_text += '<table>' + NL
	# information_text += '<tr><th>Name</th><td>' + formula['nameInComboBox'] + '</td></tr>' + NL

	if comment.get('reference'):
		information_text += '<tr><th>Reference</th><td>'
		previous_ref_is_text = False
		for ref in comment['reference']:
			information_text += reference_html(ref, previous_ref_is_text) + '<br>' + NL
			previous_ref_is_text = is_text_reference(ref, previous_ref_is_text)
		information_text += '</td></tr>' + NL
	if comment.get('author'):
		information_text += '<tr><th>Author</th><td>'
		for author in comment['author']:
			information_text += author + '<br>' + NL
		information_text += '</td></tr>' + NL
	information_text += '</table>' + NL
	formula_id = formula['id'] if formula['id'] is not None else ''
	if formula_id in formula_example_usage:
		information_text += '<br><b>Examples using this formula</b><br>'
		example_filenames = list(formula_example_usage[formula_id])
		if len(example_filenames) > 5:  # do not show more than 5 examples
			del example_filenames[5:]
			example_filenames.append('...')
		information_text += '<br>'.join(example_filenames)
	else:
		if is_warning():
			status.append(warning_string('formula ' + formula['nameInComboBox']
				+ ' is not used in any examples yet.'))

	information_text += '<h3>Code</h3>' + NL

	ui_file_content = read_file(formula['uiFile']) or ''
	regex_information = r'(<widget class="MyGroupBox" name="groupCheck_info">[\s\S]+?)<item>[\s\S]+?</layout>'

	replacement = UI_CODE_REPLACEMENT % (htmlspecialchars(information_text),
		htmlspecialchars(formatted_escaped_code))
	new_ui_file_content, count = preg_replace(regex_information, replacement, ui_file_content)
	new_ui_file_content = preg_replace1(r'<class>.+</class>', '<class>' + formula['internalName'] + '</class>',
		new_ui_file_content, 1)
	postition_of_window_title = strpos(new_ui_file_content, 'windowTitle ')
	postition_of_window_title_string_start = strpos(new_ui_file_content, '<string', postition_of_window_title) or 0
	postition_of_window_title_string_end = (strpos(new_ui_file_content, '</string>', postition_of_window_title)
		or 0) + len('</string>')
	new_ui_file_content = substr_replace(new_ui_file_content,
		'<string notr="true">' + formula['internalName'] + '</string>',
		postition_of_window_title_string_start,
		postition_of_window_title_string_end - postition_of_window_title_string_start)
	if count == 0:
		status.append(error_string('Warning, could not replace code in ui file for index: ' + index) + NL)
		return False

	for find, replace in REPLACE_FORMULA_LOOKUP:
		new_ui_file_content = preg_replace1(find, replace, new_ui_file_content)

	for item in NOTR_REMOVE_DOUBLE_POINT_CI:
		new_ui_file_content = preg_replace1('<string>' + item + ':*;*</string>',
			'<string notr="true">' + item.replace('\\', '') + '</string>', new_ui_file_content, flags=re.I)

	for find, replace in SIMPLE_REPLACE:
		new_ui_file_content = new_ui_file_content.replace(find, replace)

	if new_ui_file_content == ui_file_content:
		return True
	if not is_dry_run():
		write_file(formula['uiFile'], new_ui_file_content)
	status.append(notice_string('ui file changed'))  # + ' (' + basename(formula['uiFile']) + ')'
	return True


# --- opencl formulas -----------------------------------------------------------------------------

def generate_formula_opencl_files(formula, status):
	"""generate opencl formulas"""
	if formula['internalName'] == 'custom':
		return True  # custom.cl is user provided code, must not be autogenerated
	# TODO add primitives
	mod_interval = get_modification_interval(formula['openclFile'], True)
	file_header = ('/**' + NL
		+ ' * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______    ' + NL
		+ ' * Copyright (C) ' + mod_interval
		+ ' Mandelbulber Team   _>]|=||i=i<,     / __ \\___  ___ ___  / ___/ /' + NL
		+ ' *                                        \\><||i|=>>%)    / /_/ / _ \\/ -_) _ \\/ /__/ /__' + NL
		+ ' * This file is part of Mandelbulber.     )<=i=]=|=i<>    \\____/ .__/\\__/_//_/\\___/____/' + NL
		+ ' * The project is licensed under GPLv3,   -<>>=|><|||`        /_/                       ' + NL
		+ ' * see also COPYING file in this folder.    ~+{i%+++' + NL
		+ ' * ' + formula['rawComment'].replace('/**', '').replace('*/', '') + NL
		+ ' * This file has been autogenerated by tools/populate_ui_information.py' + NL
		+ ' * from the file "' + formula['definitionFileName'] + '" in the folder formula/definition' + NL
		+ ' * D O    N O T    E D I T    T H I S    F I L E !' + NL
		+ ' */' + NL)

	opencl_content = read_file(formula['openclFile']) or ''
	new_opencl_content = file_header + NL
	new_opencl_content += formula['openclCode']

	# clang-format
	filepath_temp = PROJECT_PATH + '/tools/.tmp.c'
	write_file(filepath_temp, new_opencl_content)
	try:
		subprocess.run([CLANG_FORMAT_EXEC_PATH, '-i', '--style=file', filepath_temp])
		new_opencl_content = read_file(filepath_temp) or ''
	finally:
		os.remove(filepath_temp)  # nothing to see here :)

	new_opencl_content_without_date_line = preg_replace1(r'Copyright\s\(C\)\s\d+', '', new_opencl_content)
	opencl_content_without_date_line = preg_replace1(r'Copyright\s\(C\)\s\d+', '', opencl_content)

	if new_opencl_content_without_date_line == opencl_content_without_date_line:
		return True
	if not is_dry_run():
		write_file(formula['openclFile'], new_opencl_content)
		# write_file(formula['openclFile'] + '.orig', formula['code'])
	status.append(notice_string('opencl file changed'))  # + ' (' + basename(formula['openclFile']) + ')'
	return True


# --- formula icons -------------------------------------------------------------------------------

def generate_formula_icons(formula, status):
	"""generate formula icons"""
	# autogenerate missing formula and transform images
	img_path = PROJECT_PATH + 'formula/img/' + formula['internalName'] + '.png'
	if os.path.exists(img_path):
		return True

	if not is_dry_run():
		if generate_formula_icon(formula, img_path):
			status.append(success_string('image generated.'))
			return True
		else:
			status.append(notice_string('image not generated. Maybe Mandelbulber not found?'))
			return True
	else:
		status.append(notice_string('image does not exist'))
		return True


def check_opencl_compile(formula, status):
	check_opencl_compile_cmd = [CLANG_EXEC_PATH, '-c', '-S', '-emit-llvm', '-o', 'test.ll', '-w',
		'-include', 'clc/clc.h', '-Dcl_clang_storage_class_specifiers', '-x', 'cl']
	check_opencl_compile_cmd += ['-include', PROJECT_PATH + 'opencl/cl_kernel_include_headers.h']
	check_opencl_compile_cmd += ['-o', os.devnull]  # -S -emit-llvm
	check_opencl_compile_cmd += ['-DOPENCL_KERNEL_CODE', '-I' + PROJECT_PATH + 'opencl/']
	check_opencl_compile_cmd += [formula['openclFile']]
	try:
		result = subprocess.run(check_opencl_compile_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		ret = result.returncode
		text = result.stdout.decode(*ENCODING.values()).replace('\r\n', '\n')
	except OSError as e:
		ret = 127
		text = str(e) + NL
	# like php's exec(): one entry per line, without trailing whitespace
	output = [line.rstrip(' \t\n\r\x0b\x0c') for line in text.split(NL)]
	if output and output[-1] == '':
		output.pop()
	if ret != 0:
		status.append(error_string('formula opencl file broken! '
			+ ('see error with verbose mode' if not is_verbose() else '')))
		if is_verbose():
			echo(print_r(output))
			status.append('1')  # the return value of php's print_r($output)
		return False
	return True


def get_format_code(code):
	# the code goes through "echo" (which adds a newline) and sed, which replaces tab with double space
	code_input = code.replace('\t', '  ') + NL
	return run([HIGHLIGHT_EXEC_PATH, '-O', 'html', '--style', 'seashell', '--inline-css', '--syntax', 'cpp'],
		input=code_input.encode(*ENCODING.values()))


def parse_comment(c):
	lines = c.split(NL)
	out = {}
	tag = 'description'
	for l in lines:
		line = trim(l)
		if line in ('/**', '*/'):
			continue
		match = preg_match(r'\*[\s]+@([\S]+)[\s]*(.*)', line)
		if match:
			tag = match.group(1)
			out.setdefault(tag, [])
			text = trim(match.group(2))
			if text != '' or len(out[tag]) > 0:
				out[tag].append(match.group(2))
			continue
		match = preg_match(r'\*[\s]+(.*)', line)
		if match:
			out.setdefault(tag, []).append(match.group(1))
		else:
			out.setdefault(tag, []).append('')
	return out


def get_formula_example_usage():
	# get a lookup for every formula in which example files it is used
	example_base_path = PROJECT_PATH + 'deploy/share/mandelbulber2/examples/'
	example_file_masks = [example_base_path + '*.fract', example_base_path + '*/*.fract']
	usage = {}
	for example_file_mask in example_file_masks:
		for example_file_name in php_glob(example_file_mask):
			example_content = read_file(example_file_name) or ''
			example_content_lines = example_content.split(NL)
			path_of_file_from_examples = example_file_name.replace(example_base_path, '')

			for line in example_content_lines:
				line = trim(line)
				match = preg_match(r'^formula_\d+\s(\d+);$', line)
				if match:
					files = usage.setdefault(match.group(1), [])
					if path_of_file_from_examples not in files:
						files.append(path_of_file_from_examples)
	return usage


def get_index_id_look_up():
	# get the integer id from fractal_list.hpp for the named index of each formula
	fractal_list_header_content = read_file(PROJECT_PATH + 'formula/definition/all_fractal_list_enums.hpp') or ''
	index_id_look_up = {}
	for line in fractal_list_header_content.split(NL):
		line = trim(line)
		match = preg_match(r'^(\w+)\s=\s(\d+),.*$', line)
		if match:
			index_id_look_up[match.group(1)] = match.group(2)
	return index_id_look_up


def parse_to_opencl(code, mode='single'):
	# fod = 'float' if mode == 'single' else 'double'
	fod = 'REAL'
	var = r'[A-Za-z_][A-Za-z0-9\.\-\>_\[\]]*'  # regex for a var name
	function_name = r'[A-Za-z_][A-Za-z0-9\.\-\>_]*'  # regex for a function name
	double = r'(?:-?\d+\.?\d*(?:[eE][+-]?\d+)?|-?\d*\.?\d+(?:[eE][+-]?\d+)?)'  # regex for a double value
	float_ = double + 'f'  # regex for a float value
	nb = r'[^()]*'  # matches everything but braces
	br = r'-?(?:' + function_name + r')?\(' + nb + r'\)'  # regex for a simply braced expression with optional function invocation
	br1_1 = r'\(' + nb + br + nb + r'\)'  # regex for a double braced expression (one inner brace)
	br1_2 = r'\(' + nb + br + nb + br + nb + r'\)'  # regex for a double braced expression (two inner braces)
	s = r'[\n\r\s]+'  # any whitespace including new lines
	all_ = r'[\S\s]+?'  # anything including new lines as few as possible (mark end with next operator)
	rval = br1_2 + '|' + br1_1 + '|' + br + '|' + float_ + '|-?' + var  # any of those types can be an "assignable expression"
	pre_f = r'\s|\(|\{|-'  # these chars can occur befire a function
	mult_chain = '(?:(?:' + rval + ')' + s + r'\*' + s + ')*(?:' + rval + ')'  # a chain of multiplicated expressions
	single = mode == 'single'
	# see here for all possible: https://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/mathFunctions.html
	cpp_to_opencl_replace_lookup = [
		(r'double', fod),  # all doubles to floats
		(r'([\s\(\{])(' + double + r')([\s\)\};,])', '$1$2' + ('f' if single else '') + '$3'),  # double literals to float literals
		('(' + pre_f + r')sin\(', '$1native_sin('),  # native sin
		('(' + pre_f + r')cos\(', '$1native_cos('),  # native cos
		# ('(' + pre_f + r')pow\(', '$1native_powr('),  # native pow
		('(' + pre_f + r')sqrt\(', '$1native_sqrt('),  # native sqrt
		(r'CVector3\((' + mult_chain + ',' + s + mult_chain + ',' + s + mult_chain + r')\)', '(' + fod + '3) {$1}'),  # CVector3 to built in float3
		(r'CVector3\(\)', '(' + fod + '3) {0, 0, 0}'),  # CVector3 default constructor to built in float3
		(r'CVector3\((' + all_ + r')\);', '(' + fod + '3) {$1};'),  # CVector3 to built in float3
		(r'CVector3(\s)', fod + '3$1'),  # CVector3 to built in float3
		(r'CVector4\(\)', '(' + fod + '4) {0, 0, 0, 0}'),  # CVector3 default constructor to built in float3
		(r'CVector4\((' + mult_chain + '),(' + s + mult_chain + r')\)', '(' + fod + '4) {$1.x,$1.y,$1.z,$2}'),  # CVector4 construct by CVector3 and float to built in construct
		(r'CVector4\((' + mult_chain + ',' + s + mult_chain + ',' + s + mult_chain + ',' + s + mult_chain + r')\)', '(' + fod + '4) {$1}'),  # CVector4 to built in float4
		(r'CVector4\((' + all_ + r')\);', '(' + fod + '4) {$1};'),  # CVector4 to built in float4
		(r'CVector4(\s)', fod + '4$1'),  # CVector4 to built in float4
		(r'\(([^()]*)\)\.Length\(\)', 'length($1)'),  # Length() of a braced expression to built in length
		('(' + var + r')\.Length\(\)', 'length($1)'),  # CVector3 Length() to built in length
		(r'\[\[maybe_unused\]\]\s*', ''),  # C++ attributes are not valid in OpenCL C
		('(' + var + r')\.Dot\(', 'dot($1, '),  # CVector3 Dot() to built in dot
		('(' + var + r')\.Cross\(', 'cross($1, '),  # CVector3 Cross() to built in cross
		('(' + var + r')\.RotateVector\(', 'Matrix33MulFloat4($1, '),  # CRotationMatrix33 to custom rotation function
		('(' + var + r')\.RotateX\(', '$1 = RotateX($1, '),  # CRotationMatrix33 to custom rotation function
		('(' + var + r')\.RotateY\(', '$1 = RotateY($1, '),  # CRotationMatrix33 to custom rotation function
		('(' + var + r')\.RotateZ\(', '$1 = RotateZ($1, '),  # CRotationMatrix33 to custom rotation function
		('(' + var + r')\.RotateAroundVectorByAngle\(', 'RotateAroundVectorByAngle4($1, '),  # CVector3 to custom rotation function
		('(' + var + r')\.GetXYZ\(\)', '$1.xyz'),  # CVector4 getxyz to native accessor of float4
		('CRotationMatrix' + s + '(' + var + ');', 'matrix33 $1;' + NL + '$1.m1 = (float3){1.0f, 0.0f, 0.0f};' + NL
			+ '$1.m2 = (float3){0.0f, 1.0f, 0.0f};' + NL + '$1.m3 = (float3){0.0f, 0.0f, 1.0f};'),  # CRotationMatrix33 to matrix33
		(r'swap\((' + var + r'),\s(' + var + r')\);', '{ ' + fod + ' temp = $1; $1 = $2; $2 = temp; }'),  # swap vals
		('(' + s + r'|\()(-?\d+)f(' + s + r'|;|\))', '$1$2$3'),  # int vals should not have a "f" at the end
		# (r'sign\((' + rval + r')\)' + s + r'\*' + s + '(' + mult_chain + ')', 'copysign($2, $1)'),  # sign(x) * y => copysign(y, x) (this is wrong! probably copysign not usable at all)

		# from here on its getting messy
		# ('1.0f' + s + '/' + s + '(' + rval + ')', 'native_recip($1)'),  # native reciprocal
		# ('(' + rval + ')' + s + '/' + s + '(' + rval + ')', 'native_divide($1, $2)'),  # native division
		('(' + pre_f + r')native_recip\(native_sqrt(' + rval + r')\)', '$1native_rsqrt$2'),  # native reciprocal sqrt
		# ('(' + pre_f + r')native_sqrt\(native_recip(' + rval + r')\)', '$1native_rsqrt$2'),  # native reciprocal sqrt

		# mad (literally ;D )
		# (r'\((' + mult_chain + ')' + s + r'\*' + s + '(' + rval + ')' + s + r'\+' + s + '(' + mult_chain + r')\)', '(mad($1, $2, $3))'),  # (a * b + c) ====> mad(a, b, c)
		# (r'\((' + mult_chain + ')' + s + r'\*' + s + '(' + rval + ')' + s + r'\-' + s + '(' + mult_chain + r')\)', '(mad($1, $2, -$3))'),  # (a * b - c) ====> mad(a, b, -c)
		# ('([^*-]' + s + ')(' + mult_chain + ')' + s + r'\*' + s + '(' + rval + ')' + s + r'\+' + s + '(' + mult_chain + ')(' + s + '[^*]|;)', '$1mad($2, $3, $4)$5'),  # a * b + c ====> mad(a, b, c)
		# ('([^*-]' + s + ')(' + mult_chain + ')' + s + r'\*' + s + '(' + rval + ')' + s + r'\-' + s + '(' + mult_chain + ')(' + s + '[^*]|;)', '$1mad($2, $3, -$4)$5'),  # a * b - c ====> mad(a, b, -c)
		# (r'\((' + mult_chain + ')' + s + r'\+' + s + '(' + rval + ')' + s + r'\*' + s + '(' + mult_chain + r')\)', '(mad($2, $3, $1))'),  # (c + a * b) ====> mad(a, b, c)
		# (r'\((' + mult_chain + ')' + s + r'\-' + s + '(' + rval + ')' + s + r'\*' + s + '(' + mult_chain + r')\)', '(mad(-$2, $3, $1))'),  # (c - a * b) ====> mad(-a, b, c)
		# ('([^*-]' + s + ')(' + mult_chain + ')' + s + r'\+' + s + '(' + mult_chain + ')' + s + r'\*' + s + '(' + rval + ')(' + s + '[^*]|;)', '$1mad($4, $3, $2)$5'),  # a * b + c ====> mad(a, b, c)
		# ('([^*-]' + s + ')(' + mult_chain + ')' + s + r'\-' + s + '(' + mult_chain + ')' + s + r'\*' + s + '(' + rval + ')(' + s + '[^*]|;)', '$1mad(-$4, $3, $2)$5'),  # c - a * b ====> mad(-a, b, c)

		# formula specific replacements
		(r'^void(\s)', fod + '4 $1'),  # mark void with inline void
		(fod + '4 &z', fod + '4 z'),  # no passing by reference
		# (fod + r' \&w', fod + ' *w'),  # no passing by reference
		# (r'z\.', 'z->'),
		(fod + '4 &z4D', fod + '4 *z4D'),  # no passing by reference
		(r'z4D\.', 'z4D->'),
		(r'sExtendedAux &aux', 'sExtendedAuxCl *aux'),  # no passing by reference
		(r'const sFractal \*fractal', '__global sFractalCl *fractal'),  # no passing by reference
		(r'aux\.', 'aux->'),
		(r'const(\s\w+\s\*)', '__global$1'),  # const modifier allowed for pointers (vars from outside)
		(r'const(\s)', '$1'),  # const modifier not allowed for local vars
		# (r'(\s)z\s=', '$1*z ='),  # z to pointer
		# (r'(\s)z\s(.)=', '$1*z $2='),  # z to pointer
		# (r'([\s\(-])z([,\);\s}])', '$1*z$2'),  # z to pointer
		(r'(\s)z4D\s=', '$1*z4D ='),  # z4D to pointer
		(r'(\s)z4D\s(.)=', '$1*z4D $2='),  # z4D to pointer
		(r'([\s\(-])z4D([,\);\s}])', '$1*z4D$2'),  # z4D to pointer
		# (r'(\s)w\s=', '$1*w ='),  # w to pointer
		# (r'(\s)w\s(.)=', '$1*w $2='),  # w to pointer
		# (r'([\s\(-])w([,\);\s}])', '$1*w$2'),  # w to pointer
		(r'case ([a-zA-Z]+[a-zA-Z0-9_]+?[^l])(_[a-zA-Z0-9]+):', 'case $1Cl$2:'),  # replace enum switch cases with cl version
		(r'== ([a-zA-Z]+[a-zA-Z0-9_]+?[^l])(_[a-zA-Z0-9]+)\)', '== $1Cl$2)'),  # replace enum if comparison with cl version
		('(' + s + r')(enum[a-zA-Z0-9_]+?[^l])(' + s + ')', '$1$2Cl$3'),  # replace enum definitions with cl version
	]
	# replace Math constants
	for constant in ['M_PI', 'M_PI_180', 'M_PI_8', 'M_PI_4', 'M_PI_2', 'M_PI_2x', 'M_PI_2x_INV', 'SQRT_1_3',
			'SQRT_1_2', 'SQRT_2_3', 'SQRT_3_2', 'SQRT_3_4', 'SQRT_3_4d2', 'SQRT_3_4d2', 'SQRT_3', 'FRAC_1_3']:
		cpp_to_opencl_replace_lookup.append(
			(constant + r'([\s\)\};,])', constant + ('_F$1' if single else '$1')))
	cpp_to_opencl_replace_lookup += [
		(r'1e-061f', '1e-030f' if single else '1e-061'),  # replace minimal double constant
		(r'1e-021f', '1e-006f' if single else '1e-021'),  # replace minimal double constant
		(r'reinterpret_cast<(.*?)>\((.*?)\)', '($1)$2'),  # replace reinterpret_cast with simple cast
		# TODO more replacements
	]

	for find, replace in cpp_to_opencl_replace_lookup:
		code = preg_replace1(find, replace, code)
		code = preg_replace1(find, replace, code)  # regex sometimes overlap, so run twice!
	code = code[:-1] + 'return z;' + NL + '}'
	return code


def generate_formula_icon(formula, img_path):
	formula_id = formula['id'] if formula['id'] is not None else ''
	if formula['type'] == 'transf':
		settings = '''# Mandelbulber settings file
# version 2.09
# only modified parameters
[main_parameters]
ambient_occlusion_enabled true;
camera 1,079630935663999 -2,590704819137228 1,163428502911517;
camera_distance_to_target 7,000000000000004;
camera_rotation 23,56505117707793 -24,60154959902026 0;
camera_top -0,1664347300812756 0,3815883285622319 0,9092248501486611;
flight_last_to_render 0;
formula_1 7;
formula_2 ''' + formula_id + ''';
hdr true;
hybrid_fractal_enable true;
image_height 256;
image_width 256;
keyframe_last_to_render 0;
target -1,464862100002681 3,2431066939054 -1,750709177730066;
[fractal_1]
[fractal_2]
info true;
mandelbox_folding_limit -0,5;
mandelbox_folding_value 0;
transf_addition_constant_0000 0,1 -0,5 0,1 -0,2;
transf_addition_constant_111 0,135135 1 1;
transf_function_enabledAz_false true;
transf_int_A 5;
transf_int_B 5;
transf_offset 0,7;
transf_offset_0 0,8;'''
	else:
		settings = '''# Mandelbulber settings file
# version 2.09
# only modified parameters
[main_parameters]
ambient_occlusion_enabled true;
camera 1,080915186153033 -1,903722297292515 1,202015809937754;
camera_distance_to_target 6,277124159763113;
camera_rotation 29,9681686749304 -29,10022837829406 0;
camera_top -0,2429354017345652 0,421316840960071 0,8737702845184721;
DE_factor 0,18197;
flight_last_to_render 0;
formula_1 ''' + formula_id + ''';
hdr true;
hybrid_fractal_enable true;
image_height 256;
image_width 256;
keyframe_last_to_render 0;
mat1_coloring_palette_offset 1,6;
target -1,65882778581033 2,847745976777177 -1,850793618305075;
[fractal_1]
transf_function_enabled_false true;
[fractal_2]
info true;
mandelbox_folding_limit -0,5;
mandelbox_folding_value 0;
transf_addition_constant 1,935484 0 0;
transf_addition_constant_0000 0,1 -0,5 0,1 -0,2;
transf_constant_multiplier_111 2,934272 2,089201 1,877934;
transf_function_enabledy_false true;
transf_function_enabledz_false true;
transf_int_A 5;
transf_int_B 5;
transf_offset 0,7;
transf_offset_0 0,8;
transf_scale_0 0,034722;
transf_scale_2 1,079812;'''
	temp_fract_path = PROJECT_PATH + 'formula/img/' + formula['internalName'] + '.fract'

	if not os.path.exists(temp_fract_path):  # allow manual override
		write_file(temp_fract_path, settings)
	mandelbulber = shutil.which(MANDELBULBER_EXEC_PATH)
	if mandelbulber is None:
		return False
	cmd = [mandelbulber, '-n', '-f', 'png16alpha', '-O', 'opencl_enabled=0', '-o', img_path, temp_fract_path]
	# echo(NL + ' '.join(cmd) + NL)
	run(cmd)
	run([CONVERT_EXEC_PATH, img_path, '-depth', '8', img_path])  # save disk space with 8-bit png
	return True


def git_mv(source, target):
	run(['git', 'mv', source, target], cwd=PROJECT_PATH)


def upgrade_internal_name(internal_name, internal_name_new):
	for folder, prefix, extension in [('formula/ui/', '', '.ui'), ('formula/img/', '', '.png'),
			('formula/definition/', 'fractal_', '.cpp'), ('formula/opencl/', '', '.cl'),
			('formula/img/', '', '.fract')]:
		if os.path.exists(PROJECT_PATH + folder + prefix + internal_name + extension):
			git_mv(PROJECT_PATH + folder + prefix + internal_name + extension,
				PROJECT_PATH + folder + prefix + internal_name_new + extension)
	definition_file = PROJECT_PATH + 'formula/definition/fractal_' + internal_name_new + '.cpp'
	fractal_list_content = read_file(definition_file) or ''
	fractal_list_content = fractal_list_content.replace('"' + internal_name + '"', '"' + internal_name_new + '"')
	write_file(definition_file, fractal_list_content)


def upgrade_function_name(function_name, function_name_new):
	replace_in_files = [
		PROJECT_PATH + 'src/compute_fractal.cpp',
		PROJECT_PATH + 'src/fractal_formulas.cpp',
		PROJECT_PATH + 'src/fractal_formulas.hpp',
		PROJECT_PATH + 'src/fractal_list.cpp',
	]
	for replace_in_file in replace_in_files:
		file_content = read_file(replace_in_file) or ''
		file_content = file_content.replace(function_name, function_name_new)
		write_file(replace_in_file, file_content)


def from_camel_case(input_):
	input_ = input_.replace('dIFS', 'difs')  # explicitly keep those
	ret = [m.group(0) for m in re.finditer(r'([A-Z][A-Z0-9]*(?=$|[A-Z][a-z0-9])|[A-Za-z][a-z0-9]+)', input_)]
	ret = [strtolower(match) if match == strtoupper(match) else lcfirst(match) for match in ret]
	return '_'.join(ret)


# --- csv, parameter names and fractal definitions ------------------------------------------------

def csv_line(fields):
	"""a line like php's fputcsv() writes it (delimiter , enclosure " escape \\)"""
	out = []
	for field in fields:
		field = '' if field is None else str(field)
		if any(c in field for c in ',"\\\n\r\t '):
			quoted = '"'
			escaped = False
			for c in field:
				if c == '\\':
					escaped = True
				elif not escaped and c == '"':
					quoted += '"'
				else:
					escaped = False
				quoted += c
			field = quoted + '"'
		out.append(field)
	return ','.join(out) + NL


def write_formula_csv(formulas):
	content = csv_line([
		'index',
		'name',
		'deType',
		'deFunctionType',
		'pixelAddition',
		'defaultBailout',
		'analyticFunction',
		'coloringFunction',
	])
	for index, formula in formulas.items():
		data = [
			index,
			formula.get('nameInComboBox'),
			# formula.get('internalName'),
			# formula.get('functionName'),
			formula.get('DEType'),
			formula.get('DEFunctionType'),
			formula.get('cpixelAddition'),
			formula.get('defaultBailout'),
			formula.get('DEAnalyticFunction'),
			formula.get('coloringFunction'),
		]
		content += csv_line(data)
	write_file(PROJECT_PATH + 'deploy/formulaData.csv', content)


def write_parameter_names_txt():
	fractal_cpp = read_file(PROJECT_PATH + 'src/fractal.cpp') or ''
	parameter_names = {}
	min_i = max_i = None
	for match in re.finditer(r'(.*?)\s=[\s\n]+.*container->Get<(.*?)>\("(.*?)"(, i)?\).*;', fractal_cpp, re.ASCII):
		parameter = trim(match.group(1))
		if '//' in parameter:
			continue  # skip comments
		main_parameter = parameter.split('.')[0]
		type_ = {'double': 'REAL', 'CVector3': 'REAL3', 'CVector4': 'REAL4'}.get(match.group(2), match.group(2))
		name = match.group(3)
		if not match.group(4):
			parameter_names.setdefault(main_parameter, []).append(parameter + ' ' + type_ + ' ' + name)
		else:
			if 'mandelbox.rot' in parameter:
				min_i = 1
				max_i = 4
			if 'IFS' in parameter:
				min_i = 0
				max_i = 9
			for i in (range(min_i, max_i) if min_i is not None else []):
				parameter_run = parameter
				parameter_run = parameter_run.replace('[i - 1]', '[' + str(i - 1) + ']')
				parameter_run = parameter_run.replace('[i]', '[' + str(i) + ']')
				type_run = type_.replace('REAL3', 'matrix33')
				parameter_names.setdefault(main_parameter, []).append(
					parameter_run + ' ' + type_run + ' ' + name + '_' + str(i))
	# legacy per-fractal params migrated to node level (see objects_tree.cpp), kept for the custom formula editor
	# mapped by the name of the bucket after which they should be inserted, to preserve the file layout
	legacy_params = {
		'formulaStopIteration': [
			'juliaMode bool julia_mode',
			'juliaConstant REAL3 julia_c',
			'constantMultiplier REAL3 fractal_constant_factor',
			'initialWAxis REAL initial_waxis',
		],
		'checkForBailout': [
			'formulaMaxiter int formula_maxiter',
			'formulaMaterialId int formula_material_id',
		],
	}
	txt_file = ''
	for parameter_name, parameter_lines in parameter_names.items():
		txt_file += NL.join(parameter_lines) + NL + NL
		if parameter_name in legacy_params:
			txt_file += (NL + NL).join(legacy_params[parameter_name]) + NL + NL

	# derived parameters
	for match in re.finditer(r'(.*\..*?)\s=[\s\n]([A-Za-z0-9.+*/\s\(\)]*);', fractal_cpp, re.ASCII):
		line = match.group(0)
		if '//' in line or 'for (' in line:
			continue  # skip comments and loops
		txt_file += trim(match.group(1) + ' none none') + NL

	txt_file += NL
	txt_file += 'IFS.mainRot matrix33 IFS_rotation' + NL
	txt_file += 'mandelbox.mainRot matrix33 mandelbox_rotation_main' + NL
	txt_file += 'transformCommon.rotationMatrix matrix33 transf_rotation' + NL
	txt_file += 'transformCommon.rotationMatrix2 matrix33 transf_rotation2' + NL

	write_file(PROJECT_PATH + 'deploy/share/mandelbulber2/data/parameterNames.txt', txt_file)


def write_fractal_definition_file(formulas):
	out = '''/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) ''' + time.strftime('%Y') + ''' Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \\><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \\____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 */

#ifndef MANDELBULBER2_FORMULA_DEFINITION_ALL_FRACTAL_DEFINITIONS_H_
#define MANDELBULBER2_FORMULA_DEFINITION_ALL_FRACTAL_DEFINITIONS_H_

#include "abstract_fractal.h"

'''
	out += 'FRACTAL_CLASS(cFractalNone)' + NL
	for index in formulas:
		out += 'FRACTAL_CLASS(cFractal' + ucfirst(index) + ')' + NL
	out += '''
#endif /* MANDELBULBER2_FORMULA_DEFINITION_ALL_FRACTAL_DEFINITIONS_H_ */
'''
	write_file(PROJECT_PATH + 'formula/definition/all_fractal_definitions.h', out)

	# the fractal list in all_fractal_list.cpp is no longer generated:
	# out = '//====================== FRACTAL LIST - START ==================' + NL
	# for index in formulas:
	# 	out += '	newFractalList.append(new cFractal' + ucfirst(index) + '());' + NL
	# out += '	//====================== FRACTAL LIST - END =================='
	# regex = r'//=* FRACTAL LIST - START =*[\s\S]*?//=* FRACTAL LIST - END =*'


# --- main ----------------------------------------------------------------------------------------

formula_example_usage = {}


def main():
	global formula_example_usage
	if hasattr(sys.stdout, 'reconfigure'):
		sys.stdout.reconfigure(encoding='utf-8', errors='surrogateescape')  # the tree characters, also on Windows
	for name, program, variable in [('highlight', HIGHLIGHT_EXEC_PATH, 'HIGHLIGHT'),
			('clang-format', CLANG_FORMAT_EXEC_PATH, 'CLANG_FORMAT')]:
		if shutil.which(program) is None:
			sys.exit(error_string(name + ' not found: ' + program + ' (set ' + variable + ' to its path)'))
	print_start()

	print_start_group('READING FORMULA DATA')
	formulas = get_formulas_data()
	formula_example_usage = get_formula_example_usage()

	print_start_group('RUNNING FORMULA CHECKS')
	i = 0
	for index, formula in formulas.items():
		i += 1
		success = True
		status = []
		if success:
			success = update_info_boxes(index, formula, status)
		if success:
			success = generate_formula_opencl_files(formula, status)
		if success:
			success = generate_formula_icons(formula, status)
		if success and argument_contains('checkCl'):
			success = check_opencl_compile(formula, status)
		print_result_line(formula['nameInComboBox'], success, status, i / len(formulas))
	print_end_group()
	if not is_dry_run():
		write_formula_csv(formulas)
	if not is_dry_run():
		write_parameter_names_txt()
	if not is_dry_run():
		write_fractal_definition_file(formulas)
	print_finish()
	return 0


if __name__ == '__main__':
	sys.exit(main())
