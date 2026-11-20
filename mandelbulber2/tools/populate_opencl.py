#!/usr/bin/env python3
#
# This file autogenerates the OpenCL struct headers in opencl/ from the C++ sources in src/.
# It is a port of the former populateOpenCL.php and generates the same output.
#
# Requires Python 3.6 or newer and clang-format (set CLANG_FORMAT to its path when it is not
# on PATH).
#
# By default this script runs dry: it generates the files and shows which of them would change.
# This should always be run first, to see if any issues occur. Run it with the argument
# "nondry" to write the changes to the opencl files, and with "verbose" to list every file.
#
# usage: mandelbulber2/tools/populate_opencl.py [nondry] [verbose]

import os
import re
import shutil
import subprocess
import sys
import time

PROJECT_PATH = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
CLANG_FORMAT_EXEC_PATH = os.environ.get('CLANG_FORMAT', 'clang-format')
START_TIME = time.time()
NL = '\n'

COPY_FILES = [
	('fractal.h', 'fractal_cl.h'),
	('fractparams.hpp', 'fractparams_cl.hpp'),
	('image_adjustments.h', 'image_adjustments_cl.h'),
	('common_params.hpp', 'common_params_cl.hpp'),
	('fractal_coloring.hpp', 'fractal_coloring_cl.hpp'),
	('texture_enums.hpp', 'texture_enums_cl.h'),
	('object_types.hpp', 'object_type_cl.h'),
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


def warning_string(s):
	return color('1;33', s)


def is_dry_run():
	return 'nondry' not in sys.argv[1:]


def is_verbose():
	return 'verbose' in sys.argv[1:]


def print_start_group(title):
	print(NL + '\033[1m\033[44m ' + title.ljust(54) + ' \033[0m')


def print_end_group():
	print(' ╹')


def print_result_line(name, success, status):
	if success and not is_verbose() and not status:
		return
	line = (' ├── ' + name).ljust(50)
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


# --- copyright years from git history ----------------------------------------------------------

_modification_intervals = None


def get_modification_intervals():
	"""first and last year in which each file was changed, from git log"""
	info = {}
	try:
		raw = subprocess.run(['git', 'log', '--format=COMMIT_SEPARATOR_1____________%adCOMMIT_SEPARATOR_2____________%s',
			'--name-only'], cwd=PROJECT_PATH, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
			universal_newlines=True, encoding='utf-8', errors='replace').stdout.strip()
	except OSError:
		return info
	for commit in raw.split('COMMIT_SEPARATOR_1____________'):
		if not commit:
			continue
		lines = commit.split(NL)
		meta = lines[0].split('COMMIT_SEPARATOR_2____________')
		year = meta[0][-10:-6]
		title = meta[1] if len(meta) > 1 else ''
		# these commits contain only auto formatting code changes
		# and should not be counted for the modification interval
		if 'source code' in title or 'nullptr' in title or 'remove src dir' in title:
			continue
		for line in lines:
			if line and 'COMMIT_SEPARATOR_2____________' not in line:
				entry = info.setdefault(line, {'start': year, 'end': year})
				entry['start'] = min(entry['start'], year)
				entry['end'] = max(entry['end'], year)
	return info


def get_modification_interval(file_path):
	global _modification_intervals
	if _modification_intervals is None:
		_modification_intervals = get_modification_intervals()
	relative = 'mandelbulber2/' + os.path.relpath(file_path, PROJECT_PATH).replace(os.sep, '/')
	entry = _modification_intervals.get(relative)
	if not entry:
		return time.strftime('%Y')
	if entry['start'] == entry['end']:
		return entry['start']
	return entry['start'] + '-' + entry['end'][2:4]


def patch_modification_date(file_path, content):
	years = get_modification_interval(file_path)
	return re.sub(r'Copyright \(C\) [0-9-]+ Mandelbulber Team \s+ §',
		lambda m: 'Copyright (C) ' + years + ' Mandelbulber Team ' + ' ' * (10 - len(years)) + ' §', content)


# --- generation ----------------------------------------------------------------------------------

def php_replacement(replacement):
	# the patterns below are taken over from PHP: "$1" there is "\g<1>" here
	return re.sub(r'\$(\d)', r'\\g<\1>', replacement)


# replacements of OpenCL specific tokens, applied in this order (all matches)
OPENCL_REPLACE_LOOKUP = [
	(r'(\s)int(\s)', '$1cl_int$2'),
	(r'(\s)bool(\s)', '$1cl_int$2'),
	(r'(\s)double(\s)', '$1cl_float$2'),
	(r'(\s)float(\s)', '$1cl_float$2'),
	(r'(\s)sRGB(\s)', '$1cl_float3$2'),
	(r'(\s)sRGBFloat(\s)', '$1cl_float3$2'),
	(r'(\s)CVector3(\s)', '$1cl_float3$2'),
	(r'(\s)CVector3', '$1cl_float3'),
	(r'(\s)CVector4(\s)', '$1cl_float4$2'),
	(r'(\s)[a-zA-Z0-9_]+::[a-zA-Z0-9_]+(\s)', '$1cl_int$2'),  # enums with outer scope (::) get cast to int
	(r'cColorGradient[\s\S]*?;', ''),  # remove cColorGradient parameters
	# ALIGN16 keeps float4 members 16-byte aligned on host and device
	(r'struct\s([a-zA-Z0-9_]+)\n(\s*)({[\S\s]+?\n\2})', 'typedef struct ALIGN16\n$2$3 $1'),
	(r'enum\s([a-zA-Z0-9_]+)\n(\s*)({[\S\s]+?\n\2})', 'typedef enum\n$2$3 $1'),
	(r'const cl_int\s([a-zA-Z0-9_]+)\s=\s([a-zA-Z0-9_]+);', '#define $1 $2'),
	(r'\n#include\s.*', ''),  # remove includes
	(r'(\s)CRotationMatrix(\s)', '$1matrix33$2'),
	(r'(\s)CRotationMatrix44(\s)', '$1matrix44$2'),
	(r'class\s([a-zA-Z0-9_]+);', ''),  # remove forward declaration
	(r'struct\s([a-zA-Z0-9_]+);', ''),  # remove forward declaration
	(r'//\sforward declarations', ''),  # remove comment "forward declaration"
	(r'MANDELBULBER2_SRC_(.*)_HPP_', 'MANDELBULBER2_OPENCL_$1_CL_HPP_'),  # include guard 1
	(r'MANDELBULBER2_SRC_(.*)_H_', 'MANDELBULBER2_OPENCL_$1_CL_H_'),  # include guard
	# TODO rework these regexes
	(r'using namespace[\s\S]*?\n}\n', ''),  # no namespace support -> TODO fix files with namespaces
	(r'\nnamespace[\s\S]*?{([\s\S]*?\n)};?(?:\s*//.*)?\n', '\n$1'),  # no namespace support
	(r'sParamRenderCl\([\s\S]*?\);', ''),  # remove constructor
	(r'sFractalCl\([\s\S]*?\);', ''),  # remove constructor
	(r'sImageAdjustmentsCl\([\s\S]*?}', ''),  # remove constructor
	(r'void RecalculateFractalParams\([\s\S]*?\);', ''),  # remove method
	(r'cl_float CalculateColorIndex\([\s\S]*?\);', ''),  # remove method
	(r'[\s]*// Apply common fractal params[\s\S]*?void ApplyNodeData\([\s\S]*?\);', ''),  # remove method
	(r'.*::.*', ''),  # no namespace scopes allowed?
	(r'.*cPrimitives.*', ''),  # need to include file...
	(r'sCommonParams ', 'sCommonParamsCl '),  # TODO autogen replace over all files
	(r'sImageAdjustments ', 'sImageAdjustmentsCl '),  # TODO autogen replace over all files
	(r'matrix44 ', '// matrix44 '),  # TODO
	(r' &([a-zA-Z0-9_]+)', ' *$1'),  # no passing by reference
	(r'const sExtendedAux \*extendedAux', 'const sExtendedAuxCl *extendedAux'),  # rewrite
	(r'const sFractal \*defaultFractal', 'const sFractalCl *defaultFractal'),  # rewrite
	(r'const sFractalColoring \*fractalColoring', 'const sFractalColoringCl *fractalColoring'),  # rewrite
	(r'extendedAux\.', 'extendedAux->'),  # pointer dereference
	(r'fractalColoring\.', 'fractalColoring->'),  # pointer dereference
	(r'([a-zA-Z0-9_]+\(\));', ''),  # remove constructor declaration
	(r'([a-zA-Z0-9_\[\]\s\-\+]+)(?:{.*})?(;.*)', '$1$2'),  # remove braced initializer
]


def lcfirst(s):
	return s[:1].lower() + s[1:]


def ucfirst(s):
	return s[:1].upper() + s[1:]


def get_copy_struct(struct_name, properties):
	"""C++ function which copies a C++ struct into its OpenCL counterpart"""
	out = ('inline ' + struct_name + ' clCopy' + ucfirst(struct_name) + '(const ' + struct_name[:-2]
		+ '& source){' + NL)
	out += '\t' + struct_name + ' target;' + NL
	for prop in properties:
		line = ''
		# if the var is an array, prepend for loops for each dimension
		for index, size in enumerate(prop.get('array', [])):
			x = chr(105 + index)  # i, j, k, ...
			line += 'for(int ' + x + ' = 0; ' + x + ' < ' + size + '; ' + x + '++){'
		line += 'target.' + prop['name'] + ' = '
		prop_type = prop['type']
		if prop_type == 'struct':
			# if the var is a struct, copy with the assumed clCopy* function
			line += 'clCopy' + ucfirst(prop['typeName']) + '(source.' + prop['name'] + ');'
		elif prop_type == 'enum':
			# the value is an enum, so it has to be casted to target type
			line += prop['typeName'] + '(source.' + prop['name'] + ');'
		elif prop_type == 'cl_float3':
			# the target type is a special opencl type, which has a copy method from opencl_algebra.h
			line += 'toClFloat3(source.' + prop['name'] + ');'
		elif prop_type == 'matrix33':
			line += 'toClMatrix33(source.' + prop['name'] + ');'
		elif prop_type == 'cl_int3':
			line += 'toClInt3(source.' + prop['name'] + ');'
		elif prop_type == 'cl_float4':
			line += 'toClFloat4(source.' + prop['name'] + ');'
		else:
			# the value can simply be assigned
			line += 'source.' + prop['name'] + ';'
		# close open for loops
		line += '}' * len(prop.get('array', []))
		out += '\t' + line + NL
	out += '\treturn target;' + NL
	out += '}' + NL
	return out


def autogen_opencl_file(src_name, target_name, status):
	path = os.path.join(PROJECT_PATH, 'src', src_name)
	path_target = os.path.join(PROJECT_PATH, 'opencl', target_name)
	with open(path_target, encoding='utf-8', newline='') as f:
		old_content = f.read().replace('\r\n', '\n')
	with open(path, encoding='utf-8', newline='') as f:
		content = f.read().replace('\r\n', '\n')

	# add the "autogen" - line to the file header
	match_header = re.match(r'^(/\*\*?[\s\S]*?\*/)([\s\S]*)$', content)
	if not match_header:
		status.append(error_string('header unknown!'))
		return False
	file_header, file_source_code = match_header.group(1), match_header.group(2)
	no_change_comment = [
		'      ____                   ________ ',
		'     / __ \\____  ___  ____  / ____/ / ',
		'    / / / / __ \\/ _ \\/ __ \\/ /   / /  ',
		'   / /_/ / /_/ /  __/ / / / /___/ /___',
		'   \\____/ .___/\\___/_/ /_/\\____/_____/',
		'       /_/                            ',
		'',
		'This file has been autogenerated by tools/populate_opencl.py',
		'from the file src/' + src_name,
		'D O    N O T    E D I T    T H I S    F I L E !',
	]
	file_header = file_header.replace('*/', '* ' + (NL + ' * ').join(no_change_comment) + NL + ' */')
	content = file_header + file_source_code

	# append "Cl" to the names of structs, enums and constants (and replace all matches)
	for find in [r'struct\s([a-zA-Z0-9_]+)\n', r'enum\s([a-zA-Z0-9_]+)\n', r'const int\s([a-zA-Z0-9_]+)\s=\s']:
		for replace in re.findall(find, content):
			content = re.sub('(' + replace + r')([ \]\(;\n])', r'\g<1>Cl\g<2>', content)
			strip_enum = lcfirst(replace.replace('enum', ''))
			content = re.sub('(' + strip_enum + ')_([a-zA-Z0-9_]+)', r'\g<1>Cl_\g<2>', content)

	# replace opencl specific tokens
	for find, replace in OPENCL_REPLACE_LOOKUP:
		content = re.sub(find, php_replacement(replace), content)

	# add c++ side includes
	cpp_includes = '#ifndef OPENCL_KERNEL_CODE' + NL
	# fractal_cl.h is the base of the include chain, so it must not include common_params_cl.hpp
	# (circular dependency)
	if target_name not in ('common_params_cl.hpp', 'fractal_cl.h'):
		cpp_includes += '#include "common_params_cl.hpp"' + NL
	if target_name != 'fractal_cl.h':
		cpp_includes += '#include "fractal_cl.h"' + NL
	if target_name != 'image_adjustments_cl.h':
		cpp_includes += '#include "image_adjustments_cl.h"' + NL
	if target_name != 'opencl_algebra.h':
		cpp_includes += '#include "opencl_algebra.h"' + NL
	cpp_includes += NL
	# the wrapper defines the ALIGN16 macro on the C++ side; needed only in the base header
	if target_name == 'fractal_cl.h':
		cpp_includes += '#include "src/include_header_wrapper.hpp"' + NL
	for header in ['common_params.hpp', 'fractal.h', 'fractal_enums.h', 'fractparams.hpp',
			'image_adjustments.h']:
		cpp_includes += '#include "src/' + header + '"' + NL
	cpp_includes += '#endif /* OPENCL_KERNEL_CODE */' + NL
	content = re.sub(r'(#define MANDELBULBER2_OPENCL_.*)', lambda m: m.group(1) + NL + NL + cpp_includes,
		content)

	# insert enumBooleanOperatorCl from boolean_operator.h into fractparams_cl.hpp
	if target_name == 'fractparams_cl.hpp':
		bool_op_src = os.path.join(PROJECT_PATH, 'src', 'boolean_operator.h')
		if os.path.exists(bool_op_src):
			with open(bool_op_src, encoding='utf-8', newline='') as f:
				bool_op_content = f.read().replace('\r\n', '\n')
			m = re.search(r'enum\s+(enumBooleanOperator)\s*\{([\s\S]*?)\};?', bool_op_content)
			if m:
				enum_def = (NL + 'typedef enum' + NL + '{' + NL + m.group(2).strip() + NL + '} '
					+ m.group(1) + 'Cl;' + NL + NL)
				content = re.sub(r'(} enumTextureMapTypeCl;)', lambda mm: mm.group(1) + enum_def, content)

	# create copy methods for structs
	copy_structs = []
	for body, struct_name in re.findall(r'typedef struct(?: ALIGN16)?\n{([\s\S]*?\n)}\s([0-9a-zA-Z_]+);',
			content):
		props = []
		for line in body.split(NL):
			line_match = re.match(r'^\s*([a-zA-Z0-9_]+)\s([a-zA-Z0-9_\[\]\s\-\+]+);.*', line.strip())
			if not line_match:
				continue
			prop = {'name': line_match.group(2), 'typeName': line_match.group(1), 'type': line_match.group(1)}
			if prop['type'][:1] == 's':
				prop['type'] = 'struct'
			if prop['type'][:4] == 'enum':
				prop['type'] = 'enum'
			for index, e in enumerate(line_match.group(2).split('[')[1:]):
				size = e[:e.index(']')]
				prop.setdefault('array', []).append(size)
				# the array will be inited in a loop, so the array index should be the run var in
				# each dimension: i, j, k, ...
				prop['name'] = prop['name'].replace('[' + size + ']', '[' + chr(105 + index) + ']')
			props.append(prop)
		copy_structs.append(get_copy_struct(struct_name.strip(), props))
	if copy_structs:
		content = re.sub(r'(#endif /\* MANDELBULBER2_OPENCL.*)',
			lambda m: (NL + '#ifndef OPENCL_KERNEL_CODE' + NL + NL.join(copy_structs)
				+ '#endif /* OPENCL_KERNEL_CODE */' + NL + NL + m.group(1)), content)

	# clang-format, with the .clang-format of the project (found from the folder of the temp file)
	file_path_temp = os.path.join(PROJECT_PATH, 'tools', '.tmp.c')
	with open(file_path_temp, 'w', encoding='utf-8', newline='\n') as f:
		f.write(content)
	try:
		subprocess.run([CLANG_FORMAT_EXEC_PATH, '-i', '--style=file', file_path_temp], check=True)
		with open(file_path_temp, encoding='utf-8', newline='') as f:
			content = f.read().replace('\r\n', '\n')
	finally:
		os.remove(file_path_temp)  # nothing to see here :)
	content = patch_modification_date(path_target, content)

	if content != old_content:
		if not is_dry_run():
			with open(path_target, 'w', encoding='utf-8', newline='\n') as f:
				f.write(content)
		status.append(notice_string('file changed.'))
	return True


def main():
	if hasattr(sys.stdout, 'reconfigure'):
		sys.stdout.reconfigure(encoding='utf-8')  # the tree characters, also on Windows consoles
	if shutil.which(CLANG_FORMAT_EXEC_PATH) is None:
		sys.exit(error_string('clang-format not found: ' + CLANG_FORMAT_EXEC_PATH
			+ ' (set CLANG_FORMAT to its path)'))
	print('Processing...')

	print_start_group('RUNNING OPENCL AUTOGENERATION')
	success_all = True
	for src_name, target_name in COPY_FILES:
		status = []
		success = autogen_opencl_file(src_name, target_name, status)
		success_all = success_all and success
		print_result_line(target_name, success, status)
	print_end_group()

	print_start_group('RUNNING OPENCL DEFINE COLLECT')
	print_result_line('defines_cl.h', True, [warning_string('TODO')])
	print_end_group()

	print_finish()
	return 0 if success_all else 1


if __name__ == '__main__':
	sys.exit(main())
