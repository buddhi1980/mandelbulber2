<?php
/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.       __           _       _       
 * Copyright (C) 2017 Mandelbulber Team   _>]|=||i=i<,    / _\ ___ _ __(_)_ __ | |_ ___ 
 *                                        \><||i|=>>%)    \ \ / __| '__| | '_ \| __/ __|
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    _\ \ (__| |  | | |_) | |_\__ \
 * The project is licensed under GPLv3,   -<>>=|><|||`    \__/\___|_|  |_| .__/ \__|___/
 * see also COPYING file in this folder.    ~+{i%+++                     |_|  
 *
 * This file contains the MbSettings class which enables
 * scripting modifications for Mandelbulber settings files.
 */
     
class MbSettings
{
	const SETTINGS_FROM_FILE = 1001;
	const SETTINGS_FROM_STRING = 1002;

	const SETTINGS_TO_FILE = 2001;
	const SETTINGS_TO_STRING = 2002;

	// treat parameter as raw string
	const PARAMETER_ALL = 3001;
	// treat parameter as a vector, transition to and from string will be done on saving and reading
	const PARAMETER_XYZ = 3002;

	const CATEGORY_FRAMES = 'frames';
	const CATEGORY_KEYFRAMES = 'keyframes';
	const CATEGORY_MAIN_PARAMETERS = 'main_parameters';
	const CATEGORY_FRACTAL_1 = 'fractal_1';
	const CATEGORY_FRACTAL_2 = 'fractal_2';
	const CATEGORY_FRACTAL_3 = 'fractal_3';
	const CATEGORY_FRACTAL_4 = 'fractal_4';
	const CATEGORY_FRACTAL_5 = 'fractal_5';
	const CATEGORY_FRACTAL_6 = 'fractal_6';
	const CATEGORY_FRACTAL_7 = 'fractal_7';
	const CATEGORY_FRACTAL_8 = 'fractal_8';
	const CATEGORY_FRACTAL_9 = 'fractal_9';

	var $settingsString;
	var $settingsKeyValue = array();
	var $settingsSequence = array();
	var $version;
	var $logLevel;

	/**
	 * MandelbulberSettings constructor.
	 * @param string $src
	 * @param int    $srcType
	 * @param string $logLevel QUIET|MEDIUM|VERBOSE
	 * @throws Exception
	 */
	function __construct($src, $srcType = self::SETTINGS_FROM_FILE, $logLevel = 'MEDIUM')
	{
		$this->logLevel = $logLevel;
		if ($srcType === self::SETTINGS_FROM_STRING)
			$this->settingsString = $src;
		else {
			if (!file_exists($src))
				throw new Exception('Cannot find settings file: ' . $src);
			$this->settingsString = file_get_contents($src);
			if (empty($this->settingsString))
				throw new Exception('File empty or access denied');
		}
		$this->parseSettings();
		
	}

	/**
	 * @param string $dest
	 * @param int    $destType
	 * @return string
	 * @throws Exception
	 */
	public function SaveSettings($dest = '', $destType = self::SETTINGS_TO_FILE)
	{
		$this->storeSettings();
		if ($destType === self::SETTINGS_TO_STRING) return $this->settingsString;

		if (!file_put_contents($dest, $this->settingsString))
			throw new Exception('Cannot put to file: ' . $dest);
		return '';
	}

	/**
	 * gets a given parameter from the settings
	 * @param string $category - name of the container (for example "main_parameters")
	 * @param string $name - name of the parameter (for example: "basic_fog_visibility")
	 * @param int    $seq - index of the sequence (needed for parameters in a sequence block [keyframes or flight])
	 * @param int    $parameterComponent - specifies the part of the parameter to take, see PARAMETER_* constants
	 * @return string|Vector
	 * @throws Exception
	 */
	public function Get($category, $name, $seq = 0, $parameterComponent = self::PARAMETER_ALL)
	{
		$parameterValue = null;
		if (in_array($category, array(self::CATEGORY_FRAMES, self::CATEGORY_KEYFRAMES))) {
			$sequenceSettings = &$this->settingsSequence[$category];
			if ($parameterComponent == self::PARAMETER_XYZ) {
				$parameterValue = $sequenceSettings['vals'][$name . '_x'][$seq]
					. ' ' . $sequenceSettings['vals'][$name . '_y'][$seq]
					. ' ' . $sequenceSettings['vals'][$name . '_z'][$seq];
			} else {
				$parameterValue = $sequenceSettings['vals'][$name][$seq];
			}
		} else {
			$parameterValue = $this->settingsKeyValue[$category][$name];
		}

		if ($parameterComponent === self::PARAMETER_ALL) {
			return $parameterValue;
		} elseif ($parameterComponent == self::PARAMETER_XYZ) {
			$vals = explode(' ', $parameterValue);
			return new Vector(
				floatval(str_replace(',', '.', $vals[0])),
				floatval(str_replace(',', '.', $vals[1])),
				floatval(str_replace(',', '.', $vals[2])));
		}
		throw new Exception('unknown $parameterComponent: ' . $parameterComponent);
	}

	/**
	 * sets a given value as the parameter for the settings
	 * @param mixed  $newValue - value of the parameter (for example "0.123")
	 * @param string $category - name of the container (for example "main_parameters")
	 * @param string $name - name of the parameter (for example: "basic_fog_visibility")
	 * @param int    $seq - index of the sequence (needed for parameters in a sequence block [keyframes or flight])
	 * @param int    $parameterComponent - specifies how to set the parameter from the value, see PARAMETER_* constants
	 */
	public function Set($newValue, $category, $name, $seq = 0, $parameterComponent = self::PARAMETER_ALL)
	{
		$parameterValue = '';
		if ($parameterComponent === self::PARAMETER_ALL) {
			$parameterValue = $newValue;
		} elseif ($parameterComponent == self::PARAMETER_XYZ) {
			$parameterValue = str_replace('.', ',', $newValue->x)
				. ' ' . str_replace('.', ',', $newValue->x)
				. ' ' . str_replace('.', ',', $newValue->y);
		}

		if (in_array($category, array(self::CATEGORY_FRAMES, self::CATEGORY_KEYFRAMES))) {
			$sequenceSettings = &$this->settingsSequence[$category];
			if ($parameterComponent == self::PARAMETER_XYZ) {
				$sequenceSettings['vals'][$name . '_x'][$seq] = str_replace('.', ',', $newValue->x);
				$sequenceSettings['vals'][$name . '_y'][$seq] = str_replace('.', ',', $newValue->y);
				$sequenceSettings['vals'][$name . '_z'][$seq] = str_replace('.', ',', $newValue->z);
			} else {
				$sequenceSettings['vals'][$name][$seq] = $parameterValue;
			}
			$sequenceSettings['keys'] = array_keys($this->settingsSequence[$category]['vals']);
		} else {
			$this->settingsKeyValue[$category][$name] = $parameterValue;
		}
	}

	/**
	 * gets the count if frames in a sequence
	 * @param $category
	 * @return int
	 */
	public function GetSequenceCount($category)
	{
		$sequenceSettings = &$this->settingsSequence[$category];
		return count($sequenceSettings['vals'][$sequenceSettings['keys'][0]]);
	}


	/* internal functions */
	/**
	 * read all parameters from the internal $settingsString
	 * @throws Exception
	 */
	private function parseSettings()
	{
		$lines = explode(PHP_EOL, $this->settingsString);
		$category = '';
		foreach ($lines as $l => $line) {
			$line = trim($line);
			if (empty($line)) continue;

			if (substr($line, 0, 1) == '#') {
				// line is a comment, only needed to get version
				if (preg_match('/^# version ([\d\.]+)$/', $line, $match)) {
					$this->version = $match[1];
				}
				continue;
			} else if (preg_match('/^\[([a-zA-Z0-9_]+)\]$/', $line, $match)) {
				// found a new category -> set it as the current category and proceed
				$category = $match[1];
				continue;
			} else {
				// found a settings line -> parse and read to proper location
				if (empty($category)) throw new Exception('empty category on settings line: ' . $l);
				$this->parseLine($category, $line, $l);
			}

		}
	}

	/**
	 * @param string $category
	 * @param string $line
	 * @param int $l
	 * @throws Exception
	 */
	private function parseLine($category, $line, $l)
	{
		switch ($category) {
			case self::CATEGORY_MAIN_PARAMETERS:
			case self::CATEGORY_FRACTAL_1:
			case self::CATEGORY_FRACTAL_2:
			case self::CATEGORY_FRACTAL_3:
			case self::CATEGORY_FRACTAL_4:
			case self::CATEGORY_FRACTAL_5:
			case self::CATEGORY_FRACTAL_6:
			case self::CATEGORY_FRACTAL_7:
			case self::CATEGORY_FRACTAL_8:
			case self::CATEGORY_FRACTAL_9:
				if (!preg_match('/^([a-zA-Z0-9_]+) (.+);$/', $line, $match))
					throw new Exception('unparseable settings line: ' . $l);
				if (!array_key_exists($category, $this->settingsKeyValue))
					$this->settingsKeyValue[$category] = array();
				$this->settingsKeyValue[$category][$match[1]] = trim($match[2]);
				break;
			case self::CATEGORY_FRAMES:
			case self::CATEGORY_KEYFRAMES:
				if (empty($this->settingsSequence[$category])) {
					$this->settingsSequence[$category] = array();
				}
				$seq = &$this->settingsSequence[$category];
				if (empty($seq)) {
					$seq['keys'] = explode(';', $line);
					$seq['vals'] = array_fill_keys($seq['keys'], array());
				} else {
					$values = explode(';', $line);
					if($values[0] == 'interpolation'){
						// TODO handle interpolation types
					}else{
						if (count($values) != count($seq['keys']))
							throw new Exception('Parsing ' . $category . ' -> count mismatch'
								. ' (values: ' . count($values) . ' vs keys: ' . count($seq['keys']) . ')');
						foreach ($values as $i => $val) {
							$seq['vals'][$seq['keys'][$i]][] = trim($val);
						}
					}
				}
				break;
		}
	}


	/**
	 * stores all dynamic data back into the $settingsString for saving
	 */
	private function storeSettings()
	{
		$settingsString = '';

		// create header
		$settingsString .= '# Mandelbulber settings file' . PHP_EOL;
		$settingsString .= '# version ' . $this->version . PHP_EOL;
		$settingsString .= '# only modified parameters' . PHP_EOL;

		// create key value category blocks
		foreach ($this->settingsKeyValue as $category => $categorySettings) {
			if (!empty($categorySettings)) {
				$settingsString .= '[' . $category . ']' . PHP_EOL;
				foreach ($categorySettings as $key => $value) {
					$settingsString .= $key . ' ' . $value . ';' . PHP_EOL;
				}
			}
		}
		// create sequence category blocks
		foreach ($this->settingsSequence as $category => $sequenceSettings) {
			if (!empty($sequenceSettings)) {
				$settingsString .= '[' . $category . ']' . PHP_EOL;
				$settingsString .= implode(';', $sequenceSettings['keys']) . PHP_EOL;
				$count = $this->GetSequenceCount($category);
				for ($i = 0; $i < $count; $i++) {
					foreach ($sequenceSettings['vals'] as $val) {
						$settingsString .= $val[$i] . ';';
					}
					$settingsString = substr($settingsString, 0, -1) . PHP_EOL;
				}
			}
		}

		$this->settingsString = $settingsString;
	}



	// misc helper functions
	public static function Rand01()
	{
		return mt_rand(0, 100000000) / 100000000;
	}

}


class Vector
{
	var $x = 0;
	var $y = 0;
	var $z = 0;

	/**
	 * Vector constructor.
	 * @param $x
	 * @param $y
	 * @param $z
	 */
	public function __construct($x, $y, $z)
	{
		$this->x = $x;
		$this->y = $y;
		$this->z = $z;
	}

	/**
	 * @param Vector $other
	 */
	public function add($other)
	{
		$this->x += $other->x;
		$this->y += $other->y;
		$this->z += $other->z;
	}

	/**
	 * @param float $scalar
	 */
	public function scale($scalar)
	{
		$this->x *= $scalar;
		$this->y *= $scalar;
		$this->z *= $scalar;
	}

	public function x(){ return $this->x; }
	public function y(){ return $this->y; }
	public function z(){ return $this->z; }
}

?>
