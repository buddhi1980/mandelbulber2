#!/usr/bin/env python3
#
# this file checks the source and header files
# It is a port of the former codeStyleCheck.php and behaves the same.
# requires Python 3.6 or newer, clang-format and git
# clang-format is required in version 3.8.1, get executable from here: https://releases.llvm.org/download.html
# set CLANG_FORMAT to the path of clang-format when it is not on PATH
#
# on default this script runs dry,
# it will try to parse all files and prints problems inside the files
# this should always be run first, to see if any issues occur
# if you invoke this script with "nondry" as cli argument it will write changes to the project files
# with "verbose" every file is listed, with "checkTidy" clang-tidy runs after the checks
# (needs cmake, run-clang-tidy and clang-apply-replacements; set RUN_CLANG_TIDY and
# CLANG_APPLY_REPLACEMENTS to their paths)
#
# usage: mandelbulber2/tools/code_style_check.py [nondry] [verbose] [checkTidy]

import decimal
import fnmatch
import math
import os
import re
import shutil
import subprocess
import sys
import time

# the same form as in the PHP tool: ".../mandelbulber2/tools/../", always with "/"
PROJECT_PATH = os.path.realpath(os.path.dirname(os.path.abspath(__file__))).replace(os.sep, '/') + '/../'
START_TIME = time.time()
CLANG_FORMAT_EXEC_PATH = os.environ.get('CLANG_FORMAT', 'clang-format')
RUN_CLANG_TIDY_EXEC_PATH = os.environ.get('RUN_CLANG_TIDY', '/usr/bin/run-clang-tidy-6.0.py')
CLANG_APPLY_BINARY_PATH = os.environ.get('CLANG_APPLY_REPLACEMENTS', '/usr/bin/clang-apply-replacements-6.0')
NL = '\n'

# the files are handled as UTF-8, undecodable bytes are kept as they are
ENCODING = 'utf-8'
ERRORS = 'surrogateescape'

# the characters which trim() removes in PHP
PHP_WHITESPACE = ' \t\n\r\0\x0b'

# the regexes are taken over from PHP (PCRE without the u flag): \s, \w and \d are ASCII only
RE_FLAGS = re.ASCII

FILES_TO_CHECK_SOURCE = [
	('src/', '.cpp'),
	('src/', '.c'),
	('qt/', '.cpp'),
	('qt/', '.c'),
	('opencl/', '.cpp'),
	('opencl/', '.c'),
	('opencl/engines/', '.cl'),
	# as in the PHP tool, the opencl headers are checked like source files (no include guard check)
	('opencl/', '.hpp'),
	('opencl/', '.h'),
]

FILES_TO_CHECK_HEADER = [
	('src/', '.hpp'),
	('src/', '.h'),
	('qt/', '.hpp'),
	('qt/', '.h'),
]


# --- PHP helpers ---------------------------------------------------------------------------------

def php_strlen(s):
	"""length in bytes, as strlen() in PHP"""
	return len(s.encode(ENCODING, ERRORS))


def str_pad(s, length, left=False):
	"""pads to a length in bytes, as str_pad() in PHP"""
	padding = ' ' * max(0, length - php_strlen(s))
	return padding + s if left else s + padding


def php_trim(s):
	return s.strip(PHP_WHITESPACE)


def php_rtrim(s):
	return s.rstrip(PHP_WHITESPACE)


def php_empty(s):
	"""empty() in PHP, for strings"""
	return s in ('', '0')


def php_basename(path):
	path = path.rstrip('/')
	return path[path.rfind('/') + 1:]


def php_filename(path):
	"""pathinfo($path, PATHINFO_FILENAME) in PHP"""
	name = php_basename(path)
	dot = name.rfind('.')
	return name[:dot] if dot != -1 else name


def php_strtoupper(s):
	return ''.join(chr(ord(c) - 32) if 'a' <= c <= 'z' else c for c in s)


