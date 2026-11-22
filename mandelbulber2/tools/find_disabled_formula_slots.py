#!/usr/bin/env python3
#
# This file lists settings files (*.fract) which keep a formula and/or formula parameters in a
# formula slot that does not take part in the render (see issue #957). It only reads files and
# never changes them; the cleanup of the reported files is left to the maintainers, because the
# examples are also used by the regression renders and the settings migration tests.
#
# A slot counts as not rendered when (same rules as src/objects_tree_migrator.cpp and
# src/objects_tree.cpp use for the file's version):
# - files older than 2.35 (flat format, migrated to the objects tree when loaded):
#   - "disabled": the slot has a formula and fractal_enable_N (or fractal_enable in
#     [fractal_N]) is false; the slot is used in hybrid or boolean mode, or it is slot 1
#   - "ignored": hybrid_fractal_enable and boolean_operators are both off, so only slot 1 is
#     rendered, but slot N > 1 still has a formula
# - files 2.35 and newer (objects tree format):
#   - "disabled": every fractal node (node_XXXX_definition, type 1) which uses the slot is
#     disabled itself (node_XXXX_enabled false) or through a disabled parent node
#   - "ignored": the slot has a formula but no fractal node uses it
# - with --empty: "empty" slots, which have no formula (formula none) and are not used, but still
#   carry parameters in [fractal_N] (or legacy <name>_N parameters in [main_parameters])
#
# Requires Python 3.6 or newer.
#
# By default the examples folder (deploy/share/mandelbulber2/examples) is scanned recursively.
# Other files or folders can be given instead. "--csv <file>" also writes the findings as CSV
# ("-" writes the CSV to the standard output instead of the readable list).
#
# usage: mandelbulber2/tools/find_disabled_formula_slots.py [--empty] [--csv <file>] [path ...]

import argparse
import csv
import os
import re
import sys

PROJECT_PATH = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
EXAMPLES_PATH = os.path.join(PROJECT_PATH, 'deploy', 'share', 'mandelbulber2', 'examples')
FORMULA_ENUM_FILE = os.path.join(
	PROJECT_PATH, 'formula', 'definition', 'all_fractal_list_enums.hpp')

OBJECTS_TREE_VERSION = 2.35
MAX_LEGACY_FRACTALS = 9
FORMULA_NONE = 0
FORMULA_DEFAULT_SLOT_1 = 2  # fractal::mandelbulb, default of [fractal_1] formula
NODE_TYPE_FRACTAL = 1
PRIM_BOOLEAN_OPERATOR_OR = 1

# base names of the legacy flat per-fractal parameters "<name>_N" in [main_parameters]
# (gLegacyPerFractalParams in src/objects_tree_migrator.cpp)
LEGACY_PER_FRACTAL_PARAMS = {
	'boolean_operator', 'check_for_bailout', 'dont_add_c_constant', 'fractal_constant_factor',
	'fractal_enable', 'formula', 'formula_iterations', 'formula_material_id', 'formula_maxiter',
	'formula_position', 'formula_repeat', 'formula_rotation', 'formula_scale',
	'formula_start_iteration', 'formula_stop_iteration', 'formula_weight', 'initial_waxis',
	'julia_c', 'julia_mode', 'smooth_de_combine_distance', 'smooth_de_combine_enable',
}

# per-fractal parameters which belong to the slot bookkeeping rather than to the formula
NON_FORMULA_PARAMS = {'formula', 'fractal_enable', 'boolean_operator'}

SECTION_RE = re.compile(r'^\[(.+)\]$')
FRACTAL_SECTION_RE = re.compile(r'^fractal_(\d+)$')
VERSION_RE = re.compile(r'^#\s*version\s+([0-9]+(?:[.,][0-9]+)?)')
NODE_DEFINITION_RE = re.compile(r'^node_(\d+)_definition$')
NODE_ENABLED_RE = re.compile(r'^node_(\d+)_enabled$')
PRIMITIVE_BOOLEAN_RE = re.compile(r'^primitive_\w+_boolean_operator$')


def load_formula_names():
	names = {}
	try:
		with open(FORMULA_ENUM_FILE, encoding='utf-8') as f:
			for match in re.finditer(r'^\s*(\w+)\s*=\s*(-?\d+)\s*,', f.read(), re.MULTILINE):
				names[int(match.group(2))] = match.group(1)
	except OSError:
		pass
	return names


FORMULA_NAMES = load_formula_names()


def formula_label(formula_id):
	name = FORMULA_NAMES.get(formula_id)
	return '{} ({})'.format(name, formula_id) if name else str(formula_id)


def parse_bool(value):
	return value.strip().lower() in ('true', '1')


def parse_int(value):
	return int(value.strip().split()[0])


