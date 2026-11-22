#!/usr/bin/env python3
################################# WORK IN PROGRESS
# this file takes a mandelbulb3d formula file (m3f) and does the following:
# - detect code part from m3f file
# 	- convert code hex string to binary
# 	- convert binary to intel hex
# 	- decompile intel hex to python code
# 	- TODO: further processing
# - detect options
# - TODO
#
# It is a port of the former portMandelbulb3dFormula.php and produces the same output.
#
# Requires Python 3.6 or newer, bin2hex.py from https://github.com/python-intelhex/intelhex and
# retdec-decompiler.py from https://github.com/avast/retdec. Both are looked up in
# /usr/local/bin; set BIN2HEX_PATH and RETDEC_PATH to the folders that contain them otherwise.
#
# The mandelbulb3d folder is the first argument; the formulas are read from its subfolder
# M3Formulas (retrieve from here: https://github.com/thargor6/mb3d). The results are
# written to mandelbulber2/formula/mb3dTemporaryFormulas, also without "nondry".
#
# usage: mandelbulber2/tools/port_mandelbulb3d_formula.py <mandelbulb3d folder>

import glob
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time

RETDEC_BIN_PATH = os.environ.get('RETDEC_PATH', '/usr/local/bin')  # from here: https://github.com/avast/retdec
BIN2HEX_PATH = os.environ.get('BIN2HEX_PATH', '/usr/local/bin')  # from here https://github.com/python-intelhex/intelhex
TARGET_FOLDER = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'formula',
	'mb3dTemporaryFormulas')
START_TIME = time.time()
NL = '\n'
PHP_TRIM_CHARS = ' \t\n\r\0\x0b'  # the characters PHP's trim() removes

# the PHP script printed its header comment (the text before "<?php"), so this one does too
HEADER = '''################################# WORK IN PROGRESS
# this file takes a mandelbulb3d formula file (m3f) and does the following:
# - detect code part from m3f file
# \t- convert code hex string to binary
# \t- convert binary to intel hex
# \t- decompile intel hex to python code
# \t- TODO: further processing
# - detect options
# - TODO
#

'''


# --- console output --------------------------------------------------------------------------

def echo(s):
	# file names are written as the bytes they have on disk
	sys.stdout.flush()
	sys.stdout.buffer.write(s.encode('utf-8', 'surrogateescape'))
	sys.stdout.buffer.flush()


def warn(s):
	sys.stdout.flush()
	sys.stderr.write(s + NL)
	sys.stderr.flush()


def notice_string(s):
	return '\033[1;34m' + s + '\033[0m'


def is_dry_run():
	return 'nondry' not in sys.argv[1:]


def print_start():
	echo('Processing...' + NL)


def print_finish():
	echo('\033[2K\r')
	echo('script took ' + notice_string('{:,.2f} seconds'.format(time.time() - START_TIME))
		+ ' to complete.' + NL)
	if is_dry_run():
		echo('This is a dry run.' + NL)
		echo('If you want to apply the changes, execute with argument "nondry"' + NL)
	else:
		echo('Changes applied' + NL)


# --- files and external tools ------------------------------------------------------------------
# The m3f files and the decompiler output are handled as bytes (decoded as latin-1, one character
# per byte), like PHP does, so any encoding passes through unchanged.

def read_text(path):
	"""content of a text file with CRLF line ends normalized to LF, '' if it can not be read"""
	return read_raw(path).replace('\r\n', '\n')


def read_raw(path):
	"""content of a file as it is on disk, '' if it can not be read (like PHP's file_get_contents)"""
	try:
		with open(path, 'rb') as f:
			return f.read().decode('latin-1')
	except OSError as e:
		warn('Warning: can not read ' + path + ': ' + e.strerror)
		return ''


def write_file(path, content):
	with open(path, 'wb') as f:
		f.write(content.encode('latin-1'))


def run(cmd, stdout):
	# PHP ran these with shell_exec(), which discards the output and shows the errors
	try:
		subprocess.run(cmd, stdout=stdout)
	except OSError as e:
		warn(cmd[0] + ': ' + e.strerror)