def number_format(value, decimals):
	"""number_format() in PHP: rounds half away from zero, with thousands separators"""
	rounded = decimal.Decimal(repr(value)).quantize(decimal.Decimal(1).scaleb(-decimals),
		rounding=decimal.ROUND_HALF_UP)
	return '{:,.{}f}'.format(rounded, decimals)


def current_year():
	# date('Y') in PHP, which uses UTC when no time zone is configured
	return time.strftime('%Y', time.gmtime())


def read_file(path):
	with open(path, 'rb') as f:
		return f.read().decode(ENCODING, ERRORS)


def write_file(path, content):
	with open(path, 'wb') as f:
		f.write(content.encode(ENCODING, ERRORS))


def find_executable(name):
	# also finds .bat and .cmd files on Windows, as the shell does
	return shutil.which(name) or name


def glob_files(patterns):
	"""glob() with GLOB_BRACE in PHP: the matches of each pattern, sorted, one pattern after the other"""
	files = []
	for folder, extension in patterns:
		directory = PROJECT_PATH + folder
		try:
			names = os.listdir(directory)
		except OSError:
			continue
		files += [directory + name for name in sorted(names)
			if not name.startswith('.') and fnmatch.fnmatchcase(name, '*' + extension)]
	return files


# --- console output ------------------------------------------------------------------------------

def echo(s):
	# raw bytes, as PHP writes them: no newline translation, the tree characters also on Windows consoles
	out = sys.stdout.buffer
	out.write(s.encode(ENCODING, ERRORS))
	out.flush()


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
	text += '/ ' + progress_string(str_pad(number_format(percent * 100, 1), 4, left=True) + '%')
	text += notice_string(']')
	return text


def print_start_group(title):
	echo(NL + '\033[1m\033[44m' + ' ' + str_pad(title, 54) + ' ' + '\033[0m' + NL)


def print_end_group():
	echo('\033[2K\r')
	echo(' ╹' + NL)


def error_string(s):
	return '\033[1;31m' + s + '\033[0m'


def success_string(s):
	return '\033[1;32m' + s + '\033[0m'


def notice_string(s):
	return '\033[1;34m' + s + '\033[0m'


def progress_string(s):
	return '\033[1;35m' + s + '\033[0m'


def warning_string(s):
	return '\033[1;33m' + s + '\033[0m'


def is_dry_run():
	return not argument_contains('nondry')


def is_verbose():
	return argument_contains('verbose')


def argument_contains(needle):
	return needle in sys.argv[1:]


# --- copyright years from git history -----------------------------------------------------------

_modification_intervals = None


def get_modification_intervals():
	"""first and last year in which each file was changed, from git log"""
	modification_file_info = {}
	try:
		raw = subprocess.run(['git', 'log',
			'--format=COMMIT_SEPARATOR_1____________%adCOMMIT_SEPARATOR_2____________%s', '--name-only'],
			cwd=PROJECT_PATH, stdout=subprocess.PIPE).stdout
	except OSError:
		return modification_file_info
	commit_string_raw = php_trim(raw.decode(ENCODING, ERRORS))
	for commit_string in commit_string_raw.split('COMMIT_SEPARATOR_1____________'):
		if php_empty(commit_string):
			continue
		commit_lines = commit_string.split(NL)
		meta = commit_lines[0].split('COMMIT_SEPARATOR_2____________')
		date = meta[0][max(len(meta[0]) - 10, 0):][:4]
		title = meta[1] if len(meta) > 1 else ''

		# these commits contain only auto formatting code changes
		# and should not be counted for the modification invertal
		if 'source code' in title:
			continue
		if 'nullptr' in title:
			continue
		if 'remove src dir' in title:
			continue

		for commit_line in commit_lines:
			if not php_empty(commit_line) and 'COMMIT_SEPARATOR_2____________' not in commit_line:
				entry = modification_file_info.setdefault(commit_line, {'start': date, 'end': date})
				entry['start'] = min(entry['start'], date)
				entry['end'] = max(entry['end'], date)
	return modification_file_info