class SlotData:
	def __init__(self):
		self.formula = None  # None means not set in the file
		self.enable = None  # None means not set in the file (default true)
		self.enable_source = ''
		self.params = set()  # formula parameters set for this slot (names without _N)

	def formula_id(self, slot):
		if self.formula is not None:
			return self.formula
		return FORMULA_DEFAULT_SLOT_1 if slot == 1 else FORMULA_NONE


class FractFile:
	def __init__(self, path):
		self.path = path
		self.version = None
		self.version_text = ""
		self.main = {}
		self.slots = {}

	def slot(self, index):
		return self.slots.setdefault(index, SlotData())


def read_text(path):
	with open(path, 'rb') as f:
		data = f.read()
	try:
		return data.decode('utf-8')
	except UnicodeDecodeError:
		return data.decode('latin-1')


def split_line(line):
	# settings lines look like "name value;" (value may contain spaces)
	line = line.strip()
	if line.endswith(';'):
		line = line[:-1]
	parts = line.split(None, 1)
	if not parts:
		return None, None
	return parts[0].strip('"\''), (parts[1] if len(parts) > 1 else '')


def parse_file(path):
	fract = FractFile(path)
	text = read_text(path)
	if 'Mandelbulber settings file' not in text.split('\n', 1)[0]:
		raise ValueError('not a Mandelbulber settings file')
	section = None
	for raw_line in text.splitlines():
		line = raw_line.strip()
		if not line:
			continue
		if line.startswith('#'):
			match = VERSION_RE.match(line)
			if match and fract.version is None:
				fract.version_text = match.group(1)
				fract.version = float(match.group(1).replace(',', '.'))
			continue
		match = SECTION_RE.match(line)
		if match:
			section = match.group(1)
			continue
		name, value = split_line(line)
		if name is None:
			continue
		if section == 'main_parameters':
			fract.main[name] = value
			base, _, index = name.rpartition('_')
			if base in LEGACY_PER_FRACTAL_PARAMS and index.isdigit() \
					and 1 <= int(index) <= MAX_LEGACY_FRACTALS:
				assign_slot_param(fract.slot(int(index)), base, value, name)
		elif section is not None:
			match = FRACTAL_SECTION_RE.match(section)
			if match:
				assign_slot_param(fract.slot(int(match.group(1))), name, value,
					'[{}] {}'.format(section, name))
		# [description], [keyframes], [frames] and other sections are not relevant
	if fract.version is None:
		raise ValueError('no "# version" line')
	return fract


def assign_slot_param(slot, name, value, source):
	if name == 'formula':
		slot.formula = parse_int(value)
	elif name == 'fractal_enable':
		slot.enable = parse_bool(value)
		slot.enable_source = source
	if name not in NON_FORMULA_PARAMS:
		slot.params.add(name)


def main_bool(fract, name):
	return name in fract.main and parse_bool(fract.main[name])


def analyse_legacy(fract):
	# mirrors cObjectsTreeMigrator::MigrateToObjectsTree() for files older than 2.35
	hybrid = main_bool(fract, 'hybrid_fractal_enable')
	boolean = main_bool(fract, 'boolean_operators')
	if not boolean and fract.version < 2.29:
		for name, value in fract.main.items():
			if PRIMITIVE_BOOLEAN_RE.match(name):
				try:
					if parse_int(value) != PRIM_BOOLEAN_OPERATOR_OR:
						boolean = True
						break
				except ValueError:
					pass
	if boolean:
		mode = 'boolean+hybrid' if hybrid else 'boolean'
	else:
		mode = 'hybrid' if hybrid else 'single'

	findings = []
	for index in sorted(fract.slots):
		slot = fract.slots[index]
		formula = slot.formula_id(index)
		if formula != FORMULA_NONE:
			if mode == 'single' and index > 1:
				findings.append((index, 'ignored', formula,
					'formula set, but only slot 1 is used without hybrid or boolean mode', slot))
			elif slot.enable is False:
				findings.append((index, 'disabled', formula,
					'{} is false'.format(slot.enable_source), slot))
		elif slot.params:
			findings.append((index, 'empty', formula, 'no formula, but parameters are set', slot))
	return mode, findings