def hex_to_binary(text):
	"""the bytes of a hex string, as "xxd -r -p" converts it"""
	# this follows huntype() of xxd in postscript (plain) mode: whitespace is skipped, garbage
	# before the first hex digit of a line is skipped, a pair of hex digits is one byte and after
	# three non-hex characters in a row the rest of the line is skipped
	out = bytearray()
	ign_garb = True
	n1, n2 = -1, 0
	i, length = 0, len(text)
	while i < length:
		c = text[i]
		i += 1
		if c in '\r \n\t':
			continue
		n3, n2 = n2, n1
		n1 = int(c, 16) if c in '0123456789abcdefABCDEF' else -1
		if n1 == -1 and ign_garb:
			continue
		ign_garb = False
		if n2 >= 0 and n1 >= 0:
			out.append((n2 << 4) | n1)
			n1 = -1
		elif n1 < 0 and n2 < 0 and n3 < 0:
			# already stumbled into garbage, skip line, wait and see
			eol = text.find('\n', i - 1)
			if eol < 0:
				break
			i = eol + 1
			ign_garb = True
	return bytes(out)


# --- porting -----------------------------------------------------------------------------------

def port_formula_from_file(file_name, temp_folder):
	contents = read_text(file_name)
	code = detect_code_py(contents, temp_folder)

	# c_file_path = os.path.join(TARGET_FOLDER, replace_extension(file_name, '.py'))
	# write_file(c_file_path, code)

	options = detect_options(contents)  # not used yet
	match = re.search(r'def\s*function_0.*([\s\S]+?)# -------', code, re.ASCII)
	code_part = match.group(1) if match else ''

	c_code = transpile_python_to_c(code_part)
	header_file_path = os.path.join(TARGET_FOLDER, replace_extension(file_name, '.h'))
	write_file(header_file_path, c_code)

	# constants = detect_constants()  # TODO
	# description = detect_description()  # TODO

	c_code = detect_code_c(contents, temp_folder)
	c_file_path = os.path.join(TARGET_FOLDER, replace_extension(file_name, '.c'))
	write_file(c_file_path, c_code)

	asm_code = read_raw(os.path.join(temp_folder, 'code.ll'))
	asm_file_path = os.path.join(TARGET_FOLDER, replace_extension(file_name, '.asm'))
	write_file(asm_file_path, asm_code)


def replace_extension(file_name, extension):
	# like str_ireplace('.m3f', $extension, basename($fileName)): every match, any case
	return re.sub(r'\.m3f', lambda m: extension, os.path.basename(file_name), flags=re.ASCII | re.IGNORECASE)


def transpile_python_to_c(code):
	# add semicolor at end of non empty lines
	lines = code.split(NL)
	for index, line in enumerate(lines):
		if line.strip(PHP_TRIM_CHARS) != '':
			lines[index] = line + ';'
	code = NL.join(lines)
	code = code.replace('eax', 'z.x')
	code = code.replace('edx', 'z.y')
	code = code.replace('ecx', 'z.z')
	code = code.replace('arg1[24]', 'j.x')
	code = code.replace('arg1[32]', 'j.y')
	code = code.replace('arg1[40]', 'j.z')

	c_code_formula = ('const inline void FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux) override'
		+ NL + '\t{' + NL + '        ' + code + NL + '\t}')
	return c_code_formula


def decompile(contents, cmd_flags, temp_folder):
	"""code part of the m3f file, decompiled by retdec with the given flags"""
	match = re.search(r'\[CODE\]([\s\S]+)\[END\]', contents, re.ASCII)
	code = (match.group(1) if match else '').strip(PHP_TRIM_CHARS)
	# echo('Detected following code:' + NL + code)

	# echo('Convert code to binary...' + NL)
	bin_path = os.path.join(temp_folder, 'code.bin')
	with open(bin_path, 'wb') as f:
		f.write(hex_to_binary(code))

	# echo('Convert binary to intel hex...' + NL)
	ihex_path = os.path.join(temp_folder, 'code.ihex')
	with open(ihex_path, 'wb') as f:
		run([BIN2HEX_PATH + '/bin2hex.py', bin_path], f)
	# echo('Created following intelHex:' + read_raw(ihex_path))

	# echo('Decompile intel hex to python ...' + NL)
	res_path = os.path.join(temp_folder, 'code.res')
	# results of the previous formula must not be taken when the decompiler fails
	for stale in (res_path, os.path.join(temp_folder, 'code.ll')):
		if os.path.exists(stale):
			os.remove(stale)
	cmd_flags = cmd_flags + ['-o', res_path]  # set the output file
	cmd_dec = [RETDEC_BIN_PATH + '/retdec-decompiler.py'] + cmd_flags + [ihex_path]
	run(cmd_dec, subprocess.DEVNULL)
	# echo(' '.join(cmd_dec))
	# echo('Created following code:' + NL)
	code = read_raw(res_path)
	# echo(code)
	return code