def get_modification_interval(file_path, only_end_year=False):
	global _modification_intervals
	if _modification_intervals is None:
		_modification_intervals = get_modification_intervals()

	file_path_relative = file_path.replace(PROJECT_PATH, 'mandelbulber2/')
	year_start = year_end = ''
	if file_path_relative in _modification_intervals:
		year_start = _modification_intervals[file_path_relative]['start']
		year_end = _modification_intervals[file_path_relative]['end']
	if php_empty(year_start):
		return current_year()
	if php_empty(year_end) or year_start == year_end:
		return year_start
	if only_end_year:
		return year_end
	return year_start + '-' + year_end[2:4]


# --- checks --------------------------------------------------------------------------------------
# each check returns (success, new file content) and appends its messages to status

def check_file_header(file_path, file_content, status):
	header_regex = r'^(/\*\*?[\s\S]*?\*/)([\s\S]*)$'
	match_header = re.search(header_regex, file_content, RE_FLAGS)
	if match_header:
		file_header = match_header.group(1)
		# file_source_code = match_header.group(2)
		modification_string = get_modification_interval(file_path)

		if 'MKNmMMKmm' not in file_header:
			# not a recent proper file header, try other known formats
			# old format
			regex_parse_header_old = r'^[\s\S]+Mandelbulber\sv2.*[\s\S]*?(\w.*)[\S\s]+'
			regex_parse_header_old += r'General\sPublic[\S\s]+'
			regex_parse_header_old += r'Authors:\s(.*)[\s\S]*?\*/([\s\S]*)$'
			regex_parse_header_eclipse = r'^[\s\S]+Author:[\s\S]*?\*/([\s\S]*)$'
			regex_parse_header_any = r'^/\*\*?([\s\S]*?)\*/([\s\S]*)$'

			match_header_old = re.search(regex_parse_header_old, file_content, RE_FLAGS)
			if match_header_old:
				status.append(notice_string('header is old, will rewrite to new!'))
				new_file_content = get_file_header(match_header_old.group(2), match_header_old.group(1),
					modification_string) + match_header_old.group(3)
				return True, new_file_content
			match_header_eclipse = re.search(regex_parse_header_eclipse, file_content, RE_FLAGS)
			if match_header_eclipse:
				status.append(notice_string('header is in eclipse format, will rewrite to new!'))
				new_file_content = get_file_header('Krzysztof Marczak (buddhi1980@gmail.com)', 'TODO: description',
					current_year()) + match_header_eclipse.group(1)
				return True, new_file_content
			match_header_any = re.search(regex_parse_header_any, file_content, RE_FLAGS)
			if match_header_any:
				status.append(notice_string('header is unknown format, just copying contents of header!'))
				new_file_content = get_file_header('### TODO: unknown author! ###', match_header_any.group(1),
					current_year()) + match_header_any.group(2)
				return True, new_file_content
			status.append(error_string('header unknown!'))
		else:
			regex_parse_header = r'^[\s\S]+?#{50}?[\S\s]+?Authors:\s([\s\S]*?)\*\n([\s\S]*?)\*/([\s\S]*)$'
			match_header_new = re.search(regex_parse_header, file_content, RE_FLAGS)
			if match_header_new:
				new_file_content = get_file_header(match_header_new.group(1), match_header_new.group(2),
					modification_string) + match_header_new.group(3)
				if new_file_content != file_content:
					status.append(notice_string('header is new, will rewrite to new!'))
					file_content = new_file_content
				return True, file_content
			status.append(error_string('header unknown2!'))
	else:
		status.append(error_string('No header found!'))
	return False, file_content


