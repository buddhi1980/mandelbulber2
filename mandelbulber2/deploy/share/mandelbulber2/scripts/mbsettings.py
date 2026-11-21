r"""
Mandelbulber v2, a 3D fractal generator  _%}}i*<.       __           _       _
Copyright (C) 2017 Mandelbulber Team   _>]|=||i=i<,    / _\ ___ _ __(_)_ __ | |_ ___
                                       \><||i|=>>%)    \ \ / __| '__| | '_ \| __/ __|
This file is part of Mandelbulber.     )<=i=]=|=i<>    _\ \ (__| |  | | |_) | |_\__ \
The project is licensed under GPLv3,   -<>>=|><|||`    \__/\___|_|  |_| .__/ \__|___/
see also COPYING file in this folder.    ~+{i%+++                     |_|

This file contains the MbSettings class which enables
scripting modifications for Mandelbulber settings files.

It is a Python port of the former mbSettings.php and keeps its API and its
output: a settings file loaded and saved with this class is written byte for
byte as the PHP class wrote it on Linux (lines end with LF).
Python 3.6 or newer, standard library only.
"""

import os
import random
import re

__all__ = ['MbSettings', 'Vector']

# PHP's trim() strips exactly these characters
_TRIM_CHARS = ' \t\n\r\0\x0b'
# leading number as read by PHP's floatval()
_FLOAT_PREFIX = re.compile(r'[ \t\n\r\x0b\x0c]*([+-]?([0-9]+(\.[0-9]*)?|\.[0-9]+)([eE][+-]?[0-9]+)?)')
# placeholder for an unset entry of a sequence column
_MISSING = object()


def _is_empty(value):
	"""True for the values PHP's empty() treats as empty ('' and '0' for strings)."""
	return not value or value == '0'


def _php_str(value):
	"""Converts a value to text the way PHP's string conversion does."""
	if value is None or value is False:
		return ''
	if value is True:
		return '1'
	if isinstance(value, float):
		return _php_float_str(value)
	return str(value)


def _php_float_str(value):
	"""Formats a float like PHP does with the default precision of 14 digits."""
	if value != value:
		return 'NAN'
	if value in (float('inf'), float('-inf')):
		return 'INF' if value > 0 else '-INF'
	if value == 0.0:
		return '-0' if str(value).startswith('-') else '0'
	sign = '-' if value < 0 else ''
	mantissa, exponent = ('%.13e' % abs(value)).split('e')
	digits = mantissa.replace('.', '').rstrip('0')
	exponent = int(exponent)
	if exponent < -4 or exponent >= 14:
		return '%s%s.%sE%s%d' % (sign, digits[0], digits[1:] or '0', '-' if exponent < 0 else '+',
			abs(exponent))
	if exponent < 0:
		return sign + '0.' + '0' * (-exponent - 1) + digits
	if len(digits) <= exponent + 1:
		return sign + digits + '0' * (exponent + 1 - len(digits))
	return sign + digits[:exponent + 1] + '.' + digits[exponent + 1:]


def _php_floatval(text):
	"""Reads the leading number of a text like PHP's floatval(), 0.0 if there is none."""
	match = _FLOAT_PREFIX.match(_php_str(text))
	return float(match.group(1)) if match else 0.0