def detect_code_py(contents, temp_folder):
	cmd_flags = []
	cmd_flags += ['-a', 'x86']  # target x86-64 processors
	cmd_flags += ['-e', 'little']  # set endianness to little (pascal in windows is source)
	# cmd_flags += ['--backend-no-symbolic-names']  # keep original names (local variables)
	cmd_flags += ['--backend-no-var-renaming']  # keep original names (registers)
	# cmd_flags += ['--backend-no-opts']  # needs to be optimized, keep this commented out
	# cmd_flags += ['--backend-aggressive-opts']  # breaks the code?
	# cmd_flags += ['--backend-keep-all-brackets']  # no, we dont want brackets
	cmd_flags += ['--backend-keep-library-funcs']  # why not?
	# cmd_flags += ['--backend-no-compound-operators']  # no effect?
	# cmd_flags += ['--backend-no-time-varying-info']  # no effect?
	# cmd_flags += ['--backend-strict-fpu-semantics']  # no effect?
	cmd_flags += ['-l', 'py']  # set the target language
	return decompile(contents, cmd_flags, temp_folder)


def detect_code_c(contents, temp_folder):
	cmd_flags = []
	cmd_flags += ['-a', 'x86']  # target x86-64 processors
	cmd_flags += ['-e', 'little']  # set endianness to little (pascal in windows is source)
	# cmd_flags += ['--backend-no-symbolic-names']  # keep original names (local variables)
	# cmd_flags += ['--backend-no-var-renaming']  # keep original names (registers)
	cmd_flags += ['--backend-no-opts']  # needs to be optimized, keep this commented out
	# cmd_flags += ['--backend-aggressive-opts']  # breaks the code?
	# cmd_flags += ['--backend-keep-all-brackets']  # no, we dont want brackets
	cmd_flags += ['--backend-keep-library-funcs']  # why not?
	# cmd_flags += ['--backend-no-compound-operators']  # no effect?
	# cmd_flags += ['--backend-no-time-varying-info']  # no effect?
	# cmd_flags += ['--backend-strict-fpu-semantics']  # no effect?
	cmd_flags += ['-l', 'c']  # set the target language
	return decompile(contents, cmd_flags, temp_folder)


def detect_options(content):
	options = {}
	match = re.search(r'\[OPTIONS\]\n((?:[\.A-Za-z ]+\s*=\s*[\d.]+\n)*)', content, re.ASCII)
	if match:
		options_raw = match.group(1)
		for options_line in options_raw.split(NL):
			match_option = re.search(r'\.([A-Za-z ]+)\s*=\s*([\d\.]+)', options_line, re.ASCII)
			if match_option:
				options[match_option.group(1)] = match_option.group(2)
	return options


def main():
	echo(HEADER)
	print_start()
	mb3d_folder = sys.argv[1] if len(sys.argv) > 1 else ''  # see folder M3Formulas
	if mb3d_folder[-1:] != '/':
		mb3d_folder += '/'
	if not os.path.exists(TARGET_FOLDER):
		os.mkdir(TARGET_FOLDER)
	temp_folder = tempfile.mkdtemp(prefix='mb3d-formula-')
	try:
		for filename in sorted(glob.glob(mb3d_folder + 'M3Formulas/*.m3f')):
			# if 'quick' not in filename.lower(): continue  # test with simple formula: "quickdudley"
			echo('Handling: ' + filename + NL)
			port_formula_from_file(filename, temp_folder)
	finally:
		shutil.rmtree(temp_folder, ignore_errors=True)
	print_finish()
	return 0


if __name__ == '__main__':
	sys.exit(main())