def check_defines(file_content, header_file_name, folder_name, status):
	define_regex = r'^([\s\S]*?#ifndef\s)([\s\S]+?)(\n#define\s)(\S+)([\S\s]+)(#endif\s)[\s\S]+$'
	match = re.search(define_regex, file_content, RE_FLAGS)
	if match:
		define_name = ('MANDELBULBER2_' + php_strtoupper(folder_name) + '_'
			+ php_strtoupper(header_file_name.replace('.', '_')) + '_')
		new_file_content = (match.group(1) + define_name + match.group(3) + define_name + match.group(5)
			+ match.group(6) + '/* ' + define_name + ' */' + NL)

		if new_file_content != file_content:
			file_content = new_file_content
			status.append(notice_string('define changed'))
		return True, file_content
	status.append(error_string('define not found!'))
	return False, file_content


def check_clang_format(file_content, status):
	contents_before = file_content

	# clang-format uses the .clang-format of the project, found from the folder of the temp file
	filepath_temp = PROJECT_PATH + '/tools/.tmp.c'
	write_file(filepath_temp, file_content)
	try:
		subprocess.run([find_executable(CLANG_FORMAT_EXEC_PATH), '-i', '--style=file', filepath_temp],
			stdout=subprocess.DEVNULL)
		file_content = read_file(filepath_temp)
	finally:
		os.remove(filepath_temp)  # nothing to see here :)

	if contents_before != file_content:
		status.append(notice_string('checkClangFormat changed'))
	return True, file_content


def check_include_headers(filepath, file_content, status, folder_name):
	include_regex = r'^([\s\S]*?)(\n#include[\s\S]+)?(\n#include.*)([\s\S]+)$'
	match = re.search(include_regex, file_content, RE_FLAGS)
	if match:
		before_capture = match.group(1)
		includes_capture = (match.group(2) or '') + match.group(3)
		after_capture = match.group(4)
		# array_filter() in PHP drops the empty lines (and "0")
		includes_in = [line for line in includes_capture.split(NL) if not php_empty(line)]
		includes_out = [
			('moduleHeader', []),
			('cHeader', []),
			('cppHeader', []),
			('qtHeader', []),
			('uiAutogenHeader', []),
			('localHeader', []),
			('srcHeader', []),
			('uiHeader', []),
			('openclHeader', []),
			('formulaDefinitionHeader', []),
		]
		groups = dict(includes_out)
		custom_includes = ''
		for k, include_line in enumerate(includes_in):
			include_line = include_line.replace('../src/', 'src/')
			include_line = include_line.replace('../qt/', 'qt/')
			include_line = include_line.replace('../opencl/', 'opencl/')
			include_line = include_line.replace('qt/ui_', 'ui_')
			include_line = include_line.replace('"' + folder_name + '/', '"')  # strip folder, when include from same folder
			include_format_regex = r'^#include ([<"])([a-zA-Z0-9._/]+)([>"])$'
			if include_line == '// custom includes':
				custom_includes = NL.join(includes_in[k:])
				break
			match_include = re.search(include_format_regex, include_line, RE_FLAGS)
			if match_include:
				include_file_name = match_include.group(2)
				if match_include.group(1) == '"':
					# project includes
					if php_filename(include_file_name) == php_filename(filepath):
						groups['moduleHeader'].append(include_line)
					elif include_file_name.startswith('ui_'):
						groups['uiAutogenHeader'].append(include_line)
					elif '/' not in include_file_name:
						groups['localHeader'].append(include_line)
					elif 'src/' in include_file_name:
						groups['srcHeader'].append(include_line)
					elif 'qt/' in include_file_name:
						groups['uiHeader'].append(include_line)
					elif 'opencl/' in include_file_name:
						groups['openclHeader'].append(include_line)
					elif 'formula/definition/' in include_file_name:
						groups['formulaDefinitionHeader'].append(include_line)
					else:
						status.append(error_string('invalid include line: "' + include_line + '"'))
						return False, file_content
				elif match_include.group(1) == '<':
					# system includes
					if include_file_name[:1] == 'Q':
						groups['qtHeader'].append(include_line)
					elif include_file_name[-2:] == '.h':
						groups['cHeader'].append(include_line)
					else:
						groups['cppHeader'].append(include_line)
				else:
					status.append(error_string('invalid include line: "' + include_line + '"'))
					return False, file_content
			else:
				status.append(error_string('invalid include line: "' + include_line + '"'))
				return False, file_content
		new_includes = ''
		for _, include_group in includes_out:
			if not include_group:
				continue
			unique_includes = []
			for include_line in sorted(include_group):
				if include_line not in unique_includes:
					unique_includes.append(include_line)
			new_includes += NL + NL + NL.join(unique_includes)
		if not php_empty(custom_includes):
			new_includes += NL + NL + custom_includes
		new_file_content = before_capture + NL + php_trim(new_includes) + after_capture
		if new_file_content != file_content:
			file_content = new_file_content
			status.append(notice_string('includes changed'))
		return True, file_content

	if is_verbose():
		status.append(warning_string('no includes in file'))
	return True, file_content