class MbSettings(object):
	SETTINGS_FROM_FILE = 1001
	SETTINGS_FROM_STRING = 1002

	SETTINGS_TO_FILE = 2001
	SETTINGS_TO_STRING = 2002

	# treat parameter as raw string
	PARAMETER_ALL = 3001
	# treat parameter as a vector, transition to and from string will be done on saving and reading
	PARAMETER_XYZ = 3002

	CATEGORY_FRAMES = 'frames'
	CATEGORY_KEYFRAMES = 'keyframes'
	CATEGORY_MAIN_PARAMETERS = 'main_parameters'
	CATEGORY_FRACTAL_1 = 'fractal_1'
	CATEGORY_FRACTAL_2 = 'fractal_2'
	CATEGORY_FRACTAL_3 = 'fractal_3'
	CATEGORY_FRACTAL_4 = 'fractal_4'
	CATEGORY_FRACTAL_5 = 'fractal_5'
	CATEGORY_FRACTAL_6 = 'fractal_6'
	CATEGORY_FRACTAL_7 = 'fractal_7'
	CATEGORY_FRACTAL_8 = 'fractal_8'
	CATEGORY_FRACTAL_9 = 'fractal_9'

	def __init__(self, src, srcType=SETTINGS_FROM_FILE, logLevel='MEDIUM'):
		"""
		MandelbulberSettings constructor.
		:param str src: path of the settings file, or the settings text
		:param int srcType: SETTINGS_FROM_FILE or SETTINGS_FROM_STRING
		:param str logLevel: QUIET|MEDIUM|VERBOSE
		:raises Exception:
		"""
		self.settingsString = None
		# category -> {name: value}
		self.settingsKeyValue = {}
		# category -> {'keys': [column names], 'vals': {column name: [value per frame]}}
		self.settingsSequence = {}
		self.version = None
		self.logLevel = logLevel
		if srcType == self.SETTINGS_FROM_STRING:
			self.settingsString = src
		else:
			if not os.path.exists(src):
				raise FileNotFoundError('Cannot find settings file: ' + src)
			try:
				with open(src, 'rb') as file:
					self.settingsString = file.read().decode('utf-8', 'surrogateescape')
			except OSError:
				self.settingsString = ''
			if _is_empty(self.settingsString):
				raise IOError('File empty or access denied')
		self._parseSettings()

	def SaveSettings(self, dest='', destType=SETTINGS_TO_FILE):
		"""
		:param str dest: path of the file to write
		:param int destType: SETTINGS_TO_FILE or SETTINGS_TO_STRING
		:return: the settings text for SETTINGS_TO_STRING, otherwise ''
		:rtype: str
		:raises Exception:
		"""
		self._storeSettings()
		if destType == self.SETTINGS_TO_STRING:
			return self.settingsString

		try:
			with open(dest, 'wb') as file:
				written = file.write(self.settingsString.encode('utf-8', 'surrogateescape'))
		except OSError:
			written = 0
		if not written:
			raise IOError('Cannot put to file: ' + dest)
		return ''

	def Get(self, category, name, seq=0, parameterComponent=PARAMETER_ALL):
		"""
		gets a given parameter from the settings
		:param str category: name of the container (for example "main_parameters")
		:param str name: name of the parameter (for example: "basic_fog_visibility")
		:param int seq: index of the sequence (needed for parameters in a sequence block [keyframes or flight])
		:param int parameterComponent: specifies the part of the parameter to take, see PARAMETER_* constants
		:return: the value text, or a Vector for PARAMETER_XYZ; None if the parameter is not set
		:rtype: str|Vector
		:raises Exception:
		"""
		parameterValue = None
		if category in (self.CATEGORY_FRAMES, self.CATEGORY_KEYFRAMES):
			sequenceSettings = self.settingsSequence.setdefault(category, {})
			if parameterComponent == self.PARAMETER_XYZ:
				parameterValue = _php_str(self._sequenceValue(category, name + '_x', seq)) \
					+ ' ' + _php_str(self._sequenceValue(category, name + '_y', seq)) \
					+ ' ' + _php_str(self._sequenceValue(category, name + '_z', seq))
			else:
				parameterValue = self._sequenceValue(category, name, seq)
		else:
			parameterValue = self.settingsKeyValue.get(category, {}).get(name)

		if parameterComponent == self.PARAMETER_ALL:
			return parameterValue
		elif parameterComponent == self.PARAMETER_XYZ:
			vals = _php_str(parameterValue).split(' ') + ['', '']
			return Vector(
				_php_floatval(vals[0].replace(',', '.')),
				_php_floatval(vals[1].replace(',', '.')),
				_php_floatval(vals[2].replace(',', '.')))
		raise ValueError('unknown $parameterComponent: ' + _php_str(parameterComponent))

	def Set(self, newValue, category, name, seq=0, parameterComponent=PARAMETER_ALL):
		"""
		sets a given value as the parameter for the settings
		:param newValue: value of the parameter (for example "0.123"), a Vector for PARAMETER_XYZ
		:param str category: name of the container (for example "main_parameters")
		:param str name: name of the parameter (for example: "basic_fog_visibility")
		:param int seq: index of the sequence (needed for parameters in a sequence block [keyframes or flight])
		:param int parameterComponent: specifies how to set the parameter from the value, see PARAMETER_* constants
		"""
		parameterValue = ''
		if parameterComponent == self.PARAMETER_ALL:
			parameterValue = newValue
		elif parameterComponent == self.PARAMETER_XYZ:
			# note: written as "x x y" (not "x y z") outside of [frames] and [keyframes],
			# exactly as the PHP version of this class did
			parameterValue = _php_str(newValue.x).replace('.', ',') \
				+ ' ' + _php_str(newValue.x).replace('.', ',') \
				+ ' ' + _php_str(newValue.y).replace('.', ',')

		if category in (self.CATEGORY_FRAMES, self.CATEGORY_KEYFRAMES):
			sequenceSettings = self.settingsSequence.setdefault(category, {})
			vals = sequenceSettings.setdefault('vals', {})
			if parameterComponent == self.PARAMETER_XYZ:
				self._setSequenceValue(vals, name + '_x', seq, _php_str(newValue.x).replace('.', ','))
				self._setSequenceValue(vals, name + '_y', seq, _php_str(newValue.y).replace('.', ','))
				self._setSequenceValue(vals, name + '_z', seq, _php_str(newValue.z).replace('.', ','))
			else:
				self._setSequenceValue(vals, name, seq, parameterValue)
			sequenceSettings['keys'] = list(vals.keys())
		else:
			self.settingsKeyValue.setdefault(category, {})[name] = parameterValue

	def GetSequenceCount(self, category):
		"""
		gets the count if frames in a sequence
		:param str category:
		:rtype: int
		"""
		sequenceSettings = self.settingsSequence[category]
		column = sequenceSettings['vals'][sequenceSettings['keys'][0]]
		return sum(1 for value in column if value is not _MISSING)

	# internal functions
	def _parseSettings(self):
		"""
		read all parameters from the internal settingsString
		:raises Exception:
		"""
		lines = self.settingsString.split('\n')
		category = ''
		for l, line in enumerate(lines):
			line = line.strip(_TRIM_CHARS)
			if _is_empty(line):
				continue

			if line[0] == '#':
				# line is a comment, only needed to get version
				match = re.match(r'^# version ([0-9.]+)$', line)
				if match:
					self.version = match.group(1)
				continue
			match = re.match(r'^\[([a-zA-Z0-9_]+)\]$', line)
			if match:
				# found a new category -> set it as the current category and proceed
				category = match.group(1)
				continue
			# found a settings line -> parse and read to proper location
			if _is_empty(category):
				raise ValueError('empty category on settings line: ' + str(l))
			self._parseLine(category, line, l)

	def _parseLine(self, category, line, l):
		"""
		:param str category:
		:param str line:
		:param int l:
		:raises Exception:
		"""
		if category in (self.CATEGORY_MAIN_PARAMETERS, self.CATEGORY_FRACTAL_1, self.CATEGORY_FRACTAL_2,
				self.CATEGORY_FRACTAL_3, self.CATEGORY_FRACTAL_4, self.CATEGORY_FRACTAL_5,
				self.CATEGORY_FRACTAL_6, self.CATEGORY_FRACTAL_7, self.CATEGORY_FRACTAL_8,
				self.CATEGORY_FRACTAL_9):
			match = re.match(r'^([a-zA-Z0-9_]+) (.+);$', line)
			if not match:
				raise ValueError('unparseable settings line: ' + str(l))
			self.settingsKeyValue.setdefault(category, {})[match.group(1)] = \
				match.group(2).strip(_TRIM_CHARS)
		elif category in (self.CATEGORY_FRAMES, self.CATEGORY_KEYFRAMES):
			seq = self.settingsSequence.setdefault(category, {})
			if not seq:
				seq['keys'] = line.split(';')
				seq['vals'] = dict((key, []) for key in seq['keys'])
			else:
				values = line.split(';')
				if values[0] == 'interpolation':
					pass  # TODO handle interpolation types
				else:
					if len(values) != len(seq['keys']):
						raise ValueError('Parsing ' + category + ' -> count mismatch'
							+ ' (values: ' + str(len(values)) + ' vs keys: ' + str(len(seq['keys'])) + ')')
					for i, val in enumerate(values):
						seq['vals'][seq['keys'][i]].append(val.strip(_TRIM_CHARS))
		# lines of other categories are ignored

	def _storeSettings(self):
		"""
		stores all dynamic data back into the settingsString for saving
		"""
		settingsString = []

		# create header
		settingsString.append('# Mandelbulber settings file\n')
		settingsString.append('# version ' + _php_str(self.version) + '\n')
		settingsString.append('# only modified parameters\n')

		# create key value category blocks
		for category, categorySettings in self.settingsKeyValue.items():
			if categorySettings:
				settingsString.append('[' + category + ']\n')
				for key, value in categorySettings.items():
					settingsString.append(key + ' ' + _php_str(value) + ';\n')
		# create sequence category blocks
		for category, sequenceSettings in self.settingsSequence.items():
			if sequenceSettings:
				settingsString.append('[' + category + ']\n')
				settingsString.append(';'.join(_php_str(key) for key in sequenceSettings['keys']) + '\n')
				count = self.GetSequenceCount(category)
				for i in range(count):
					row = [_php_str(self._sequenceValue(category, key, i)) for key in sequenceSettings['vals']]
					settingsString.append(';'.join(row) + '\n')

		self.settingsString = ''.join(settingsString)

	def _sequenceValue(self, category, name, seq):
		"""returns one entry of a sequence column, None if it is not set"""
		column = self.settingsSequence.get(category, {}).get('vals', {}).get(name, [])
		if 0 <= seq < len(column) and column[seq] is not _MISSING:
			return column[seq]
		return None

	@staticmethod
	def _setSequenceValue(vals, name, seq, value):
		"""sets one entry of a sequence column, the column grows as needed"""
		column = vals.setdefault(name, [])
		if seq >= len(column):
			column.extend([_MISSING] * (seq + 1 - len(column)))
		column[seq] = value

	# misc helper functions
	@staticmethod
	def Rand01():
		"""random number from 0 to 1 in steps of 1e-8 (module random, seed it with random.seed())"""
		return random.randint(0, 100000000) / 100000000


class Vector(object):
	"""
	Vector with the components x, y and z.
	The PHP accessors x(), y() and z() are the attributes x, y and z here,
	since a Python attribute and method cannot share a name.
	"""

	def __init__(self, x, y, z):
		"""
		Vector constructor.
		:param x:
		:param y:
		:param z:
		"""
		self.x = x
		self.y = y
		self.z = z

	def add(self, other):
		"""
		:param Vector other:
		"""
		self.x += other.x
		self.y += other.y
		self.z += other.z

	def scale(self, scalar):
		"""
		:param float scalar:
		"""
		self.x *= scalar
		self.y *= scalar
		self.z *= scalar

	def __repr__(self):
		return 'Vector(%r, %r, %r)' % (self.x, self.y, self.z)