def analyse_objects_tree(fract):
	# mirrors cObjectsTree::CreateNodeDataFromParameters() and the effective enabled state
	nodes = {}
	for name, value in fract.main.items():
		match = NODE_DEFINITION_RE.match(name)
		if not match:
			continue
		parts = value.split(',')
		if len(parts) < 5:
			continue
		try:
			node_id = int(parts[1])
			nodes[node_id] = {
				'type': int(parts[2]),
				'parent': int(parts[3]),
				'object': int(parts[4]),
				'key': match.group(1),
			}
		except ValueError:
			continue
	if not nodes:
		# cSettings::Decode() creates the default node_0001 ("mandelbulb 1,1,1,0,1": a fractal
		# node using slot 1) when the file defines no node
		nodes[1] = {'type': NODE_TYPE_FRACTAL, 'parent': 0, 'object': 1, 'key': '0001'}
	for node in nodes.values():
		enabled_name = 'node_{}_enabled'.format(node['key'])
		node['enabled'] = parse_bool(fract.main[enabled_name]) if enabled_name in fract.main \
			else True

	def effective_enabled(node_id, depth=0):
		node = nodes.get(node_id)
		if node is None or depth > len(nodes):
			return node is not None
		if not node['enabled']:
			return False
		return node['parent'] == 0 or effective_enabled(node['parent'], depth + 1)

	used = {}  # slot -> list of effective enabled states of the fractal nodes using it
	for node_id, node in nodes.items():
		if node['type'] == NODE_TYPE_FRACTAL:
			used.setdefault(node['object'], []).append(effective_enabled(node_id))

	findings = []
	for index in sorted(set(fract.slots) | set(used)):
		slot = fract.slots.get(index, SlotData())
		formula = slot.formula_id(index)
		states = used.get(index)
		if states:
			if not any(states):
				findings.append((index, 'disabled', formula,
					'all fractal nodes using this slot are disabled', slot))
		elif formula != FORMULA_NONE:
			findings.append((index, 'ignored', formula, 'formula set, but no node uses this slot',
				slot))
		elif slot.params:
			findings.append((index, 'empty', formula, 'no formula, but parameters are set', slot))
	return 'objects tree', findings


def analyse(fract):
	if fract.version < OBJECTS_TREE_VERSION:
		return analyse_legacy(fract)
	return analyse_objects_tree(fract)


def collect_files(paths):
	files = []
	for path in paths:
		if os.path.isdir(path):
			for root, _, names in os.walk(path):
				files.extend(os.path.join(root, n) for n in names if n.lower().endswith('.fract'))
		else:
			files.append(path)
	return sorted(files)


def main():
	parser = argparse.ArgumentParser(
		description='List .fract files which keep formulas or formula parameters in formula '
		'slots that are not rendered (disabled or unused). Read-only.')
	parser.add_argument('paths', nargs='*', default=[EXAMPLES_PATH],
		help='files or folders to scan (default: the examples folder)')
	parser.add_argument('--empty', action='store_true',
		help='also report unused slots without formula which still carry parameters')
	parser.add_argument('--csv', metavar='FILE',
		help='also write the findings as CSV to FILE ("-" for the standard output)')
	args = parser.parse_args()

	files = collect_files(args.paths)
	rows = []
	errors = []
	flagged_files = 0
	counts = {'disabled': 0, 'ignored': 0, 'empty': 0}
	to_stdout = args.csv != '-'

	for path in files:
		try:
			fract = parse_file(path)
			mode, findings = analyse(fract)
		except (OSError, ValueError) as e:
			errors.append((path, str(e)))
			continue
		if not args.empty:
			findings = [f for f in findings if f[1] != 'empty']
		if not findings:
			continue
		flagged_files += 1
		display_path = os.path.relpath(path, args.paths[0]) \
			if os.path.isdir(args.paths[0]) and len(args.paths) == 1 else path
		if to_stdout:
			print('{} (version {}, {})'.format(display_path, fract.version_text, mode))
		for index, kind, formula, reason, slot in findings:
			counts[kind] += 1
			if to_stdout:
				print('  slot {}: {:8s} {} - {}; {} formula parameter(s)'.format(index, kind,
					formula_label(formula) if formula != FORMULA_NONE else 'no formula', reason,
					len(slot.params)))
			rows.append([display_path, fract.version_text, mode, index, kind, formula,
				FORMULA_NAMES.get(formula, ''), len(slot.params), reason])

	if args.csv:
		header = ['file', 'version', 'mode', 'slot', 'kind', 'formula_id', 'formula_name',
			'formula_parameters', 'reason']
		if args.csv == '-':
			writer = csv.writer(sys.stdout, lineterminator='\n')
			writer.writerow(header)
			writer.writerows(rows)
		else:
			with open(args.csv, 'w', newline='', encoding='utf-8') as f:
				writer = csv.writer(f)
				writer.writerow(header)
				writer.writerows(rows)

	out = sys.stdout if to_stdout else sys.stderr
	print('', file=out)
	print('scanned {} file(s), {} with findings: {} disabled, {} ignored{} slot(s)'.format(
		len(files), flagged_files, counts['disabled'], counts['ignored'],
		', {} empty'.format(counts['empty']) if args.empty else ''), file=out)
	for path, message in errors:
		print('could not parse {}: {}'.format(path, message), file=out)
	return 1 if errors else 0


if __name__ == '__main__':
	sys.exit(main())