def check_clang_tidy():
	cmake_folder = PROJECT_PATH + 'cmake'
	run_quiet([find_executable('cmake'), cmake_folder, '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON'])

	# TODO apply more styles, maybe modernize-*?
	# Following checks are available with clang-tidy-6.0
	checks = [
		'modernize-avoid-bind',
		'modernize-deprecated-headers',
		'modernize-loop-convert',
		'modernize-make-shared',
		'modernize-make-unique',
		'modernize-pass-by-value',
		'modernize-raw-string-literal',
		'modernize-redundant-void-arg',
		'modernize-replace-auto-ptr',
		'modernize-replace-random-shuffle',
		# 'modernize-return-braced-init-list',
		'modernize-shrink-to-fit',
		'modernize-unary-static-assert',
		# 'modernize-use-auto',
		'modernize-use-bool-literals',
		# 'modernize-use-default-member-init',
		'modernize-use-emplace',
		'modernize-use-equals-default',
		'modernize-use-equals-delete',
		'modernize-use-noexcept',
		'modernize-use-nullptr',
		'modernize-use-override',
		'modernize-use-transparent-functors',
		'modernize-use-default-member-init',
		# 'modernize-use-using',
	]

	check_string = '-*,' + ','.join(checks)

	cmd = [find_executable(RUN_CLANG_TIDY_EXEC_PATH),
		'-clang-apply-replacements-binary=' + CLANG_APPLY_BINARY_PATH,
		'-checks=' + check_string,
		'-header-filter=.*']
	# cmd.append('-quiet')
	# cmd += ['-p', cmake_folder]
	if not is_dry_run():
		cmd.append('-fix')

	out = run_quiet(cmd)
	for check in checks:
		cnt = out.count(('[' + check + ']').encode(ENCODING))
		echo(notice_string(str_pad(check, 30)) + ' - ')
		if cnt > 0:
			echo(success_string(str(cnt) + ' occurences'))
		else:
			echo(str(cnt) + ' occurences')
		echo(NL)
	with open(PROJECT_PATH + '/tools/clang-tidy.log', 'wb') as f:
		f.write(out)

	return True


def run_quiet(cmd):
	"""shell_exec() in PHP: returns the standard output, errors go to the console"""
	try:
		return subprocess.run(cmd, stdout=subprocess.PIPE).stdout
	except OSError as e:
		sys.stderr.write(cmd[0] + ': ' + e.strerror + NL)
		return b''


# --- file header ---------------------------------------------------------------------------------

FILE_HEADER_TEMPLATE = r'''/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) __date__ Mandelbulber Team __spacing__ §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .\'\'\'
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: __author__
 *
 * __description__
 */'''.replace("\\'", "'")


def get_file_header(author, description, modification_string):
	lines = description.split(NL)
	non_empty_lines = []
	first_found = False
	for line in lines:
		l = php_trim(line)
		if not first_found and (l == '*' or php_empty(l)):
			continue
		regex_comment_start = r'^\*\s?(.*)$'
		match = re.search(regex_comment_start, l, RE_FLAGS)
		if match:
			l = match.group(1)
		non_empty_lines.append(php_rtrim((' * ' if first_found else '') + l))
		first_found = True
	if non_empty_lines and non_empty_lines[-1] == ' *':
		non_empty_lines.pop()

	description = NL.join(non_empty_lines)
	spacing = ' ' * (10 - php_strlen(modification_string))
	out = FILE_HEADER_TEMPLATE
	# str_replace() with arrays in PHP: one search string after the other
	for search, replace in [('__author__', format_author_line(author)), ('__description__', description),
			('__date__', modification_string), ('__spacing__', spacing)]:
		out = out.replace(search, replace)
	return out


def format_author_line(author_line):
	old_authors = author_line.split(',')
	out = ''
	for old_author in old_authors:
		name = php_trim(old_author)
		if name == '':
			continue
		author = look_up_author(name)
		if php_strlen(out) > 0:
			out += ', '
		last_line_break = out.rfind(NL)
		last_line_break = php_strlen(out[:last_line_break]) if last_line_break != -1 else 0
		if php_strlen(out) - last_line_break + php_strlen(author) > 90:
			out = out[:-1] + NL + ' *  '
		out += author
	return out


def look_up_author(author_name):
	# use consistent author name
	if 'Krzysztof Marczak' in author_name:
		return 'Krzysztof Marczak (buddhi1980@gmail.com)'
	if 'Graeme McLaren' in author_name:
		return 'Graeme McLaren'
	if 'pmneila' in author_name:
		return 'pmneila'
	if 'Stanislaw Adaszewski' in author_name:
		return 'Stanislaw Adaszewski'
	if 'Rayan Hitchman' in author_name:
		return 'Rayan Hitchman'
	if 'Robert Pancoast' in author_name:
		return 'Robert Pancoast (RobertPancoast77@gmail.com)'
	if 'Sebastian Jennen' in author_name:
		return 'Sebastian Jennen (jenzebas@gmail.com)'

	# the PHP tool built an "unknown author" error string here but never printed it
	return author_name


# --- main ----------------------------------------------------------------------------------------

def check_files(file_paths, is_header):
	for i, file_path in enumerate(file_paths):
		file_name = php_basename(file_path)
		if is_header:
			if file_name[:len('ui_')] == 'ui_':
				continue
			if file_name in ('clew.h', 'clew-cl.hpp', 'include_header_wrapper.hpp'):
				continue
		elif file_name == 'clew.cpp':
			continue
		folder_name = php_basename(file_path.replace(file_name, ''))
		status = []
		success = True
		with open(file_path, 'rb') as f:
			content = f.read().replace(b'\r\n', b'\n').decode(ENCODING, ERRORS)

		if success:
			success, content = check_file_header(file_path, content, status)
		if is_header and success:
			success, content = check_defines(content, file_name, folder_name, status)
		if success:
			success, content = check_clang_format(content, status)
		if success:
			success, content = check_include_headers(file_path, content, status, folder_name)

		if success and not is_dry_run() and len(status) > 0:
			write_file(file_path, content)
		print_result_line(folder_name + '/' + file_name, success, status, i / len(file_paths))


def main():
	if shutil.which(CLANG_FORMAT_EXEC_PATH) is None:
		sys.exit(error_string('clang-format not found: ' + CLANG_FORMAT_EXEC_PATH
			+ ' (set CLANG_FORMAT to its path)'))
	print_start()

	source_files = glob_files(FILES_TO_CHECK_SOURCE)
	header_files = glob_files(FILES_TO_CHECK_HEADER)

	print_start_group('RUNNING CHECKS ON SOURCE FILES')
	check_files(source_files, False)
	print_end_group()

	print_start_group('RUNNING CHECKS ON HEADER FILES')
	check_files(header_files, True)
	print_end_group()

	if argument_contains('checkTidy'):
		print_start_group('RUNNING CLANG-TIDY')
		echo(notice_string('This may take some time ...') + NL)
		check_clang_tidy()
		print_end_group()
	print_finish()
	return 0


if __name__ == '__main__':
	sys.exit(main())
